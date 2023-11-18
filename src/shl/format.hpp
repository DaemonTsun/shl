
// format.hpp
// 
// string formatting library
//
// provides the to_string(...) functions with overloads for basic types,
// e.g. int, char, bool, float, string (const char*, string* and const_string),
// pointers, etc.
//
// also provides the format(string, fmt, ...) function to format a string object
// using the format string and the rest of the arguments.
// format() is not (sn)printf compliant, but provides similar functionality.
//
// the formatting is generally type-safe, as every argument of a type T
// in the variable argument list must have the following function signature
// declared:
//
//      to_string(string_base<C> *s, T x, u64 offset, format_options<C> opt)
//
// this to_string overload is called with the argument when the placeholder
// character '%' is encountered in the format string.
// This also means that you can define your own to_string overload for other
// types and use them with the format() function (be aware that you need to
// reserve enough memory from within to_string on the string s to be able
// to store the formatted value).
//
// Basic format() example:
// 
//      string str = ""_s;
//      format(&str, "hello %!\n"_cs, "world");
//
// str will contain the string "hello world!" afterwards (don't forget to free(&str)).
// format() (and the to_string functions) will allocate more memory in str as needed.
//
// format() also accepts options for placeholders, similar to (sn)printf, although
// more limited.
// See tests/format.tests for examples.

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
                    // for integers its zero pad count excluding sign or prefix.
                    // for floats its number of remainder places
};

template<typename C = char>
inline constexpr format_options<C> default_format_options
{
    .pad_length = 0,
    .pad_char = ' ',
    .sign = '\0',
    .precision = -1
};

s64 pad_string(string  *s, char    chr, s64 count, u64 offset);
s64 pad_string(wstring *s, wchar_t chr, s64 count, u64 offset);

s64 to_string(string  *s, bool x);
s64 to_string(string  *s, bool x, u64 offset);
s64 to_string(string  *s, bool x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, bool x, u64 offset, format_options<char> opt, bool as_text);
s64 to_string(wstring *s, bool x);
s64 to_string(wstring *s, bool x, u64 offset);
s64 to_string(wstring *s, bool x, u64 offset, format_options<wchar_t> options);
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
s64 to_string(string  *s, u8 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, u8 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, u16 x);
s64 to_string(string  *s, u16 x, u64 offset);
s64 to_string(string  *s, u16 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, u16 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, u32 x);
s64 to_string(string  *s, u32 x, u64 offset);
s64 to_string(string  *s, u32 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, u32 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, u64 x);
s64 to_string(string  *s, u64 x, u64 offset);
s64 to_string(string  *s, u64 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, u64 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(wstring *s, u8 x);
s64 to_string(wstring *s, u8 x, u64 offset);
s64 to_string(wstring *s, u8 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, u8 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);
s64 to_string(wstring *s, u16 x);
s64 to_string(wstring *s, u16 x, u64 offset);
s64 to_string(wstring *s, u16 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, u16 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);
s64 to_string(wstring *s, u32 x);
s64 to_string(wstring *s, u32 x, u64 offset);
s64 to_string(wstring *s, u32 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, u32 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);
s64 to_string(wstring *s, u64 x);
s64 to_string(wstring *s, u64 x, u64 offset);
s64 to_string(wstring *s, u64 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, u64 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);

s64 to_string(string  *s, s8 x);
s64 to_string(string  *s, s8 x, u64 offset);
s64 to_string(string  *s, s8 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, s8 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, s16 x);
s64 to_string(string  *s, s16 x, u64 offset);
s64 to_string(string  *s, s16 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, s16 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, s32 x);
s64 to_string(string  *s, s32 x, u64 offset);
s64 to_string(string  *s, s32 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, s32 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(string  *s, s64 x);
s64 to_string(string  *s, s64 x, u64 offset);
s64 to_string(string  *s, s64 x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, s64 x, u64 offset, format_options<char> opt, integer_format_options ioptions);
s64 to_string(wstring *s, s8 x);
s64 to_string(wstring *s, s8 x, u64 offset);
s64 to_string(wstring *s, s8 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, s8 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);
s64 to_string(wstring *s, s16 x);
s64 to_string(wstring *s, s16 x, u64 offset);
s64 to_string(wstring *s, s16 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, s16 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);
s64 to_string(wstring *s, s32 x);
s64 to_string(wstring *s, s32 x, u64 offset);
s64 to_string(wstring *s, s32 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, s32 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);
s64 to_string(wstring *s, s64 x);
s64 to_string(wstring *s, s64 x, u64 offset);
s64 to_string(wstring *s, s64 x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, s64 x, u64 offset, format_options<wchar_t> opt, integer_format_options ioptions);

s64 to_string(string  *s, const void *x);
s64 to_string(string  *s, const void *x, u64 offset);
s64 to_string(string  *s, const void *x, u64 offset, format_options<char> opt);
s64 to_string(wstring *s, const void *x);
s64 to_string(wstring *s, const void *x, u64 offset);
s64 to_string(wstring *s, const void *x, u64 offset, format_options<wchar_t> opt);

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
s64 to_string(string  *s, float x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, float x, u64 offset, format_options<char> opt, float_format_options foptions);
s64 to_string(wstring *s, float x);
s64 to_string(wstring *s, float x, u64 offset);
s64 to_string(wstring *s, float x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, float x, u64 offset, format_options<wchar_t> opt, float_format_options foptions);

s64 to_string(string  *s, double x);
s64 to_string(string  *s, double x, u64 offset);
s64 to_string(string  *s, double x, u64 offset, format_options<char> opt);
s64 to_string(string  *s, double x, u64 offset, format_options<char> opt, float_format_options foptions);
s64 to_string(wstring *s, double x);
s64 to_string(wstring *s, double x, u64 offset);
s64 to_string(wstring *s, double x, u64 offset, format_options<wchar_t> opt);
s64 to_string(wstring *s, double x, u64 offset, format_options<wchar_t> opt, float_format_options foptions);

////////////////////
// format function
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

template<typename T>
void *best_type_match(T &&arg)
{
    return (void*)(&arg);
}

template<typename T, typename T2, typename...Ts>
auto best_type_match(T &&arg)
{
    if constexpr (is_same(T, T2))
        return &arg;
    else if constexpr (sizeof(T) >= sizeof(T2))
        return (T2*)(void*)(&arg);
    else
        return best_type_match<T, Ts...>(forward<T>(arg));
}

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

    u64 tostring_written = 0;

    if (i < fmt.size)
    {
        // skiped all non-placeholder characters and we're not at the end
        C c = fmt[i];

        switch (c)
        {
        case 'f':
        {
            float_format_options fopt = default_float_options;
            fopt.ignore_trailing_zeroes = !pl.alternative;

            auto ptr = best_type_match<T, double, float>(forward<T>(arg));

            if constexpr (!is_same(decltype(ptr), void*))
            {
                tostring_written = to_string(s, *ptr, offset, pl.options, fopt);
                return _format(++i, written + tostring_written, s, offset + tostring_written, fmt, forward<Ts>(args)...);
            }
            else
                return -1;
        }
#define case_int_base(Lower, Upper, Base)\
        case Lower:\
        case Upper:\
        {\
            integer_format_options iopt = default_integer_options;\
            iopt.base = Base;\
            iopt.caps_letters = (c == Upper);\
            iopt.include_prefix = pl.alternative;\
\
            auto ptr = best_type_match<T, u64, u32, u16, u8>(forward<T>(arg));\
\
            if constexpr (!is_same(decltype(ptr), void*))\
            {\
                tostring_written = to_string(s, *ptr, offset, pl.options, iopt);\
                return _format(++i, written + tostring_written, s, offset + tostring_written, fmt, forward<Ts>(args)...);\
            }\
            else\
                return -1;\
        }
        case_int_base('b', 'B', 2);
        case_int_base('o', 'O', 8);
        case_int_base('x', 'X', 16);
#undef case_int_base
        case 'c':
        case 's':
        case 'd':
        case 'p':
            i++;
            break;
        }
    }
    else if (!pl.has_placeholder)
        return written;

    tostring_written = to_string(s, forward<T>(arg), offset, pl.options);
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

    if (written < 0)
        return written;

    if (offset + written > s->size)
    {
        s->size = offset + written;
        s->data[s->size] = '\0';
    }

    return written;
}

template<typename C, typename... Ts>
string_base<C> new_format(const C *fmt, Ts &&...args)
{
    string_base<C> _ret{};
    format(&_ret, 0, fmt, forward<Ts>(args)...);

    return _ret;
}

template<typename C, typename... Ts>
string_base<C> new_format(const_string_base<C> fmt, Ts &&...args)
{
    string_base<C> _ret{};
    format(&_ret, 0, fmt, forward<Ts>(args)...);

    return _ret;
}

// tformat
// this is not the max size that the temporary string can have, but
// the size at which the next format will reset the temporary string.
#define TEMP_STRING_MAX_SIZE 4096

namespace internal
{
void _get_temp_format_string(string  **s, u64 **offset);
void _get_temp_format_string(wstring **s, u64 **offset);
}

template<typename C, typename... Ts>
const_string_base<C> tformat(const_string_base<C> fmt, Ts &&...args)
{
    string_base<C> *s;
    u64 *offset;
    internal::_get_temp_format_string(&s, &offset);

    s64 written = format(s, *offset, fmt, forward<Ts>(args)...);

    if (written < 0)
        return const_string_base<C>{nullptr, 0};

    const_string_base<C> ret{s->data + *offset, static_cast<u64>(written)};
    *offset += written;
    return ret;
}

template<typename C, typename... Ts>
const_string_base<C> tformat(const C *fmt, Ts &&...args)
{
    return tformat(to_const_string(fmt), forward<Ts>(args)...);
}
