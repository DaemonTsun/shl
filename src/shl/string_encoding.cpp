
#include "shl/memory.hpp" // copy_memory
#include "shl/compare.hpp" // Min
#include "shl/string_encoding.hpp"

static constexpr char _utf8_lengths[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
};

extern "C"
{
// strings must be zero padded to four bytes
const c8 *utf8_decode (const c8 *str, u32 *cp, int *error)
{
    // https://nullprogram.com/blog/2017/10/06/
    static constexpr const s32 masks[]  = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static constexpr const u32 mins[]   = {4194304, 0, 128, 2048, 65536};
    static constexpr const s32 shiftc[] = {0, 18, 12, 6, 0};
    static constexpr const s32 shifte[] = {0, 6, 4, 2, 0};

    const u8 *s = (const u8 *)str;
    int len = _utf8_lengths[s[0] >> 3];
    const u8 *next = s + len + !len;

    /* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
    *cp  = (u32)(s[0] & masks[len]) << 18;
    *cp |= (u32)(s[1] & 0x3f) << 12;
    *cp |= (u32)(s[2] & 0x3f) <<  6;
    *cp |= (u32)(s[3] & 0x3f) <<  0;
    *cp >>= shiftc[len];

    /* Accumulate the various error conditions. */
    *error  = (*cp < mins[len]) << 6; // non-canonical encoding
    *error |= ((*cp >> 11) == 0x1b) << 7;  // surrogate half?
    *error |= (*cp > 0x10FFFF) << 8;  // out of range?
    *error |= (s[1] & 0xc0) >> 2;
    *error |= (s[2] & 0xc0) >> 4;
    *error |= (s[3]       ) >> 6;
    *error ^= 0x2a; // top two bits of each tail byte correct?
    *error >>= shifte[len];

    return (const c8*)next;
}

const c16 *utf16_decode(const c16 *str, u32 *cp, int *error)
{
    u16 high = (u16)*str;

    // I'm not sure if this is possible branchless
    if ((high & UTF16_SURROGATE_MASK) != UTF16_SURROGATE_HIGH)
    {
        *cp = (u32)high & 0x0000ffff;
        return str + 1;
    }

    *error = ((high & UTF16_SURROGATE_HIGH_LOW_MASK) ^ UTF16_SURROGATE_HIGH);

    u16 low = (u16)*(str + 1);

    *error |= ((low & UTF16_SURROGATE_HIGH_LOW_MASK) ^ UTF16_SURROGATE_LOW);

    *cp =  (high & UTF16_CODEPOINT_MASK) << 10;
    *cp |= (low & UTF16_CODEPOINT_MASK);
    *cp += UTF16_CODEPOINT_OFFSET;

    return str + 2;
}

const c32 *utf32_decode(const c32 *str, u32 *cp, int *error)
{
    *cp = (u32)*str;

    if (*cp > UNICODE_MAX)
        *error = 1;

    return str + 1;
}

static inline s64 _utf8_decode_string(const c8 *u8str, const c8 **end, s64 u8str_size, u32 *out, s64 out_length)
{
    s64 i = 0;
    int err = 0;
    const c8 *s = u8str;

    while (err == 0 && (s - u8str < u8str_size) && *s != '\0' && i < out_length)
    {
        s = utf8_decode(s, out + i, &err);
        i += 1;
    }

    if (err != 0)
        return -1;

    if (end != nullptr)
        *end = s;

    return i;
}

s64 utf8_decode_string(const c8 *u8str, s64 u8str_size, u32 *out, s64 out_length)
{
    return _utf8_decode_string(u8str, nullptr, u8str_size, out, out_length);
}

static inline s64 _utf16_decode_string(const c16 *u16str, const c16 **end, s64 u16str_size, u32 *out, s64 out_length)
{
    s64 i = 0;
    int err = 0;
    const c16 *s = u16str;

    while (err == 0 && (s - u16str < u16str_size) && *s != (c16)'\0' && i < out_length)
    {
        s = utf16_decode(s, out + i, &err);
        i += 1;
    }

    if (err != 0)
        return -1;

    if (end != nullptr)
        *end = s;

    return i;
}

s64 utf16_decode_string(const c16 *u16str, s64 u16str_size, u32 *out, s64 out_length)
{
    return _utf16_decode_string(u16str, nullptr, u16str_size, out, out_length);
}

static inline s64 _utf32_decode_string(const c32 *u32str, const c32 **end, s64 u32str_size, u32 *out, s64 out_length)
{
    s64 i = 0;
    int err = 0;
    const c32 *s = u32str;

    while (err == 0 && (s - u32str < u32str_size) && *s != (c32)'\0' && i < out_length)
    {
        s = utf32_decode(s, out + i, &err);
        i += 1;
    }

    if (err != 0)
        return -1;

    if (end != nullptr)
        *end = s;

    return i;
}

s64 utf32_decode_string(const c32 *u32str, s64 u32str_size, u32 *out, s64 out_length)
{
    return _utf32_decode_string(u32str, nullptr, u32str_size, out, out_length);
}

s64 utf8_decode_string_safe (const c8 *u8str, s64 u8str_size, u32 *out, s64 out_size)
{
    s64 ret = 0;

    if (u8str_size > 4)
    {
        const c8 *start = u8str;
        ret = _utf8_decode_string(start, &u8str, u8str_size - 4, out, out_size);

        if (ret < 0)
            return ret;

        u8str_size -= (s64)(u8str - start);

        if (u8str_size == 0)
            return ret;

        if (out_size - ret <= 0)
            return ret;

        out += ret;
        out_size -= ret;
    }

    c8 _rest_to_decode[8] = {0};
    const c8 *rest_to_decode = _rest_to_decode;
    int err = 0;

    for (int i = 0; i < u8str_size; ++i)
        _rest_to_decode[i] = u8str[i];

    while (*rest_to_decode != '\0' && err == 0)
    {
        rest_to_decode = utf8_decode(rest_to_decode, out++, &err);
        ret++;
    }

    if (err != 0)
        return -1;

    return ret;
}

s64 utf16_decode_string_safe(const c16 *u16str, s64 u16str_size, u32 *out, s64 out_size)
{
    s64 ret = 0;

    if (u16str_size > 2)
    {
        const c16 *start = u16str;
        ret = _utf16_decode_string(start, &u16str, u16str_size - 2, out, out_size);

        if (ret < 0)
            return ret;

        u16str_size -= (s64)(u16str - start);

        if (u16str_size == 0)
            return ret;

        if (out_size - ret <= 0)
            return ret;

        out += ret;
        out_size -= ret;
    }

    c16 _rest_to_decode[4] = {0};
    const c16 *rest_to_decode = _rest_to_decode;
    int err = 0;

    for (int i = 0; i < u16str_size; ++i)
        _rest_to_decode[i] = u16str[i];

    while (*rest_to_decode != '\0' && err == 0)
    {
        rest_to_decode = utf16_decode(rest_to_decode, out++, &err);
        ret++;
    }

    if (err != 0)
        return -1;

    return ret;
}

s64 utf32_decode_string_safe(const c32 *u32str, s64 u32str_size, u32 *out, s64 out_length)
{
    // Nothing to check here I think, but let's keep this here just in case,
    // so that the API doesn't change.
    return _utf32_decode_string(u32str, nullptr, u32str_size, out, out_length);
}

s64 utf8_encode(u32 cp, c8 *out)
{
    if (cp <= UTF8_1_MAX)
    {
        out[0] = (c8)cp;
        return 1;
    }
    else if (cp <= UTF8_2_MAX)
    {
        out[0] = (c8)(((cp >> 6) & 0x1f) | 0xc0);
        out[1] = (c8)(((cp >> 0) & 0x3f) | 0x80);
        return 2;
    }
    else if (cp <= UTF8_3_MAX)
    {
        out[0] = (c8)(((cp >> 12) & 0x0f) | 0xe0);
        out[1] = (c8)(((cp >>  6) & 0x3f) | 0x80);
        out[2] = (c8)(((cp >>  0) & 0x3f) | 0x80);
        return 3;
    }
    else if (cp <= UTF8_4_MAX)
    {
        out[0] = (c8)(((cp >> 18) & 0x07) | 0xf0);
        out[1] = (c8)(((cp >> 12) & 0x3f) | 0x80);
        out[2] = (c8)(((cp >>  6) & 0x3f) | 0x80);
        out[3] = (c8)(((cp >>  0) & 0x3f) | 0x80);
        return 4;
    }

    // error
    return -1;
}

s64 utf16_encode(u32 cp, c16 *out)
{
    if (cp <= UTF16_1_MAX)
    {
        out[0] = (c16)cp;
        return 1;
    }
    else if (cp <= UNICODE_MAX)
    {
        cp -= UTF16_CODEPOINT_OFFSET;

        out[0] = (c16)((cp >> 10) & UTF16_CODEPOINT_MASK) | UTF16_SURROGATE_HIGH;
        out[1] = (c16)(cp         & UTF16_CODEPOINT_MASK) | UTF16_SURROGATE_LOW;

        return 2;
    };

    return -1;
}

s64 utf32_encode(u32 cp, c32 *out)
{
    if (cp <= UNICODE_MAX)
    {
        *out = (c32)cp;

        return 1;
    };

    return -1;
}

s64 utf8_encode_string(const u32 *cps, s64 cp_count, c8 *out, s64 out_size)
{
    s64 i = 0;
    s64 size_left = out_size;
    s64 size_required = 0;
    c8 *start = out;
    u32 cp = 0;

    while (i < cp_count)
    {
        cp = cps[i];

        if (cp == 0)
            break;

        size_required = codepoint_utf8_length(cp);

        if (size_left < size_required)
            break;

        if (utf8_encode(cp, out) != size_required)
            return -1;

        i += 1;
        size_left -= size_required;
        out += size_required;
    }

    return (s64)(out - start);
}

s64 utf16_encode_string(const u32 *cps, s64 cp_count, c16 *out, s64 out_size)
{
    s64 i = 0;
    s64 size_left = out_size;
    s64 size_required = 0;
    c16 *start = out;
    u32 cp = 0;

    while (i < cp_count)
    {
        cp = cps[i];

        if (cp == 0)
            break;

        size_required = codepoint_utf16_length(cp);

        if (size_left < size_required)
            break;

        if (utf16_encode(cp, out) != size_required)
            return -1;

        i += 1;
        size_left -= size_required;
        out += size_required;
    }

    return (s64)(out - start);
}

s64 utf32_encode_string(const u32 *cps, s64 cp_count, c32 *out, s64 out_size)
{
    s64 max_size = Min(cp_count, out_size);

    for (s64 i = 0; i < max_size; ++i)
        out[i] = (c32)cps[i];

    return max_size;
}

s64 utf8_to_utf16(const c8 *u8str, s64 u8str_size,  c16  *out, s64 out_size)
{
    s64 size_required = 0;
    s64 u16_index = 0;
    int err = 0;
    const c8 *s = u8str;

    u32 cp = 0;

    while (true)
    {
        if (s == nullptr || (s64)(s - u8str) >= u8str_size)
            break;

        if (*s == '\0')
            break;

        if (u16_index >= out_size)
            break;

        s = utf8_decode(s, &cp, &err);

        if (err != 0)
            break;

        size_required = codepoint_utf16_length(cp);

        if (out_size - u16_index < size_required)
            break;

        if (utf16_encode(cp, out + u16_index) != size_required)
        {
            err = 1;
            break;
        }

        u16_index += size_required;
    }

    if (err != 0)
        return -1;

    return u16_index;
}

s64 utf8_to_utf32(const c8 *u8str, s64 u8str_size,  c32 *out, s64 out_size)
{
    return utf8_decode_string(u8str, u8str_size, (u32*)out, out_size);
}

s64 utf16_to_utf8(const c16 *u16str, s64 u16str_size, char *out, s64 out_size)
{
    s64 size_required = 0;
    s64 u8_index = 0;
    int err = 0;
    const c16 *s = u16str;

    u32 cp = 0;

    while (true)
    {
        if (s == nullptr || (s64)(s - u16str) >= u16str_size)
            break;

        if (*s == 0)
            break;

        if (u8_index >= out_size)
            break;

        s = utf16_decode(s, &cp, &err);

        if (err != 0)
            break;

        size_required = codepoint_utf8_length(cp);

        if (out_size - u8_index < size_required)
            break;

        if (utf8_encode(cp, out + u8_index) != size_required)
        {
            err = 1;
            break;
        }

        u8_index += size_required;
    }

    if (err != 0)
        return -1;

    return u8_index;
}

s64 utf16_to_utf32(const c16 *u16str, s64 u16str_size, c32 *out, s64 out_size)
{
    return utf16_decode_string(u16str, u16str_size, (u32*)out, out_size);
}

s64 utf32_to_utf8(const c32 *u32str, s64 u32str_size, c8 *out, s64 out_size)
{
    return utf8_encode_string((const u32*)u32str, u32str_size, out, out_size);
}

s64 utf32_to_utf16(const c32 *u32str, s64 u32str_size, c16 *out, s64 out_size)
{
    return utf16_encode_string((const u32*)u32str, u32str_size, out, out_size);
}

s64 utf8_units_required_from_utf16(const c16 *u16str, s64 u16str_size)
{
    return utf8_bytes_required_from_utf16(u16str, u16str_size);
}

s64 utf8_units_required_from_utf32(const c32 *u32str, s64 u32str_size)
{
    return utf8_bytes_required_from_utf32(u32str, u32str_size);
}

s64 utf16_units_required_from_utf8(const c8  *u8str,  s64 u8str_size)
{
    return utf16_bytes_required_from_utf8(u8str, u8str_size) / sizeof(c16);
}

s64 utf16_units_required_from_utf32(const c32 *u32str, s64 u32str_size)
{
    return utf16_bytes_required_from_utf32(u32str, u32str_size) / sizeof(c16);
}

s64 utf32_units_required_from_utf8(const c8  *u8str,  s64 u8str_size)
{
    return utf32_bytes_required_from_utf8(u8str, u8str_size) / sizeof(c32);
}

s64 utf32_units_required_from_utf32(const c16 *u16str, s64 u16str_size)
{
    return utf32_bytes_required_from_utf16(u16str, u16str_size) / sizeof(c32);
}

s64 utf8_bytes_required_from_utf16(const c16 *u16str, s64 u16str_size)
{
    s64 sz = 0;
    u16 high = 0;

    while (u16str_size > 0 && *u16str != 0)
    {
        high = (u16)*u16str;

        if ((high & UTF16_SURROGATE_MASK) != UTF16_SURROGATE_HIGH)
        {
            if      (high <= UTF8_1_MAX) sz += 1;
            else if (high <= UTF8_2_MAX) sz += 2;
            else                         sz += 3;

            u16str += 1;
            u16str_size -= 1;
        }
        else
        {
            sz += 4;
            u16str += 2;
            u16str_size -= 2;
        }
    }

    return sz;
}

s64 utf8_bytes_required_from_utf32(const c32 *u32str, s64 u32str_size)
{
    return utf8_bytes_required_from_codepoints((const u32*)u32str, u32str_size);
}

s64 utf16_bytes_required_from_utf8(const c8 *u8str, s64 u8str_size)
{
    s64 sz = 0;

    while (u8str_size > 0 && *u8str != '\0')
    {
        if ((u8)(*u8str) - 0x80u >= 0x40u) sz += 1;
        if ((u8)(*u8str) >= 0xf0u)         sz += 1;

        u8str_size -= 1;
        u8str++;
    }

    return sz * sizeof(c16);
}

s64 utf16_bytes_required_from_utf32(const c32 *u32str, s64 u32str_size)
{
    return utf16_bytes_required_from_codepoints((const u32*)u32str, u32str_size);
}

s64 utf32_bytes_required_from_utf8(const c8 *u8str, s64 u8str_size)
{
    s64 sz = 0;
    const c8 *start = u8str;
    const c8 *s = start;
    int len = 0;

    while ((s64)(s - start) < u8str_size && *s != 0u)
    {
        len = _utf8_lengths[s[0] >> 3];
        s = s + len + !len;
        sz += sizeof(c32);
    }

    return sz;
}

s64 utf32_bytes_required_from_utf16(const c16 *u16str, s64 u16str_size)
{
    s64 sz = 0;
    c16 high = 0;

    while (u16str_size > 0 && *u16str != 0)
    {
        high = *(const u16*)u16str;
        sz += sizeof(c32);

        if ((high & UTF16_SURROGATE_MASK) != UTF16_SURROGATE_HIGH)
        {
            u16str += 1;
            u16str_size -= 1;
        }
        else
        {
            u16str += 2;
            u16str_size -= 2;
        }
    }

    return sz;
}

s64 utf8_bytes_required_from_codepoints(const u32 *cps, s64 cp_count)
{
    s64 sz = 0;

    while (cp_count > 0 && *cps != 0)
    {
        sz += codepoint_utf8_length(*cps);
        cp_count -= 1;
        cps++;
    }

    return sz;
}

s64 utf16_bytes_required_from_codepoints(const u32 *cps, s64 cp_count)
{
    s64 sz = 0;

    while (cp_count > 0 && *cps != 0)
    {
        sz += codepoint_utf16_length(*cps);
        cp_count -= 1;
        cps++;
    }

    return sz * sizeof(c16);
}

s64 utf32_bytes_required_from_codepoints(const u32 *cps, s64 cp_count)
{
    s64 sz = 0;

    while (cp_count > 0 && *cps != 0)
    {
        sz += codepoint_utf32_length(*cps);
        cp_count -= 1;
        cps++;
    }

    return sz * sizeof(c32);
}

} // extern C

s64 string_convert(const c8 *u8str, s64 u8str_size, c16 *u16str, s64 u16str_size)
{
    return utf8_to_utf16(u8str, u8str_size, u16str, u16str_size);
}

s64 string_convert(const c8 *u8str, s64 u8str_size, c32 *u32str, s64 u32str_size)
{
    return utf8_to_utf32(u8str, u8str_size, u32str, u32str_size);
}

s64 string_convert(const c16 *u16str, s64 u16str_size, c8 *u8str, s64 u8str_size)
{
    return utf16_to_utf8(u16str, u16str_size, u8str, u8str_size);
}

s64 string_convert(const c16 *u16str, s64 u16str_size, c32 *u32str, s64 u32str_size)
{
    return utf16_to_utf32(u16str, u16str_size, u32str, u32str_size);
}

s64 string_convert(const c32 *u32str, s64 u32str_size, c8 *u8str, s64 u8str_size)
{
    return utf32_to_utf8(u32str, u32str_size, u8str, u8str_size);
}

s64 string_convert(const c32 *u32str, s64 u32str_size, c16 *u16str, s64 u16str_size)
{
    return utf32_to_utf16(u32str, u32str_size, u16str, u16str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c8  *, const c16 *u16str, s64 u16str_size)
{
    return utf8_bytes_required_from_utf16(u16str, u16str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c8  *, const c32 *u32str, s64 u32str_size)
{
    return utf8_bytes_required_from_utf32(u32str, u32str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c16 *, const c8  *u8str,  s64 u8str_size)
{
    return utf16_bytes_required_from_utf8(u8str, u8str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c16 *, const c32 *u32str, s64 u32str_size)
{
    return utf16_bytes_required_from_utf32(u32str, u32str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c32 *, const c8  *u8str,  s64 u8str_size)
{
    return utf32_bytes_required_from_utf8(u8str, u8str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c32 *, const c16 *u16str, s64 u16str_size)
{
    return utf32_bytes_required_from_utf16(u16str, u16str_size);
}

/*
 * REMOVE THESE
 */
s64 string_convert(const c8  *u8str,  s64 u8str_size,  wchar_t *wstr, s64 wstr_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_convert(u8str, u8str_size, (c16*)wstr, wstr_size);
    else
        return string_convert(u8str, u8str_size, (c32*)wstr, wstr_size);
}

s64 string_convert(const c16 *u16str, s64 u16str_size, wchar_t *wstr, s64 wstr_size)
{
    if constexpr (sizeof(wchar_t) == 2)
    {
        s64 count = Min(u16str_size, wstr_size);
        copy_memory((const void*)u16str, (void*)wstr, count * sizeof(c16));
        return count;
    }
    else
        return string_convert(u16str, u16str_size, (c32*)wstr, wstr_size);
}

s64 string_convert(const c32 *u32str, s64 u32str_size, wchar_t *wstr, s64 wstr_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_convert(u32str, u32str_size, (c16*)wstr, wstr_size);
    else
    {
        s64 count = Min(u32str_size, wstr_size);
        copy_memory((const void*)u32str, (void*)wstr, count * sizeof(c32));
        return count;
    }
}

s64 string_convert(const wchar_t *wstr, s64 wstr_size, c8  *u8str,  s64 u8str_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_convert((const c16*)wstr, wstr_size, u8str, u8str_size);
    else
        return string_convert((const c32*)wstr, wstr_size, u8str, u8str_size);
}

s64 string_convert(const wchar_t *wstr, s64 wstr_size, c16 *u16str, s64 u16str_size)
{
    if constexpr (sizeof(wchar_t) == 2)
    {
        s64 count = Min(u16str_size, wstr_size);
        copy_memory((const void*)wstr, (void*)u16str, count * sizeof(c16));
        return count;
    }
    else
        return string_convert((const c32*)wstr, wstr_size, u16str, u16str_size);
}

s64 string_convert(const wchar_t *wstr, s64 wstr_size, c32 *u32str, s64 u32str_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_convert((const c16*)wstr, wstr_size, u32str, u32str_size);
    else
    {
        s64 count = Min(u32str_size, wstr_size);
        copy_memory((const void*)wstr, (void*)u32str, count * sizeof(c32));
        return count;
    }
}

s64 string_conversion_bytes_required([[maybe_unused]] c8  *x,  const wchar_t *wstr, s64 wstr_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_conversion_bytes_required(x, (const c16*)wstr, wstr_size);
    else
        return string_conversion_bytes_required(x, (const c32*)wstr, wstr_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c16 *x, const wchar_t *wstr, s64 wstr_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return wstr_size;
    else
        return string_conversion_bytes_required(x, (const c32*)wstr, wstr_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] c32 *x, const wchar_t *wstr, s64 wstr_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_conversion_bytes_required(x, (const c16*)wstr, wstr_size);
    else
        return wstr_size;
}

s64 string_conversion_bytes_required([[maybe_unused]] wchar_t *x, const c8  *u8str,  s64 u8str_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_conversion_bytes_required((c16*)x, u8str, u8str_size);
    else
        return string_conversion_bytes_required((c32*)x, u8str, u8str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] wchar_t *x, const c16 *u16str, s64 u16str_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return u16str_size;
    else
        return string_conversion_bytes_required((c32*)x, u16str, u16str_size);
}

s64 string_conversion_bytes_required([[maybe_unused]] wchar_t *x, const c32 *u32str, s64 u32str_size)
{
    if constexpr (sizeof(wchar_t) == 2)
        return string_conversion_bytes_required((c16*)x, u32str, u32str_size);
    else
        return u32str_size;
}
