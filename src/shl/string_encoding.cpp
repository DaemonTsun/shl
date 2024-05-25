
#include "shl/string_encoding.hpp"

static constexpr char utf8_lengths[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
};

// strings must be zero padded to four bytes
const char *utf8_decode (const char *str, u32 *cp, int *error)
{
    // https://nullprogram.com/blog/2017/10/06/
    static constexpr const s32 masks[]  = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static constexpr const u32 mins[]   = {4194304, 0, 128, 2048, 65536};
    static constexpr const s32 shiftc[] = {0, 18, 12, 6, 0};
    static constexpr const s32 shifte[] = {0, 6, 4, 2, 0};

    const u8 *s = (const u8 *)str;
    int len = utf8_lengths[s[0] >> 3];
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

    return (const char*)next;
}

const u16  *utf16_decode(const u16  *str, u32 *cp, int *error)
{
    u16 high = *str;

    // I'm not sure if this is possible branchless
    if ((high & UTF16_SURROGATE_MASK) != UTF16_SURROGATE_HIGH)
    {
        *cp = (u32)high & 0x0000ffff;
        return str + 1; 
    }

    *error = ((high & UTF16_SURROGATE_HIGH_LOW_MASK) ^ UTF16_SURROGATE_HIGH);
    
    u16 low = *(str + 1);

    *error |= ((low & UTF16_SURROGATE_HIGH_LOW_MASK) ^ UTF16_SURROGATE_LOW);

    *cp =  (high & UTF16_CODEPOINT_MASK) << 10;
    *cp |= (low & UTF16_CODEPOINT_MASK);
    *cp += UTF16_CODEPOINT_OFFSET;
    
    return str + 2;
}

s64 utf8_decode_string(const char *u8str, s64 str_buf_size, u32 *out, s64 out_length)
{
    s64 i = 0;
    int err = 0;
    const char *s = u8str;

    while (err == 0 && (s - u8str < str_buf_size) && *s != '\0' && i < out_length)
    {
        s = utf8_decode(s, out + i, &err);
        i += 1;
    }
    
    if (err != 0)
        return -1;

    return i;
}

s64 utf16_decode_string(const u16 *u16str, s64 str_buf_size, u32 *out, s64 out_length)
{
    s64 i = 0;
    int err = 0;
    const u16 *s = u16str;

    while (err == 0 && (s - u16str < str_buf_size) && *s != '\0' && i < out_length)
    {
        s = utf16_decode(s, out + i, &err);
        i += 1;
    }
    
    if (err != 0)
        return -1;

    return i;
}

s64 utf8_decode_string_safe (const char *u8str, s64 str_buf_size, u32 *out, s64 out_size)
{
    s64 str_buf_size4 = str_buf_size & S64_LIT(-4);
    s64 diff = str_buf_size - str_buf_size4;
    s64 ret = utf8_decode_string(u8str, str_buf_size4, out, out_size);

    if (ret < 0)
        return ret;

    if (diff == 0)
        return ret;

    if (out_size - ret <= 0)
        return ret;

    out += ret;
    out_size -= ret;

    char rest_to_decode[4] = {0};
    u8str += str_buf_size4;

    for (s64 i = 0; i < diff; ++i)
        rest_to_decode[i] = u8str[i];

    int err = 0;
    utf8_decode(rest_to_decode, out, &err);

    if (err != 0)
        return -1;

    return ret + 1;
}

s64 utf16_decode_string_safe(const u16 *u16str, s64 str_buf_size, u32 *out, s64 out_size)
{
    // -2 because u16 is 2 bytes, 2*2 = 4
    s64 str_buf_size4 = str_buf_size & S64_LIT(-2);
    s64 diff = str_buf_size - str_buf_size4;
    s64 ret = utf16_decode_string(u16str, str_buf_size4, out, out_size);

    if (ret < 0)
        return ret;

    if (diff == 0)
        return ret;

    if (out_size - ret <= 0)
        return ret;

    out += ret;
    out_size -= ret;

    u16 rest_to_decode[2] = {u16str[str_buf_size4], 0};
    int err = 0;

    utf16_decode(rest_to_decode, out, &err);

    if (err != 0)
        return -1;

    return ret + 1;
}
