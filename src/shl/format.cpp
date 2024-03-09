
#include <stdlib.h> // wcstombs
#include "shl/at_exit.hpp"
#include "shl/bits.hpp"
#include "shl/format.hpp"

#define LIT(C, Literal)\
    inline_const_if(is_same(C, char), Literal, L##Literal)

#define as_array_ptr(C, str) (array<C>*)(str)

static const char *hex_letters      = "0123456789abcdef";
static const char *hex_letters_caps = "0123456789ABCDEF";

template<typename C>
s64 _copy_string_reverse(const C *src, C *dst, u64 size)
{
    u64 i = size;
	while (i > 0)
    {
        *dst = src[i - 1];
        i--;
        dst++;
	}

    return size;
}

template<typename C>
s64 _copy_string_reverse_checked(const C *src, u64 src_size, C *dst, u64 dst_size)
{
    u64 ssize = 0;

    if (dst_size < src_size)
    {
        // we advance src here because src is in reverse.
        // if dst_size is smaller, we would be copying src from the wrong end.
        src += (src_size - dst_size);
        ssize = dst_size;
    }
    else
        ssize = src_size;

    return _copy_string_reverse(src, dst, ssize);
}

template<typename C>
s64 _copy_string_reverse_checked(const C *src, u64 src_size, C *dst, u64 dst_size, u64 dst_offset)
{
    if (dst_offset >= dst_size)
        return 0;

    return _copy_string_reverse_checked(src, src_size, dst + dst_offset, dst_size - dst_offset);
}

template<typename C>
s64 _copy_string_checked(const C *src, u64 src_size, C *dst, u64 dst_size)
{
    if (src == nullptr)
    {
        src = LIT(C, "<null>");
        src_size = 6;
    }

    u64 ssize = Min(src_size, dst_size);
    C *ptr = copy_string(src, dst, ssize);

    if (ptr == nullptr)
        return 0;

    return ptr - dst;
}

template<typename C>
s64 _copy_string_checked(const C *src, u64 src_size, C *dst, u64 dst_size, u64 dst_offset)
{
    if (dst_offset >= dst_size)
        return 0;

    return _copy_string_checked(src, src_size, dst + dst_offset, dst_size - dst_offset);
}

template<typename C>
s64 _pad_string_pc(C *s, u64 ssize, C chr, s64 count, u64 offset)
{
    if (count <= 0)
        return 0;

    if (count + offset > ssize)
        count = (s64)ssize - (s64)offset;

    if (count <= 0)
        return 0;

    for (s64 i = 0; i < count; ++i)
        s[i + offset] = chr;

    return count;
}

s64 pad_string(char    *s, u64 ssize, char    chr, s64 count, u64 offset)
{
    return _pad_string_pc(s, ssize, chr, count, offset);
}

s64 pad_string(wchar_t *s, u64 ssize, wchar_t chr, s64 count, u64 offset)
{
    return _pad_string_pc(s, ssize, chr, count, offset);
}

template<typename C>
s64 _pad_string_s(string_base<C> *s, C chr, s64 count, u64 offset)
{
    if (count <= 0)
        return 0;

    string_reserve(s, count + offset);

    for (s64 i = 0; i < count; ++i)
        s->data[i + offset] = chr;

    if (offset + count > s->size)
    {
        s->size = offset + count;
        s->data[s->size] = '\0';
    }

    return count;
}

s64 pad_string(string *s, char chr, s64 count, u64 offset)
{
    return _pad_string_s(s, chr, count, offset);
}

s64 pad_string(wstring *s, wchar_t chr, s64 count, u64 offset)
{
    return _pad_string_s(s, chr, count, offset);
}

#define _to_string_c_s_body(F, String, SSize, Value, Offset, Options, ...)\
{\
    s64 written = F(String, SSize, Value, Offset, Options __VA_OPT__(,) __VA_ARGS__);\
\
    if (Options.pad_length < 0) written += pad_string(String, SSize, Options.pad_char, -Options.pad_length - written, written + Offset);\
\
    return written;\
}

// this basically just calls F and sets the size and null terminating
// character of String accordingly.
#define _to_string_s_body(F, String, Value, Offset, Options, ...)\
{\
    s64 written = F(String, Value, Offset, Options __VA_OPT__(,) __VA_ARGS__);\
\
    if (Options.pad_length < 0) written += pad_string(String, Options.pad_char, -Options.pad_length - written, written + Offset);\
\
    if (written + Offset >= String->size)\
    {\
        String->size = written + Offset;\
        String->data[String->size] = '\0';\
    }\
\
    return written;\
}

template<typename C>
s64 _bool_to_c_string(C *s, s64 ssize, bool value, u64 offset, format_options<C> opt, bool as_text)
{
    s64 written = 0;

    if (as_text)
    {
        const_string_base<C> lit_to_write{};

        if (value)
            lit_to_write = LIT(C, "true"_cs);
        else
            lit_to_write = LIT(C, "false"_cs);

        written += pad_string(s, ssize, opt.pad_char, opt.pad_length - lit_to_write.size, offset);
        written += _copy_string_checked(lit_to_write.c_str, lit_to_write.size, s, ssize, written + offset);
    }
    else
    {
        written += pad_string(s, ssize, opt.pad_char, opt.pad_length - 1, offset);

        if (written + offset < ssize)
        {
            s[written + offset] = value ? '1' : '0';
            written += 1;
        }
    }

    return written;
}

s64 to_string(char    *s, u64 ssize, bool x, u64 offset, format_options<char>    opt, bool as_text) _to_string_c_s_body(_bool_to_c_string, s, ssize, x, offset, opt, as_text)
s64 to_string(wchar_t *s, u64 ssize, bool x, u64 offset, format_options<wchar_t> opt, bool as_text) _to_string_c_s_body(_bool_to_c_string, s, ssize, x, offset, opt, as_text)

template<typename C>
s64 _bool_to_string(string_base<C> *s, bool value, u64 offset, format_options<C> opt, bool as_text)
{
    u64 sz = as_text ? 5 : 1;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    return _bool_to_c_string(s->data, s->reserved_size, value, offset, opt, as_text);
}

s64 to_string(string  *s, bool x, u64 offset, format_options<char>    opt, bool as_text) _to_string_s_body(_bool_to_string, s, x, offset, opt, as_text)
s64 to_string(wstring *s, bool x, u64 offset, format_options<wchar_t> opt, bool as_text) _to_string_s_body(_bool_to_string, s, x, offset, opt, as_text)

template<typename C>
s64 _char_to_c_string(C *s, u64 ssize, C value, u64 offset, format_options<C> opt)
{
    s64 written = 0;

    written += pad_string(s, ssize, opt.pad_char, opt.pad_length - 1, offset);

    if (offset + written < ssize)
    {
        s[offset + written] = value;
        written += 1;
    }

    return written;
}

s64 to_string(char    *s, u64 ssize, char    value, u64 offset, format_options<char>    opt) _to_string_c_s_body(_char_to_c_string, s, ssize, value, offset, opt)
s64 to_string(wchar_t *s, u64 ssize, wchar_t value, u64 offset, format_options<wchar_t> opt) _to_string_c_s_body(_char_to_c_string, s, ssize, value, offset, opt)

template<typename C>
s64 _char_to_string(string_base<C> *s, C value, u64 offset, format_options<C> opt)
{
    u64 sz = 1;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    return _char_to_c_string(s->data, s->reserved_size, value, offset, opt);
}

s64 to_string(string  *s, char    x, u64 offset, format_options<char>    opt) _to_string_s_body(_char_to_string, s, x, offset, opt)
s64 to_string(wstring *s, wchar_t x, u64 offset, format_options<wchar_t> opt) _to_string_s_body(_char_to_string, s, x, offset, opt)

template<typename C>
s64 _string_to_c_string(C *s, u64 ssize, const_string_base<C> x, u64 offset, format_options<C> opt)
{
    s64 written = 0;

    written += pad_string(s, ssize, opt.pad_char, opt.pad_length - x.size, offset);
    written += _copy_string_checked(x.c_str, x.size, s, ssize, written + offset);

    return written;
}

s64 _to_string(char    *s, u64 ssize, const_string   x, u64 offset, format_options<char>    opt) _to_string_c_s_body(_string_to_c_string, s, ssize, x, offset, opt)
s64 _to_string(wchar_t *s, u64 ssize, const_wstring  x, u64 offset, format_options<wchar_t> opt) _to_string_c_s_body(_string_to_c_string, s, ssize, x, offset, opt)

template<typename C>
s64 _string_to_string(string_base<C> *s, const_string_base<C> x, u64 offset, format_options<C> opt)
{
    u64 sz = x.size;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    return _string_to_c_string(s->data, s->reserved_size, x, offset, opt);
}

s64 _to_string(string  *s, const_string   x, u64 offset, format_options<char> opt)    _to_string_s_body(_string_to_string, s, x, offset, opt)
s64 _to_string(wstring *s, const_wstring  x, u64 offset, format_options<wchar_t> opt) _to_string_s_body(_string_to_string, s, x, offset, opt)

static inline u64 _convert_str(char *dst, const wchar_t *src, u64 n)
{
    return wcstombs(dst, src, n);
}

static inline u64 _convert_str(wchar_t *dst, const char *src, u64 n)
{
    return mbstowcs(dst, src, n);
}

// different chars
template<typename C1, typename C2>
s64 _encoding_to_c_string(C1    *s, u64 ssize, const_string_base<C2> x, u64 offset, format_options<C1>    opt)
{
    if (offset >= ssize)
        return 0;

    s64 written = 0;

    written += pad_string(s, ssize, opt.pad_char, opt.pad_length - x.size, offset);

    s64 chars_left = ssize - (offset + written);
    written += _convert_str(s + offset + written, x.c_str, chars_left);

    return written;
}

s64 _to_string(char    *s, u64 ssize, const_wstring x, u64 offset, format_options<char>    opt) _to_string_c_s_body(_encoding_to_c_string, s, ssize, x, offset, opt)
s64 _to_string(wchar_t *s, u64 ssize, const_string  x, u64 offset, format_options<wchar_t> opt) _to_string_c_s_body(_encoding_to_c_string, s, ssize, x, offset, opt)

template<typename C1, typename C2>
s64 _encoding_to_string(string_base<C1> *s, const_string_base<C2> x, u64 offset, format_options<C1> opt)
{
    u64 sz = x.size + 1;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    return _encoding_to_c_string(s->data, s->reserved_size, x, offset, opt);
}

s64 _to_string(string  *s, const_wstring x, u64 offset, format_options<char>    opt) _to_string_s_body(_encoding_to_string, s, x, offset, opt)
s64 _to_string(wstring *s, const_string  x, u64 offset, format_options<wchar_t> opt) _to_string_s_body(_encoding_to_string, s, x, offset, opt)

template<typename N>
int _get_number_of_int_digits_required(N value, int base)
{
    if (value == 0)
        return 1;

    int ret = 0;

    switch (base)
    {
    case 2:
    case 8:
    case 16:
    {
        u64 shift = const_log2(base);

        while (value > 0)
        {
            value >>= shift;
            ret += 1;
        }

        break;
    }

    case 10:
    {
        while (value > 0)
        {
            value /= 10;
            ret += 1;
        }

        break;
    }

    default:
        ret = -1;
        break;
    }

    return ret;
}

template<u8 Pow, typename C, typename N>
s64 _c_unsigned_pow2_to_string_reverse(C *s, N x, bool caps_letters = false)
{
    static_assert(Pow <= 16);

    const char *letters = caps_letters ? hex_letters_caps : hex_letters;

    int i = 0;

    if (x == 0)
    {
        s[i++] = '0';
        return i;
    }

    constexpr u64 Shift = const_log2(Pow);
    constexpr u64 mask = bitmask<u64>(0, Shift - 1);

    while (x > 0)
    {
        s[i++] = letters[x & mask];
        x >>= Shift;
    }

    return i;
}

template<typename C, typename N>
s64 _c_unsigned_decimal_to_string_reverse(C *s, N x)
{
    int i = 0;

    if (x == 0)
    {
        s[i++] = '0';
        return i;
    }

    while (x > 0)
    {
        s[i++] = '0' + (x % 10);
        x /= 10;
    }

    return i;
}

template<typename C, typename N>
s64 _integer_to_c_string(C *s, u64 ssize, N x, u64 offset, format_options<C> opt, integer_format_options iopt)
{
    if (opt.precision < 0)
        opt.precision = DEFAULT_INT_PRECISION;

    u64 buf_write_size = 0;
    u64 no_sign_x = x < 0 ? -x : x;
    int digit_count = _get_number_of_int_digits_required(no_sign_x, iopt.base);

    if (digit_count < 0)
        return -1;

    if (digit_count == 0)
        return 0;

    if (iopt.include_prefix)
    switch (iopt.base)
    {
    case 2:  buf_write_size += 2; break;
    case 8:  buf_write_size += 1; break;
    case 16: buf_write_size += 2; break;
    } 

    if (x < 0 || opt.sign == '+')
        buf_write_size += 1;

    buf_write_size += Max(opt.precision, digit_count);
    
    // buf is in reverse
    C buf[64];

    switch (iopt.base)
    {
    case 2:  _c_unsigned_pow2_to_string_reverse<2>(buf,  no_sign_x); break;
    case 8:  _c_unsigned_pow2_to_string_reverse<8>(buf,  no_sign_x); break;
    case 10: _c_unsigned_decimal_to_string_reverse(buf,  no_sign_x); break;
    case 16: _c_unsigned_pow2_to_string_reverse<16>(buf, no_sign_x, iopt.caps_letters); break;
    }

    u64 i = offset;

    if (opt.pad_char == ' ')
        i += pad_string(s, ssize, opt.pad_char, opt.pad_length - buf_write_size, i);

    if (i >= ssize) return i - offset;

    // sign, can be forced (+ or -), or negative numbers
    if (opt.sign == '+')
    {
        s[i++] = (x < 0) ? '-' : '+';
    }
    else if constexpr (is_signed(N))
    {
        if (x < 0)
            s[i++] = '-';
    }

    if (i >= ssize) return i - offset;

    // prefix, e.g. 0b, 0x
    if (iopt.include_prefix)
    {
        switch (iopt.base)
        {
        case 2: 
        {
            s[i++] = '0';
            if (i >= ssize) return i - offset;
            s[i++] = iopt.caps_prefix ? 'B' : 'b';
            break;
        }
        case 8:  s[i++] = '0'; break;
        case 16:
        {
            s[i++] = '0';
            if (i >= ssize) return i - offset;
            s[i++] = iopt.caps_prefix ? 'X' : 'x';
            break;
        }
        }
    }

    if (opt.pad_char != ' ')
        i += pad_string(s, ssize, opt.pad_char, opt.pad_length - buf_write_size, i);

    i += pad_string(s, ssize, (C)'0', opt.precision - digit_count, i);

    s64 copied_len = _copy_string_reverse_checked(buf, digit_count, s + i, ssize - i);
    i += copied_len;

    return i - offset;
}

template<typename C, typename N>
s64 _integer_to_c_string_conv(C *s, u64 ssize, N x, u64 offset, format_options<C> opt, integer_format_options iopt)
{
    if constexpr (is_signed(N))
    {
        s64 val = x;
        return _integer_to_c_string(s, ssize, val, offset, opt, iopt);
    }
    else
    {
        u64 val = x;
        return _integer_to_c_string(s, ssize, val, offset, opt, iopt);
    }
}

s64 to_string(char    *s, u64 ssize, u8  x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(char    *s, u64 ssize, u16 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(char    *s, u64 ssize, u32 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(char    *s, u64 ssize, u64 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, u8  x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, u16 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, u32 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, u64 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);

s64 to_string(char    *s, u64 ssize, s8  x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(char    *s, u64 ssize, s16 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(char    *s, u64 ssize, s32 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(char    *s, u64 ssize, s64 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, s8  x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, s16 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, s32 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);
s64 to_string(wchar_t *s, u64 ssize, s64 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_c_s_body(_integer_to_c_string_conv, s, ssize, x, offset, opt, iopt);

template<typename C, typename N>
s64 _integer_to_string(string_base<C> *s, N x, u64 offset, format_options<C> opt, integer_format_options iopt)
{
    if (opt.precision < 0)
        opt.precision = DEFAULT_INT_PRECISION;

    u64 bytes_to_reserve = 0;
    u64 no_sign_x = x < 0 ? -x : x;
    int digit_count = _get_number_of_int_digits_required(no_sign_x, iopt.base);

    if (digit_count < 0)
        return -1;

    if (digit_count == 0)
        return 0;

    if (iopt.include_prefix)
    switch (iopt.base)
    {
    case 2:  bytes_to_reserve += 2; break;
    case 8:  bytes_to_reserve += 1; break;
    case 16: bytes_to_reserve += 2; break;
    } 

    if (x < 0 || opt.sign == '+')
        bytes_to_reserve += 1;

    bytes_to_reserve += Max(opt.precision, digit_count);
    
    if (opt.pad_length > 0 && bytes_to_reserve < opt.pad_length)
        bytes_to_reserve = opt.pad_length;

    string_reserve(s, offset + bytes_to_reserve);

    return _integer_to_c_string(s->data, s->reserved_size, x, offset, opt, iopt);
}

// we do this to reduce the number of generated _integer_to_string functions
template<typename C, typename N>
s64 _integer_to_string_conv(string_base<C> *s, N x, u64 offset, format_options<C> opt, integer_format_options iopt)
{
    if constexpr (is_signed(N))
    {
        s64 val = x;
        return _integer_to_string(s, val, offset, opt, iopt);
    }
    else
    {
        u64 val = x;
        return _integer_to_string(s, val, offset, opt, iopt);
    }
}

s64 to_string(string  *s, u8  x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(string  *s, u16 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(string  *s, u32 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(string  *s, u64 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, u8  x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, u16 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, u32 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, u64 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);

s64 to_string(string  *s, s8  x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(string  *s, s16 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(string  *s, s32 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(string  *s, s64 x, u64 offset, format_options<char>    opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, s8  x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, s16 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, s32 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);
s64 to_string(wstring *s, s64 x, u64 offset, format_options<wchar_t> opt, integer_format_options iopt) _to_string_s_body(_integer_to_string_conv, s, x, offset, opt, iopt);

template<typename C>
s64 _pointer_to_c_string(C *s, u64 ssize, const void *x, u64 offset, format_options<C> opt)
{
    if (opt.precision < 0)
        opt.precision = 8;

    return _integer_to_c_string(s, ssize, reinterpret_cast<u64>(x), offset, opt, integer_format_options{
                .base = 16,
                .include_prefix = true,
                .caps_letters = false,
                .caps_prefix = false
            });
}

s64 to_string(char    *s, u64 ssize, const void *x, u64 offset, format_options<char>    opt) _to_string_c_s_body(_pointer_to_c_string, s, ssize, x, offset, opt);
s64 to_string(wchar_t *s, u64 ssize, const void *x, u64 offset, format_options<wchar_t> opt) _to_string_c_s_body(_pointer_to_c_string, s, ssize, x, offset, opt);

template<typename C>
s64 _pointer_to_string(string_base<C> *s, const void *x, u64 offset, format_options<C> opt)
{
    if (opt.precision < 0)
        opt.precision = 8;

    return _integer_to_string(s, reinterpret_cast<u64>(x), offset, opt, integer_format_options{
                .base = 16,
                .include_prefix = true,
                .caps_letters = false,
                .caps_prefix = false
            });
}

s64 to_string(string  *s, const void *x, u64 offset, format_options<char>    opt) _to_string_s_body(_pointer_to_string, s, x, offset, opt);
s64 to_string(wstring *s, const void *x, u64 offset, format_options<wchar_t> opt) _to_string_s_body(_pointer_to_string, s, x, offset, opt);

template<typename C, typename N>
s64 _c_remainder_to_string_reverse(C *s, N x, int precision, bool ignore_trailing_zeroes)
{
    if (precision > FLOAT_MAX_PRECISION)
        precision = FLOAT_MAX_PRECISION;

    if (precision <= 0)
        return 0;

    s64 i = 0;
    u64 cap = 1;

	for (u64 _pow10 = 0; _pow10 < precision; _pow10++)
		cap *= 10;

	double round = x * static_cast<double>(cap);
	u64 whole = static_cast<u64>(round);

	if ((static_cast<u64>((round - static_cast<double>(whole)) * 10.0)) >= 5)
    {
		whole++;

		if(whole >= cap)
			whole = cap - 1;
	}

	i += _c_unsigned_decimal_to_string_reverse(s, whole);

    while (i < precision)
        s[i++] = '0';

    s[i++] = '.';

    return i;
}

#define FLOAT_BUFFER_SIZE 64

template<typename C, typename N>
s64 _float_to_c_string(C *s, u64 ssize, N x, u64 offset, format_options<C> opt, float_format_options fopt)
{
    if (opt.precision < 0)
        opt.precision = DEFAULT_FLOAT_PRECISION;

    C buf[FLOAT_BUFFER_SIZE];
    s64 buf_size = -1;

    double no_sign_x = x < 0 ? -x : x;

    u64 whole = static_cast<u64>(no_sign_x);
    double remainder = no_sign_x - static_cast<double>(whole);

    s64 remainder_size = _c_remainder_to_string_reverse(buf, remainder, opt.precision, fopt.ignore_trailing_zeroes);
    s64 whole_size = _c_unsigned_decimal_to_string_reverse(buf + remainder_size, whole);

    buf_size = remainder_size + whole_size;

    if (buf_size < 0)
        return -1;

    if (buf_size == 0)
        return 0;

    u64 buf_write_size = 0;

    if (opt.sign == '+' || x < 0)
        buf_write_size += 1;

    buf_write_size += whole_size;
    buf_write_size += remainder_size;

    u64 copy_start = 0;

    if (fopt.ignore_trailing_zeroes)
        while (copy_start < FLOAT_BUFFER_SIZE)
        {
            C c = buf[copy_start];

            if (c == '0')
            {
                copy_start++;
                continue;
            }

            if (c == '.')
            {
                copy_start++;
                break;
            }

            break;
        }

    buf_write_size -= copy_start;

    u64 i = offset;

    i += pad_string(s, ssize, opt.pad_char, opt.pad_length - buf_write_size, offset);

    if (i >= ssize) return i - offset;

    if (opt.sign == '+') s[i++] = (x < 0.0) ? '-' : '+';
    else if (x < 0.0)    s[i++] = '-';

    if (i >= ssize) return i - offset;

    i += _copy_string_reverse_checked(buf + copy_start, buf_size - copy_start, s + i, ssize - i);

    return i - offset;
}

s64 to_string(char    *s, u64 ssize, float  x, u64 offset, format_options<char>    opt, float_format_options fopt) _to_string_c_s_body(_float_to_c_string, s, ssize, x, offset, opt, fopt)
s64 to_string(char    *s, u64 ssize, double x, u64 offset, format_options<char>    opt, float_format_options fopt) _to_string_c_s_body(_float_to_c_string, s, ssize, x, offset, opt, fopt)
s64 to_string(wchar_t *s, u64 ssize, float  x, u64 offset, format_options<wchar_t> opt, float_format_options fopt) _to_string_c_s_body(_float_to_c_string, s, ssize, x, offset, opt, fopt)
s64 to_string(wchar_t *s, u64 ssize, double x, u64 offset, format_options<wchar_t> opt, float_format_options fopt) _to_string_c_s_body(_float_to_c_string, s, ssize, x, offset, opt, fopt)

template<typename C, typename N>
s64 _float_to_string(string_base<C> *s, N x, u64 offset, format_options<C> opt, float_format_options fopt)
{
    if (opt.precision < 0)
        opt.precision = DEFAULT_FLOAT_PRECISION;

    C buf[FLOAT_BUFFER_SIZE];
    s64 buf_size = -1;

    double no_sign_x = x < 0 ? -x : x;

    u64 whole = static_cast<u64>(no_sign_x);
    double remainder = no_sign_x - static_cast<double>(whole);

    s64 remainder_size = _c_remainder_to_string_reverse(buf, remainder, opt.precision, fopt.ignore_trailing_zeroes);
    s64 whole_size = _c_unsigned_decimal_to_string_reverse(buf + remainder_size, whole);

    buf_size = remainder_size + whole_size;

    if (buf_size < 0)
        return -1;

    if (buf_size == 0)
        return 0;

    u64 bytes_to_reserve = 0;

    if (opt.sign == '+' || x < 0)
        bytes_to_reserve += 1;

    bytes_to_reserve += whole_size;
    bytes_to_reserve += remainder_size;

    u64 buf_write_size = bytes_to_reserve;

    if (opt.pad_length > 0 && bytes_to_reserve < opt.pad_length)
        bytes_to_reserve = opt.pad_length;

    string_reserve(s, offset + bytes_to_reserve);

    u64 copy_start = 0;

    if (fopt.ignore_trailing_zeroes)
        while (copy_start < FLOAT_BUFFER_SIZE)
        {
            C c = buf[copy_start];

            if (c == '0')
            {
                copy_start++;
                continue;
            }

            if (c == '.')
            {
                copy_start++;
                break;
            }

            break;
        }

    buf_write_size -= copy_start;

    u64 i = offset + pad_string(s, opt.pad_char, opt.pad_length - buf_write_size, offset);

    if (opt.sign == '+')
        s->data[i++] = (x < 0.0) ? '-' : '+';
    else if (x < 0.0)
        s->data[i++] = '-';

    s64 copied_len = _copy_string_reverse_checked(buf + copy_start, buf_size - copy_start, s->data + i, s->reserved_size - i);
    i += copied_len;

    return i - offset;
}

s64 to_string(string  *s, float  x, u64 offset, format_options<char>    opt, float_format_options fopt) _to_string_s_body(_float_to_string, s, x, offset, opt, fopt)
s64 to_string(string  *s, double x, u64 offset, format_options<char>    opt, float_format_options fopt) _to_string_s_body(_float_to_string, s, x, offset, opt, fopt)
s64 to_string(wstring *s, float  x, u64 offset, format_options<wchar_t> opt, float_format_options fopt) _to_string_s_body(_float_to_string, s, x, offset, opt, fopt)
s64 to_string(wstring *s, double x, u64 offset, format_options<wchar_t> opt, float_format_options fopt) _to_string_s_body(_float_to_string, s, x, offset, opt, fopt)

// format
template<typename C>
s64 _format_c_s(u64 i, s64 written, C *s, u64 ssize, u64 offset, const_string_base<C> fmt)
{
    C c;

    while (i < fmt.size && offset < ssize)
    {
        c = fmt[i];

        if (c != '%')
        {
            if (c == '\\')
            {
                i++;

                if (i >= fmt.size)
                {
                    s[offset++] = '\\';
                    written++;
                    break;
                }
                else if (fmt[i] != '%')
                {
                    s[offset++] = '\\';
                    written++;
                }
            }

            s[offset++] = fmt[i];
        }
        else
        {
            // %
            // ERROR
            return -1;
        }

        written++;
        i++;
    }

    return written;
}

template<typename C>
s64 _format_s(u64 i, s64 written, string_base<C> *s, u64 offset, const_string_base<C> fmt)
{
    C c;

    while (i < fmt.size)
    {
        c = fmt[i];

        if (c != '%')
        {
            string_reserve(s, offset + (offset % FORMAT_BUFFER_INCREMENT));

            if (c == '\\')
            {
                i++;

                if (i >= fmt.size)
                {
                    s->data[offset++] = '\\';
                    written++;
                    break;
                }
                else if (fmt[i] != '%')
                {
                    s->data[offset++] = '\\';
                    written++;
                }
            }

            s->data[offset++] = fmt[i];
        }
        else
        {
            // %
            // ERROR
            return -1;
        }

        written++;
        i++;
    }

    return written;
}

s64 internal::_format(u64 i, s64 written, char *s, u64 ssize, u64 offset, const_string fmt)
{
    return ::_format_c_s(i, written, s, ssize, offset, fmt);
}

s64 internal::_format(u64 i, s64 written, wchar_t *s, u64 ssize, u64 offset, const_wstring fmt)
{
    return ::_format_c_s(i, written, s, ssize, offset, fmt);
}

s64 internal::_format(u64 i, s64 written, string *s, u64 offset, const_string fmt)
{
    return ::_format_s(i, written, s, offset, fmt);
}

s64 internal::_format(u64 i, s64 written, wstring *s, u64 offset, const_wstring fmt)
{
    return ::_format_s(i, written, s, offset, fmt);
}

template<typename C>
s64 _format_skip_until_placeholder_c_s(u64 *_i, internal::_placeholder_info<C> *pl, C *s, u64 ssize, u64 offset, const_string_base<C> fmt)
{
    C c;
    u64 i = *_i;
    s64 written = 0;

    while (i < fmt.size && offset < ssize)
    {
        c = fmt[i];

        if (c == '\\')
        {
            i++;

            if (i >= fmt.size)
            {
                s[offset++] = '\\';
                written++;
                break;
            }
            else if (fmt[i] != '%')
            {
                s[offset++] = '\\';
                written++;
                c = fmt[i];
            }
            else
                c = fmt[i];
        }
        else if (c == '%')
            break;

        s[offset++] = c;
        i++;
        written++;
    }

    if (i >= fmt.size || offset >= ssize)
    {
        *_i = i;
        return written;
    }

    // %
    s64 j = i + 1;
    pl->has_placeholder = true;

#define if_at_end_goto(Var, Label)\
    if (Var >= fmt.size)\
    {\
        i = Var - 1;\
        goto Label;\
    }

    if_at_end_goto(j, fmt_end);

    c = fmt[j];

    if (c == '#')
    {
        pl->alternative = true;
        j++;

        if_at_end_goto(j, fmt_end);
        c = fmt[j];
    }

    switch (c)
    {
    case '-': pl->options.sign = '-'; j++; break;
    case '+': pl->options.sign = '+'; j++; break;
    }

    if_at_end_goto(j, fmt_end);

    c = fmt[j];

    if (c == '0')
        pl->options.pad_char = '0';

    while (c >= '0' && c <= '9')
    {
        pl->options.pad_length = (pl->options.pad_length * 10) + (c - '0');
        j++;

        if (j >= fmt.size) // break here to apply sign
            break;

        c = fmt[j];
    }

    if (pl->options.sign == '-')
        pl->options.pad_length = -pl->options.pad_length;

    if_at_end_goto(j, fmt_end);

    if (c == '.')
    {
        j++;

        if_at_end_goto(j, fmt_end);

        c = fmt[j];

        if (c >= '0' && c <= '9')
            pl->options.precision = 0;
        
        while (c >= '0' && c <= '9')
        {
            pl->options.precision += (pl->options.precision * 10) + (c - '0');
            j++;

            if_at_end_goto(j, fmt_end);
            c = fmt[j];
        }
    }

    i = j - 1;

#undef if_at_end_goto
    
fmt_end:

    *_i = i + 1;
    return written;
}

template<typename C>
s64 _format_skip_until_placeholder_s(u64 *_i, internal::_placeholder_info<C> *pl, string_base<C> *s, u64 offset, const_string_base<C> fmt)
{
    C c;
    u64 i = *_i;
    s64 written = 0;

    while (i < fmt.size)
    {
        string_reserve(s, offset + (offset % FORMAT_BUFFER_INCREMENT));
        c = fmt[i];

        if (c == '\\')
        {
            i++;

            if (i >= fmt.size)
            {
                s->data[offset++] = '\\';
                written++;
                break;
            }
            else if (fmt[i] != '%')
            {
                s->data[offset++] = '\\';
                written++;
                c = fmt[i];
            }
            else
                c = fmt[i];
        }
        else if (c == '%')
            break;

        s->data[offset++] = c;
        i++;
        written++;
    }

    *_i = i;
    return written + _format_skip_until_placeholder_c_s(_i, pl, s->data, s->reserved_size, offset, fmt);
}

s64 internal::_format_skip_until_placeholder(u64 *i, _placeholder_info<char>    *pl, char  *s, u64 ssize, u64 offset, const_string  fmt)
{
    return ::_format_skip_until_placeholder_c_s(i, pl, s, ssize, offset, fmt);
}

s64 internal::_format_skip_until_placeholder(u64 *i, _placeholder_info<wchar_t> *pl, wchar_t *s, u64 ssize, u64 offset, const_wstring fmt)
{
    return ::_format_skip_until_placeholder_c_s(i, pl, s, ssize, offset, fmt);
}

s64 internal::_format_skip_until_placeholder(u64 *i, _placeholder_info<char>    *pl, string  *s, u64 offset, const_string  fmt)
{
    return ::_format_skip_until_placeholder_s(i, pl, s, offset, fmt);
}

s64 internal::_format_skip_until_placeholder(u64 *i, _placeholder_info<wchar_t> *pl, wstring *s, u64 offset, const_wstring fmt)
{
    return ::_format_skip_until_placeholder_s(i, pl, s, offset, fmt);
}

void _format_buffer_cleanup();

#define TFORMAT_RING_BUFFER_MIN_SIZE 4096

template<typename C>
internal::tformat_buffer *_get_static_format_buffer(bool free_buffer = false)
{
    static internal::tformat_buffer _buf{};

    if (free_buffer && _buf.buffer.data != nullptr)
    {
        free(&_buf.buffer);
        _buf.offset = 0;
    }
    else if (!free_buffer && _buf.buffer.data == nullptr)
    {
        if (!init(&_buf.buffer, TFORMAT_RING_BUFFER_MIN_SIZE, 2))
            return nullptr;

        ::register_exit_function(_format_buffer_cleanup);
    }

    return &_buf;
}

void _format_buffer_cleanup()
{
    _get_static_format_buffer<char>(true);
    _get_static_format_buffer<wchar_t>(true);
}

internal::tformat_buffer *internal::_get_tformat_buffer_char()
{
    return _get_static_format_buffer<char>(false);
}

internal::tformat_buffer *internal::_get_tformat_buffer_wchar()
{
    return _get_static_format_buffer<wchar_t>(false);
}

u64 get_tformat_buffer_size()
{
    // doesn't matter which one
    internal::tformat_buffer *buf = internal::_get_tformat_buffer_char();

    if (buf == nullptr)
        return 0;

    return buf->buffer.size;
}
