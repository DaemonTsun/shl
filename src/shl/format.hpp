
#pragma once

#include "shl/type_functions.hpp"
#include "shl/string.hpp"

#define DEFAULT_INT_PRECISION 0
#define DEFAULT_FLOAT_PRECISION 6

template<typename C = char>
struct format_options
{
    int pad_length;
    C pad_char;
    C sign;         // only used by numbers, either +, - or 0x00
    int precision;  // only used by numbers, for float its remainder places
                    // for integers, its zero pad count, excluding sign or prefix.
};

template<typename C = char>
inline constexpr format_options<C> default_format_options
{
    .pad_length = 0,
    .pad_char = ' ',
    .sign = '\0',
    .precision = -1
};

s64 to_string(string  *s, bool x);
s64 to_string(string  *s, bool x, u64 offset);
s64 to_string(string  *s, bool x, u64 offset, format_options<char> opt, bool as_text);
s64 to_string(wstring *s, bool x);
s64 to_string(wstring *s, bool x, u64 offset);
s64 to_string(wstring *s, bool x, u64 offset, format_options<wchar_t> options, bool as_text);

s64 to_string(string  *s, char    x);
s64 to_string(string  *s, char    x, u64 offset);
s64 to_string(string  *s, char    x, u64 offset, format_options<char> opt);
s64 to_string(wstring *s, wchar_t x);
s64 to_string(wstring *s, wchar_t x, u64 offset);
s64 to_string(wstring *s, wchar_t x, u64 offset, format_options<wchar_t> options);

// kind of redundant
s64 to_string(string  *s, const char    *x);
s64 to_string(string  *s, const char    *x, u64 offset);
s64 to_string(string  *s, const char    *x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, const_string   x);
s64 to_string(string  *s, const_string   x, u64 offset);
s64 to_string(string  *s, const_string   x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, const string  *x);
s64 to_string(string  *s, const string  *x, u64 offset);
s64 to_string(string  *s, const string  *x, u64 offset, format_options<char> opt);
s64 to_string(wstring *s, const wchar_t *x);
s64 to_string(wstring *s, const wchar_t *x, u64 offset);
s64 to_string(wstring *s, const wchar_t *x, u64 offset, format_options<wchar_t> options);
s64 to_string(wstring *s, const_wstring  x);
s64 to_string(wstring *s, const_wstring  x, u64 offset);
s64 to_string(wstring *s, const_wstring  x, u64 offset, format_options<wchar_t> options);
s64 to_string(wstring *s, const wstring *x);
s64 to_string(wstring *s, const wstring *x, u64 offset);
s64 to_string(wstring *s, const wstring *x, u64 offset, format_options<wchar_t> options);

struct integer_format_options
{
    int base;               // 2, 8, 10 or 16
    bool include_prefix;    // 0b for base 2, 0 for base 8, 0x for base 16
    bool caps_letters;      // only for hex, whether to use ABCDEF
    bool caps_prefix;       // if include_prefix is on and base is 2 or 16, make the
                            // prefix letter caps
};

inline constexpr integer_format_options default_integer_options = 
{
    .base = 10,
    .include_prefix = false,
    .caps_letters = false,
    .caps_prefix = false
};

s64 to_string(string  *s, u8 x);
s64 to_string(string  *s, u8 x, u64 offset);
s64 to_string(string  *s, u8 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, u16 x);
s64 to_string(string  *s, u16 x, u64 offset);
s64 to_string(string  *s, u16 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, u32 x);
s64 to_string(string  *s, u32 x, u64 offset);
s64 to_string(string  *s, u32 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, u64 x);
s64 to_string(string  *s, u64 x, u64 offset);
s64 to_string(string  *s, u64 x, u64 offset, format_options<char> opt, integer_format_options ioptions);

s64 to_string(string  *s, s8 x);
s64 to_string(string  *s, s8 x, u64 offset);
s64 to_string(string  *s, s8 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, s16 x);
s64 to_string(string  *s, s16 x, u64 offset);
s64 to_string(string  *s, s16 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, s32 x);
s64 to_string(string  *s, s32 x, u64 offset);
s64 to_string(string  *s, s32 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, s64 x);
s64 to_string(string  *s, s64 x, u64 offset);
s64 to_string(string  *s, s64 x, u64 offset, format_options<char> opt, integer_format_options ioptions);

s64 to_string(string  *s, const void *x);
s64 to_string(string  *s, const void *x, u64 offset);
s64 to_string(string  *s, const void *x, u64 offset, format_options<char> opt);

struct float_format_options
{
    bool ignore_trailing_zeroes;    // if true, e.g. 0.1000 becomes 0.1
};

#define FLOAT_MAX_PRECISION 19

inline constexpr float_format_options default_float_options = 
{
    .ignore_trailing_zeroes = true
};

s64 to_string(string  *s, float x);
s64 to_string(string  *s, float x, u64 offset);
s64 to_string(string  *s, float x, u64 offset, format_options<char> opt, float_format_options foptions);

s64 to_string(string  *s, double x);
s64 to_string(string  *s, double x, u64 offset);
s64 to_string(string  *s, double x, u64 offset, format_options<char> opt, float_format_options foptions);

// format

#define FORMAT_BUFFER_INCREMENT 32

namespace internal
{
s64 _format(u64 i, s64 written, string  *s, u64 offset, const_string  fmt);
s64 _format(u64 i, s64 written, wstring *s, u64 offset, const_wstring fmt);

template<typename C>
struct _placeholder_info
{
    format_options<C> options;
    bool has_placeholder;
    bool alternative; // #
};

s64 _format_skip_until_placeholder(u64 *i, _placeholder_info<char>    *pl, string  *s, u64 offset, const_string  fmt);
s64 _format_skip_until_placeholder(u64 *i, _placeholder_info<wchar_t> *pl, wstring *s, u64 offset, const_wstring fmt);

template<typename C, typename T, typename... Ts>
s64 _format(u64 i, s64 written, string_base<C> *s, u64 offset, const_string_base<C> fmt, T &&arg, Ts &&...args)
{
    if (i >= fmt.size)
        return written;

    _placeholder_info<C> pl;
    pl.options = default_format_options<C>;
    pl.has_placeholder = false;
    pl.alternative = false;

    u64 pl_written = _format_skip_until_placeholder(&i, &pl, s, offset, fmt);
    written += pl_written;
    offset += pl_written;

    if (i < fmt.size)
    {
        // skiped all non-placeholder characters and we're not at the end
        C c = fmt[i];

        switch (c)
        {
        case 'f':
        {
            /*
            float_format_options fopt{.precision = precision, .number_pad_length = 0, .force_sign = (sign == '+'), .ignore_trailing_zeroes = true};

            double val = *(double*)(void*)(&arg);

            written += to_string(s, val, offset, opt, fopt);
            return _format(++i, written, s, offset + written, fmt, forward<Ts>(args)...);
            */
            i++;
            break;
        }
        default:
            break;
        }
    }
    else if (!pl.has_placeholder)
        return written;

    u64 tostring_written = to_string(s, forward<T>(arg), offset, pl.options);
    return _format(i, written + tostring_written, s, offset + tostring_written, fmt, forward<Ts>(args)...);
}
}

template<typename C, typename... Ts>
s64 format(string_base<C> *s, const C *fmt, Ts &&...args)
{
    return format(s, 0, to_const_string(fmt), forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 format(string_base<C> *s, u64 offset, const C *fmt, Ts &&...args)
{
    return format(s, offset, to_const_string(fmt), forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 format(string_base<C> *s, const_string_base<C> fmt, Ts &&...args)
{
    return format(s, 0, fmt, forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 format(string_base<C> *s, u64 offset, const_string_base<C> fmt, Ts &&...args)
{
    string_reserve(s, fmt.size);
    s64 written = internal::_format(0, 0, s, offset, fmt, forward<Ts>(args)...);

    if (offset + written > s->data.size)
    {
        s->data.size = offset + written;
        s->data.data[s->data.size] = '\0';
    }

    return written;
}
