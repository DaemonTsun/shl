
#include "shl/bits.hpp"
#include "shl/format.hpp"

#define LIT(C, Literal)\
    inline_const_if(is_same(C, char), Literal##_cs, L##Literal##_cs)

static const char *hex_letters      = "0123456789abcdef";
static const char *hex_letters_caps = "0123456789ABCDEF";

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
    if (count <= 0)
        return 0;

    string_reserve(s, count + offset);

    for (s64 i = 0; i < count; ++i)
        s->data.data[i + offset] = chr;

    return count;
}

template<typename C>
s64 _bool_to_string(string_base<C> *s, bool x, u64 offset, format_options<C> opt, bool as_text)
{
    s64 written = 0;
    u64 sz = as_text ? 5 : 1;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    if (as_text)
    {
        if (x)
        {
            written += pad_string(s, opt.pad_char, opt.pad_length - 4, offset);
            copy_string(LIT(C, "true"), s, 4, written + offset);

            written += 4;
        }
        else
        {
            written += pad_string(s, opt.pad_char, opt.pad_length - 5, offset);
            copy_string(LIT(C, "false"), s, 5, written + offset);

            written += 5;
        }
    }
    else
    {
        written += pad_string(s, opt.pad_char, opt.pad_length - 1, offset);
        s->data.data[written + offset] = x ? '1' : '0';
        written += 1;
    }

    return written;
}

// this basically just calls F and sets the size and null terminating
// character of String accordingly.
#define to_string_body(F, String, Value, Offset, Options, ...)\
{\
    s64 written = F(String, Value, Offset, Options __VA_OPT__(,) __VA_ARGS__);\
\
    if (Options.pad_length < 0) written += pad_string(String, Options.pad_char, -Options.pad_length - written, written + Offset);\
\
    if (written + Offset >= String->data.size)\
    {\
        String->data.size = written + Offset;\
        String->data.data[String->data.size] = '\0';\
    }\
\
    return written;\
}

s64 to_string(string  *s, bool x)                           to_string_body(_bool_to_string, s, x, 0, default_format_options<char>, false)
s64 to_string(string  *s, bool x, u64 offset)               to_string_body(_bool_to_string, s, x, offset, default_format_options<char>, false)
s64 to_string(string  *s, bool x, u64 offset, format_options<char> opt, bool as_text) to_string_body(_bool_to_string, s, x, offset, opt, as_text)
s64 to_string(wstring *s, bool x)                           to_string_body(_bool_to_string, s, x, 0, default_format_options<wchar_t>, false)
s64 to_string(wstring *s, bool x, u64 offset)               to_string_body(_bool_to_string, s, x, offset, default_format_options<wchar_t>, false)
s64 to_string(wstring *s, bool x, u64 offset, format_options<wchar_t> opt, bool as_text) to_string_body(_bool_to_string, s, x, offset, opt, as_text)

template<typename C>
s64 _char_to_string(string_base<C> *s, C x, u64 offset, format_options<C> opt)
{
    s64 written = 0;
    u64 sz = 1;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    written += pad_string(s, opt.pad_char, opt.pad_length - 1, offset);
    s->data.data[offset + written] = x;
    written += 1;

    return written;
}

s64 to_string(string  *s, char    x)             to_string_body(_char_to_string, s, x, 0, default_format_options<char>)
s64 to_string(string  *s, char    x, u64 offset) to_string_body(_char_to_string, s, x, offset, default_format_options<char>)
s64 to_string(string  *s, char    x, u64 offset, format_options<char> opt) to_string_body(_char_to_string, s, x, offset, opt)
s64 to_string(wstring *s, wchar_t x)             to_string_body(_char_to_string, s, x, 0, default_format_options<wchar_t>)
s64 to_string(wstring *s, wchar_t x, u64 offset) to_string_body(_char_to_string, s, x, offset, default_format_options<wchar_t>)
s64 to_string(wstring *s, wchar_t x, u64 offset, format_options<wchar_t> opt) to_string_body(_char_to_string, s, x, offset, opt)

template<typename C>
s64 _string_to_string(string_base<C> *s, const_string_base<C> x, u64 offset, format_options<C> opt)
{
    s64 written = 0;
    u64 sz = x.size;

    if (opt.pad_length > 0 && sz < opt.pad_length)
        sz = opt.pad_length;

    sz += offset;

    string_reserve(s, sz);

    written += pad_string(s, opt.pad_char, opt.pad_length - x.size, offset);
    copy_string(x, s, x.size, offset + written);
    written += x.size;

    return written;
}

s64 to_string(string  *s, const char    *x)             to_string_body(_string_to_string, s, to_const_string(x), 0, default_format_options<char>)
s64 to_string(string  *s, const char    *x, u64 offset) to_string_body(_string_to_string, s, to_const_string(x), offset, default_format_options<char>)
s64 to_string(string  *s, const char    *x, u64 offset, format_options<char> opt) to_string_body(_string_to_string, s, to_const_string(x), offset, opt)
s64 to_string(string  *s, const_string   x)             to_string_body(_string_to_string, s, x, 0, default_format_options<char>)
s64 to_string(string  *s, const_string   x, u64 offset) to_string_body(_string_to_string, s, x, offset, default_format_options<char>)
s64 to_string(string  *s, const_string   x, u64 offset, format_options<char> opt) to_string_body(_string_to_string, s, x, offset, opt)
s64 to_string(string  *s, const string  *x)             to_string_body(_string_to_string, s, to_const_string(x), 0, default_format_options<char>)
s64 to_string(string  *s, const string  *x, u64 offset) to_string_body(_string_to_string, s, to_const_string(x), offset, default_format_options<char>)
s64 to_string(string  *s, const string  *x, u64 offset, format_options<char> opt) to_string_body(_string_to_string, s, to_const_string(x), offset, opt)

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
s64 _integer_to_string(string_base<C> *s, N x, u64 offset, format_options<C> opt, integer_format_options iopt)
{
    if (opt.precision < 0)
        opt.precision = DEFAULT_INT_PRECISION;

    C buf[64];
    s64 buf_size = -1;

    u64 no_sign_x = x < 0 ? -x : x;

    switch (iopt.base)
    {
    case 2:  buf_size = _c_unsigned_pow2_to_string_reverse<2>(buf,  no_sign_x); break;
    case 8:  buf_size = _c_unsigned_pow2_to_string_reverse<8>(buf,  no_sign_x); break;
    case 10: buf_size = _c_unsigned_decimal_to_string_reverse(buf,  no_sign_x); break;
    case 16: buf_size = _c_unsigned_pow2_to_string_reverse<16>(buf, no_sign_x, iopt.caps_letters); break;
    }

    if (buf_size < 0)
        return -1;

    if (buf_size == 0)
        return 0;

    u64 bytes_to_reserve = 0;

    if (iopt.include_prefix)
    switch (iopt.base)
    {
    case 2:  bytes_to_reserve += 2; break;
    case 8:  bytes_to_reserve += 1; break;
    case 16: bytes_to_reserve += 2; break;
    }

    if (x < 0 || opt.sign == '+')
        bytes_to_reserve += 1;

    if (opt.precision <= buf_size)
        bytes_to_reserve += buf_size;
    else
        bytes_to_reserve += opt.precision;

    u64 buf_write_size = bytes_to_reserve;

    if (opt.pad_length > 0 && bytes_to_reserve < opt.pad_length)
        bytes_to_reserve = opt.pad_length;

    string_reserve(s, offset + bytes_to_reserve);

    u64 i = offset + pad_string(s, opt.pad_char, opt.pad_length - buf_write_size, offset);

    // sign, can be forced (+ or -), or negative numbers
    if (opt.sign == '+')
    {
        s->data[i++] = (x < 0) ? '-' : '+';
    }
    else if constexpr (is_signed(N))
    {
        if (x < 0)
            s->data[i++] = '-';
    }

    // prefix, e.g. 0b, 0x
    if (iopt.include_prefix)
    {
        switch (iopt.base)
        {
        case 2:  s->data[i++] = '0'; s->data[i++] = iopt.caps_prefix ? 'B' : 'b'; break;
        case 8:  s->data[i++] = '0'; break;
        case 16: s->data[i++] = '0'; s->data[i++] = iopt.caps_prefix ? 'X' : 'x'; break;
        }
    }

    i += pad_string(s, '0', opt.precision - buf_size, i);

    copy_string_reverse(buf, s->data.data + i, buf_size);

    i += buf_size;

    return i - offset;
}

s64 to_string(string  *s, u8 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u8 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u8 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);
s64 to_string(string  *s, u16 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u16 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u16 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);
s64 to_string(string  *s, u32 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u32 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u32 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);
s64 to_string(string  *s, u64 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u64 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, u64 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);

s64 to_string(string  *s, s8 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s8 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s8 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);
s64 to_string(string  *s, s16 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s16 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s16 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);
s64 to_string(string  *s, s32 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s32 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s32 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);
s64 to_string(string  *s, s64 x)             to_string_body(_integer_to_string, s, x, 0, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s64 x, u64 offset) to_string_body(_integer_to_string, s, x, offset, default_format_options<char>, default_integer_options);
s64 to_string(string  *s, s64 x, u64 offset, format_options<char> opt, integer_format_options iopt) to_string_body(_integer_to_string, s, x, offset, opt, iopt);

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

s64 to_string(string  *s, const void *x)             to_string_body(_pointer_to_string, s, x, 0, default_format_options<char>);
s64 to_string(string  *s, const void *x, u64 offset) to_string_body(_pointer_to_string, s, x, offset, default_format_options<char>);
s64 to_string(string  *s, const void *x, u64 offset, format_options<char> opt) to_string_body(_pointer_to_string, s, x, offset, opt);

template<typename C, typename N>
s64 _c_remainder_to_string_reverse(C *s, N x, int precision, bool ignore_trailing_zeroes)
{
    if (precision > FLOAT_MAX_PRECISION)
        precision = FLOAT_MAX_PRECISION;

    if (precision <= 0)
        return 0;

    s64 i = 0;
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

    copy_string_reverse(buf + copy_start, s->data.data + i, buf_size - copy_start);
    i += (buf_size - copy_start);

    return i - offset;
}

s64 to_string(string  *s, float x)             to_string_body(_float_to_string, s, x, 0, default_format_options<char>, default_float_options)
s64 to_string(string  *s, float x, u64 offset) to_string_body(_float_to_string, s, x, offset, default_format_options<char>, default_float_options)
s64 to_string(string  *s, float x, u64 offset, format_options<char> opt, float_format_options fopt) to_string_body(_float_to_string, s, x, offset, opt, fopt)

s64 to_string(string  *s, double x)             to_string_body(_float_to_string, s, x, 0, default_format_options<char>, default_float_options)
s64 to_string(string  *s, double x, u64 offset) to_string_body(_float_to_string, s, x, offset, default_format_options<char>, default_float_options)
s64 to_string(string  *s, double x, u64 offset, format_options<char> opt, float_format_options fopt) to_string_body(_float_to_string, s, x, offset, opt, fopt)

// format

template<typename C>
s64 _format(u64 i, s64 written, string_base<C> *s, u64 offset, const_string_base<C> fmt)
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
                    break;

            }

            s->data.data[offset++] = fmt[i];
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

s64 internal::_format(u64 i, s64 written, string *s, u64 offset, const_string fmt)
{
    return ::_format(i, written, s, offset, fmt);
}

s64 internal::_format(u64 i, s64 written, wstring *s, u64 offset, const_wstring fmt)
{
    return ::_format(i, written, s, offset, fmt);
}

template<typename C>
s64 _format_skip_until_placeholder(u64 *_i, internal::_placeholder_info<C> *pl, string_base<C> *s, u64 offset, const_string_base<C> fmt)
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
                break;

            c = fmt[i];
        }
        else if (c == '%')
            break;

        s->data.data[offset++] = c;
        i++;
        written++;
    }

    if (i >= fmt.size)
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

    while (c >= '0' && c <= '9')
    {
        pl->options.pad_length += (pl->options.pad_length * 10) + (c - '0');
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
            pl->options.precision = -1;
        
        while (c >= '0' && c <= '9')
        {
            pl->options.precision += (pl->options.precision * 10) + (c - '0');
            j++;

            if_at_end_goto(j, fmt_end);
            c = fmt[j];
        }
    }

    if_at_end_goto(j, fmt_end);

#undef read_decimal
#undef if_at_end_goto
    
fmt_end:

    *_i = i + 1;
    return written;
}

s64 internal::_format_skip_until_placeholder(u64 *i, _placeholder_info<char>    *pl, string  *s, u64 offset, const_string  fmt)
{
    return ::_format_skip_until_placeholder(i, pl, s, offset, fmt);
}

s64 internal::_format_skip_until_placeholder(u64 *i, _placeholder_info<wchar_t> *pl, wstring *s, u64 offset, const_wstring fmt)
{
    return ::_format_skip_until_placeholder(i, pl, s, offset, fmt);
}
