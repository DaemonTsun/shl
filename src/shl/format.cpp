
#include "shl/bits.hpp"
#include "shl/format.hpp"

#define LIT(C, Literal)\
    inline_const_if(is_same(C, char), Literal##_cs, L##Literal##_cs)

static const char *hex_letters      = "0123456789abcdef";
static const char *hex_letters_caps = "0123456789ABCDEF";

// TODO: maybe move this to string.hpp?
template<typename C>
bool string_reserve(string_base<C> *s, u64 total_size)
{
    // +1 for \0
    if (s->data.reserved_size < total_size + 1)
    {
        reserve(&s->data, total_size + 1);
        return true;
    }

    return false;
}

template<typename C>
s64 copy_string_reverse(const C* src, C *dst, u64 size)
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
s64 pad_string(string_base<C> *s, C chr, s64 count, u64 offset = 0)
{
    string_reserve(s, count + offset);

    for (s64 i = 0; i < count; ++i)
        s->data.data[i + offset] = chr;

    return count;
}

template<typename C>
s64 _bool_to_string(string_base<C> *s, bool x, u64 offset, bool as_text)
{
    u64 sz = as_text ? 5 : 1;
    sz += offset;

    string_reserve(s, sz);

    if (as_text)
    {
        if (x)
        {
            copy_string(LIT(C, "true"), s, 4, offset);

            return 4;
        }
        else
        {
            copy_string(LIT(C, "false"), s, 5, offset);

            return 5;
        }
    }
    else
    {
        s->data.data[offset] = x ? '1' : '0';
        return 1;
    }
}

// this basically just calls F and sets the size and null terminating
// character of String accordingly.
#define to_string_body(F, String, Value, Offset, ...)\
{\
    u64 sz = String->data.size;\
    u64 n = F(String, Value, Offset __VA_OPT__(,) __VA_ARGS__);\
\
    if (n + Offset >= sz)\
    {\
        String->data.size = n + Offset;\
        String->data.data[String->data.size] = '\0';\
    }\
\
    return n;\
}

s64 to_string(string  *s, bool x)                           to_string_body(_bool_to_string, s, x, 0, false)
s64 to_string(string  *s, bool x, u64 offset)               to_string_body(_bool_to_string, s, x, offset, false)
s64 to_string(string  *s, bool x, u64 offset, bool as_text) to_string_body(_bool_to_string, s, x, offset, as_text)
s64 to_string(wstring *s, bool x)                           to_string_body(_bool_to_string, s, x, 0, false)
s64 to_string(wstring *s, bool x, u64 offset)               to_string_body(_bool_to_string, s, x, offset, false)
s64 to_string(wstring *s, bool x, u64 offset, bool as_text) to_string_body(_bool_to_string, s, x, offset, as_text)

template<typename C>
s64 _char_to_string(string_base<C> *s, C x, u64 offset)
{
    u64 sz = 1;
    sz += offset;

    string_reserve(s, sz);
    s->data.data[offset] = x;

    return 1;
}

s64 to_string(string  *s, char    x)             to_string_body(_char_to_string, s, x, 0)
s64 to_string(string  *s, char    x, u64 offset) to_string_body(_char_to_string, s, x, offset)
s64 to_string(wstring *s, wchar_t x)             to_string_body(_char_to_string, s, x, 0)
s64 to_string(wstring *s, wchar_t x, u64 offset) to_string_body(_char_to_string, s, x, offset)

template<typename C>
s64 _string_to_string(string_base<C> *s, const_string_base<C> x, u64 offset)
{
    u64 sz = x.size;
    sz += offset;

    string_reserve(s, sz);
    copy_string(x, s, x.size, offset);

    return x.size;
}

s64 to_string(string  *s, const char    *x)             to_string_body(_string_to_string, s, to_const_string(x), 0)
s64 to_string(string  *s, const char    *x, u64 offset) to_string_body(_string_to_string, s, to_const_string(x), offset)
s64 to_string(string  *s, const_string   x)             to_string_body(_string_to_string, s, x, 0)
s64 to_string(string  *s, const_string   x, u64 offset) to_string_body(_string_to_string, s, x, offset)
s64 to_string(string  *s, const string  *x)             to_string_body(_string_to_string, s, to_const_string(x), 0)
s64 to_string(string  *s, const string  *x, u64 offset) to_string_body(_string_to_string, s, to_const_string(x), offset)
s64 to_string(wstring *s, const wchar_t *x)             to_string_body(_string_to_string, s, to_const_string(x), 0)
s64 to_string(wstring *s, const wchar_t *x, u64 offset) to_string_body(_string_to_string, s, to_const_string(x), offset)
s64 to_string(wstring *s, const_wstring  x)             to_string_body(_string_to_string, s, x, 0)
s64 to_string(wstring *s, const_wstring  x, u64 offset) to_string_body(_string_to_string, s, x, offset)
s64 to_string(wstring *s, const wstring *x)             to_string_body(_string_to_string, s, to_const_string(x), 0)
s64 to_string(wstring *s, const wstring *x, u64 offset) to_string_body(_string_to_string, s, to_const_string(x), offset)

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

    constexpr u64 Shift = log2(Pow);
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
s64 _integer_to_string(string_base<C> *s, N x, u64 offset, integer_format_options<C> options)
{
    C buf[64];
    s64 buf_size = -1;

    u64 no_sign_x = x < 0 ? -x : x;

    switch (options.base)
    {
    case 2:  buf_size = _c_unsigned_pow2_to_string_reverse<2>(buf,  no_sign_x); break;
    case 8:  buf_size = _c_unsigned_pow2_to_string_reverse<8>(buf,  no_sign_x); break;
    case 10: buf_size = _c_unsigned_decimal_to_string_reverse(buf,  no_sign_x); break;
    case 16: buf_size = _c_unsigned_pow2_to_string_reverse<16>(buf, no_sign_x, options.caps_letters); break;
    }

    if (buf_size < 0)
        return -1;

    if (buf_size == 0)
        return 0;

    u64 bytes_to_reserve = 0;

    if (options.include_prefix)
        bytes_to_reserve += 2;

    if (is_signed(N) || options.force_sign)
        bytes_to_reserve += 1;

    if (options.pad_length <= buf_size)
        bytes_to_reserve += buf_size;
    else
        bytes_to_reserve += options.pad_length;

    string_reserve(s, offset + bytes_to_reserve);

    u64 i = offset;

    // sign, can be forced (+ or -), or negative numbers
    if (options.force_sign)
    {
        s->data[i++] = (x < 0) ? '-' : '+';
    }
    else if constexpr (is_signed(N))
    {
        if (x < 0)
            s->data[i++] = '-';
    }

    // prefix, e.g. 0b, 0x
    if (options.include_prefix)
    {
        switch (options.base)
        {
        case 2:  s->data[i++] = '0'; s->data[i++] = options.caps_prefix ? 'B' : 'b'; break;
        case 8:  s->data[i++] = '0'; break;
        case 16: s->data[i++] = '0'; s->data[i++] = options.caps_prefix ? 'X' : 'x'; break;
        }
    }

    if (options.pad_length > buf_size)
        i += pad_string(s, options.pad_char, options.pad_length - buf_size, i);

    copy_string_reverse(buf, s->data.data + i, buf_size);

    i += buf_size;

    return i - offset;
}

s64 to_string(string  *s, u8 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, u8 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, u8 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);
s64 to_string(string  *s, u16 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, u16 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, u16 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);
s64 to_string(string  *s, u32 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, u32 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, u32 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);
s64 to_string(string  *s, u64 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, u64 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, u64 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);

s64 to_string(string  *s, s8 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, s8 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, s8 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);
s64 to_string(string  *s, s16 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, s16 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, s16 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);
s64 to_string(string  *s, s32 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, s32 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, s32 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);
s64 to_string(string  *s, s64 x)             to_string_body(_integer_to_string, s, x, 0, default_integer_options<char>);
s64 to_string(string  *s, s64 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_integer_options<char>);
s64 to_string(string  *s, s64 x, u64 offset, integer_format_options<char> options) to_string_body(_integer_to_string, s, x, offset, options);

template<typename C>
s64 _pointer_to_string(string_base<C> *s, const void *x, u64 offset)
{
    return _integer_to_string(s, reinterpret_cast<u64>(x), offset, integer_format_options<C>{
                .base = 16,
                .include_prefix = true,
                .pad_length = 8,
                .pad_char = '0',
                .force_sign = false,
                .caps_letters = false,
                .caps_prefix = false
            });
}

s64 to_string(string  *s, const void *x)             to_string_body(_pointer_to_string, s, x, 0);
s64 to_string(string  *s, const void *x, u64 offset) to_string_body(_pointer_to_string, s, x, offset);

template<typename C, typename N>
s64 _c_remainder_to_string_reverse(C *s, N x, int precision, bool ignore_trailing_zeroes)
{
    if (precision > FLOAT_MAX_PRECISION)
        precision = FLOAT_MAX_PRECISION;

    if (precision <= 0)
        return 0;

    s64 i = 0;
    /*

    u64 mult = 10; 

    while (i <= precision)
    {
        s[i++] = '0' + (static_cast<u64>((x * mult)) % 10);
        mult *= 10;
    }
    */

    u64 cap = 1;

	for (u64 i = 0; i < precision; i++)
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
s64 _float_to_string(string_base<C> *s, N x, u64 offset, float_format_options<C> options)
{
    C buf[FLOAT_BUFFER_SIZE];
    s64 buf_size = -1;

    double no_sign_x = x < 0 ? -x : x;

    u64 whole = static_cast<u64>(no_sign_x);
    double remainder = no_sign_x - static_cast<double>(whole);

    buf_size = _c_remainder_to_string_reverse(buf, remainder, options.precision, options.ignore_trailing_zeroes);
    buf_size += _c_unsigned_decimal_to_string_reverse(buf + buf_size, whole);

    if (buf_size < 0)
        return -1;

    if (buf_size == 0)
        return 0;

    u64 bytes_to_reserve = 0;

    bytes_to_reserve += 1; // sign

    if (options.pad_length <= buf_size)
        bytes_to_reserve += buf_size;
    else
        bytes_to_reserve += options.pad_length;

    string_reserve(s, offset + bytes_to_reserve);

    u64 i = offset;

    if (options.force_sign)
        s->data[i++] = (x < 0.0) ? '-' : '+';
    else if (x < 0.0)
        s->data[i++] = '-';

    if (options.pad_length > buf_size)
        i += pad_string(s, options.pad_char, options.pad_length - buf_size, i);

    u64 copy_start = 0;

    if (options.ignore_trailing_zeroes)
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

    copy_string_reverse(buf + copy_start, s->data.data + i, buf_size - copy_start);

    i += (buf_size - copy_start);

    return i - offset;
}

s64 to_string(string  *s, float x)             to_string_body(_float_to_string, s, x, 0, default_float_options<char>)
s64 to_string(string  *s, float x, u64 offset) to_string_body(_float_to_string, s, x, offset, default_float_options<char>)
s64 to_string(string  *s, float x, u64 offset, float_format_options<char> options) to_string_body(_float_to_string, s, x, offset, options)

s64 to_string(string  *s, double x)             to_string_body(_float_to_string, s, x, 0, default_float_options<char>)
s64 to_string(string  *s, double x, u64 offset) to_string_body(_float_to_string, s, x, offset, default_float_options<char>)
s64 to_string(string  *s, double x, u64 offset, float_format_options<char> options) to_string_body(_float_to_string, s, x, offset, options)
