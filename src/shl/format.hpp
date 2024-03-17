
/* format.hpp

String formatting library

Provides the to_string(...) functions with overloads for basic types,
e.g. int, char, bool, float, string (const char*, string* and const_string),
pointers, etc.

Also provides the format(string, fmt, ...) function to format a string object
using the format string and the rest of the arguments.
format() is not (sn)printf compliant, but provides similar functionality.

The formatting is generally type-safe, as every argument of a type T
in the variable argument list must have the following function signature
declared:

     to_string(string_base<C> *s, T x, u64 offset, format_options<C> opt)

This to_string overload is called with the argument when the placeholder
character '%' is encountered in the format string.
This also means that you can define your own to_string overload for other
types and use them with the format() function (be aware that you need to
reserve enough memory from within to_string on the string s to be able
to store the formatted value).

Basic format() example:

     string str = ""_s;
     format(&str, "hello %!\n"_cs, "world");

str will contain the string "hello world!" afterwards (don't forget to free(&str)).
format() (and the to_string functions) will allocate more memory in str as needed.

2024.01.14: to_string (and format) now has overloads that look like this:

     to_string(C *s, u64 ssize, T x, u64 offset, format_options<C> opt)

These overloads take a pointer to a char (or wchar_t) buffer of size ssize and
work similar to the overloads that take a string parameter, except these
overloads never allocate more memory.
These functions will only write up to ssize characters, which means that if the
buffers don't have enough memory to format the value / format string, the
return value will be ssize (or rather, ssize - offset if offset is not 0).

format() also accepts options for placeholders, similar to (sn)printf, although
more limited.
See tests/format.tests for examples.

tformat(fmt, ...) is a function that returns a const_string to a formatted
string using the arguments. The first time this function is called, a ring buffer
is allocated which has at least 4096 bytes of space, or more depending on the
page size of the system. When the program exits, the buffer is free'd.
Because the buffer is a ring buffer, it will continuously write to the same
buffer, then wrap back to the start, overwriting old formatted strings, hence
the name tformat (temporary format).
If you need the string for longer than the immediate use, copy the string.
All tformatted strings are null terminated.
*/

#pragma once

#include "shl/type_functions.hpp"
#include "shl/ring_buffer.hpp"
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
    .pad_char = (C)' ',
    .sign = (C)'\0',
    .precision = -1
};

s64 pad_string(char    *s, u64 ssize, char    chr, s64 count, u64 offset = 0);
s64 pad_string(wchar_t *s, u64 ssize, wchar_t chr, s64 count, u64 offset = 0);
s64 pad_string(string  *s, char    chr, s64 count, u64 offset = 0);
s64 pad_string(wstring *s, wchar_t chr, s64 count, u64 offset = 0);

s64 to_string(char    *s, u64 ssize, bool x, u64 offset = 0, format_options<char> opt = default_format_options<char>, bool as_text = false);
s64 to_string(wchar_t *s, u64 ssize, bool x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, bool as_text = false);
s64 to_string(string  *s, bool x, u64 offset = 0, format_options<char> opt = default_format_options<char>, bool as_text = false);
s64 to_string(wstring *s, bool x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, bool as_text = false);

s64 to_string(char    *s, u64 ssize, char    x, u64 offset = 0, format_options<char> opt = default_format_options<char>);
s64 to_string(wchar_t *s, u64 ssize, wchar_t x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>);
s64 to_string(string  *s, char    x, u64 offset = 0, format_options<char> opt = default_format_options<char>);
s64 to_string(wstring *s, wchar_t x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>);

// kind of redundant
s64 _to_string(char    *s, u64 ssize, const_string   x, u64 offset, format_options<char>    opt);
s64 _to_string(wchar_t *s, u64 ssize, const_wstring  x, u64 offset, format_options<wchar_t> opt);
s64 _to_string(string  *s, const_string   x, u64 offset, format_options<char>    opt);
s64 _to_string(wstring *s, const_wstring  x, u64 offset, format_options<wchar_t> opt);

// not redundant
s64 _to_string(char    *s, u64 ssize, const_wstring x, u64 offset, format_options<char>    opt);
s64 _to_string(wchar_t *s, u64 ssize, const_string  x, u64 offset, format_options<wchar_t> opt);
s64 _to_string(string  *s, const_wstring x, u64 offset, format_options<char>    opt);
s64 _to_string(wstring *s, const_string  x, u64 offset, format_options<wchar_t> opt);

template<typename T>
auto to_string(char *out, u64 ssize, T other, u64 offset = 0, format_options<char> opt = default_format_options<char>)
    -> decltype(_to_string(out, ssize, to_const_string(other), offset, opt))
{
    return _to_string(out, ssize, to_const_string(other), offset, opt);
}

template<typename T>
auto to_string(wchar_t *out, u64 ssize, T other, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>)
    -> decltype(_to_string(out, ssize, to_const_string(other), offset, opt))
{
    return _to_string(out, ssize, to_const_string(other), offset, opt);
}

template<typename C, typename T>
auto to_string(string_base<C> *out, T other, u64 offset = 0, format_options<C> opt = default_format_options<C>)
    -> decltype(_to_string(out, to_const_string(other), offset, opt))
{
    return _to_string(out, to_const_string(other), offset, opt);
}

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

s64 to_string(char    *s, u64 ssize, u8  x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(char    *s, u64 ssize, u16 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(char    *s, u64 ssize, u32 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(char    *s, u64 ssize, u64 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, u8  x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, u16 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, u32 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, u64 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);

s64 to_string(char    *s, u64 ssize, s8  x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(char    *s, u64 ssize, s16 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(char    *s, u64 ssize, s32 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(char    *s, u64 ssize, s64 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, s8  x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, s16 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, s32 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wchar_t *s, u64 ssize, s64 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);

s64 to_string(string  *s, u8  x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(string  *s, u16 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(string  *s, u32 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(string  *s, u64 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, u8  x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, u16 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, u32 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, u64 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);

s64 to_string(string  *s, s8  x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(string  *s, s16 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(string  *s, s32 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(string  *s, s64 x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, s8  x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, s16 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, s32 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);
s64 to_string(wstring *s, s64 x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, integer_format_options ioptions = default_integer_options);

// pointer
s64 to_string(char    *s, u64 ssize, const void *x, u64 offset = 0, format_options<char>    opt = default_format_options<char>);
s64 to_string(wchar_t *s, u64 ssize, const void *x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>);
s64 to_string(string  *s, const void *x, u64 offset = 0, format_options<char>    opt = default_format_options<char>);
s64 to_string(wstring *s, const void *x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>);

// float
struct float_format_options
{
    bool ignore_trailing_zeroes;    // if true, e.g. 0.1000 becomes 0.1
};

#define FLOAT_MAX_PRECISION 19

inline constexpr float_format_options default_float_options = 
{
    .ignore_trailing_zeroes = true
};

s64 to_string(char    *s, u64 ssize, float  x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    float_format_options foptions = default_float_options);
s64 to_string(char    *s, u64 ssize, double x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    float_format_options foptions = default_float_options);
s64 to_string(wchar_t *s, u64 ssize, float  x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, float_format_options foptions = default_float_options);
s64 to_string(wchar_t *s, u64 ssize, double x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, float_format_options foptions = default_float_options);
s64 to_string(string  *s, float  x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    float_format_options foptions = default_float_options);
s64 to_string(string  *s, double x, u64 offset = 0, format_options<char>    opt = default_format_options<char>,    float_format_options foptions = default_float_options);
s64 to_string(wstring *s, float  x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, float_format_options foptions = default_float_options);
s64 to_string(wstring *s, double x, u64 offset = 0, format_options<wchar_t> opt = default_format_options<wchar_t>, float_format_options foptions = default_float_options);

////////////////////
// format function
#define FORMAT_BUFFER_INCREMENT 32

namespace internal
{
s64 _format(u64 i, s64 written, char    *s, u64 ssize, u64 offset, const_string  fmt);
s64 _format(u64 i, s64 written, wchar_t *s, u64 ssize, u64 offset, const_wstring fmt);
s64 _format(u64 i, s64 written, string  *s, u64 offset, const_string  fmt);
s64 _format(u64 i, s64 written, wstring *s, u64 offset, const_wstring fmt);

template<typename C>
struct _placeholder_info
{
    format_options<C> options;
    bool has_placeholder;
    bool alternative; // #
};

s64 _format_skip_until_placeholder(u64 *i, _placeholder_info<char>    *pl, char    *s, u64 ssize, u64 offset, const_string  fmt);
s64 _format_skip_until_placeholder(u64 *i, _placeholder_info<wchar_t> *pl, wchar_t *s, u64 ssize, u64 offset, const_wstring fmt);
s64 _format_skip_until_placeholder(u64 *i, _placeholder_info<char>    *pl, string  *s, u64 offset, const_string  fmt);
s64 _format_skip_until_placeholder(u64 *i, _placeholder_info<wchar_t> *pl, wstring *s, u64 offset, const_wstring fmt);

template<typename T>
void *best_type_match(T &&arg)
{
    return (void*)(&arg);
}

// ok this function gets a bunch of types, ideally in decreasing size, 
// as template arguments and one argument of a type as regular argument,
// and it returns a pointer to the regular argument of the type that is
// either the same as the regular argument, or the first type in the template
// argument list that is smaller or equal in size to the regular argument.
// If no type matches, returns a void* to the argument.
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

// this is probably really bad for the stack but idk
template<typename C, typename T, typename... Ts>
s64 _format(u64 i, s64 written, C *s, u64 ssize, u64 offset, const_string_base<C> fmt, T &&arg, Ts &&...args)
{
    if (i >= fmt.size || offset >= ssize)
        return written;

    _placeholder_info<C> pl;
    pl.options = default_format_options<C>;
    pl.has_placeholder = false;
    pl.alternative = false;

    u64 pl_written = _format_skip_until_placeholder(&i, &pl, s, ssize, offset, fmt);
    written += pl_written;
    offset += pl_written;

    if (offset >= ssize)
        return written;

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
                tostring_written = to_string(s, ssize, *ptr, offset, pl.options, fopt);
                return _format(++i, written + tostring_written, s, ssize, offset + tostring_written, fmt, forward<Ts>(args)...);
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
\
            auto ptr = best_type_match<T, u64, u32, u16, u8>(forward<T>(arg));\
\
            if constexpr (!is_same(decltype(ptr), void*))\
            {\
                iopt.include_prefix = pl.alternative && (*ptr != 0);\
                tostring_written = to_string(s, ssize, *ptr, offset, pl.options, iopt);\
                return _format(++i, written + tostring_written, s, ssize, offset + tostring_written, fmt, forward<Ts>(args)...);\
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
        case 'u':
        case 'p':
            i++;
            break;
        }
    }
    else if (!pl.has_placeholder)
        return written;

    tostring_written = to_string(s, ssize, forward<T>(arg), offset, pl.options);
    return _format(i, written + tostring_written, s, ssize, offset + tostring_written, fmt, forward<Ts>(args)...);
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
        case 'u':
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

// c string format
template<typename C, typename... Ts>
s64 format(C *s, u64 ssize, const C *fmt, Ts &&...args)
{
    return format(s, ssize, 0, to_const_string(fmt), forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 format(C *s, u64 ssize, u64 offset, const C *fmt, Ts &&...args)
{
    return format(s, ssize, offset, to_const_string(fmt), forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 format(C *s, u64 ssize, const_string_base<C> fmt, Ts &&...args)
{
    return format(s, ssize, 0, fmt, forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 format(C *s, u64 ssize, u64 offset, const_string_base<C> fmt, Ts &&...args)
{
    return internal::_format(0, 0, s, ssize, offset, fmt, forward<Ts>(args)...);
}

// string format
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
        return -1;

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
namespace internal
{
struct tformat_buffer
{
    ring_buffer buffer;
    u64 offset;
};

template<typename C>
C *_buffer_data(const tformat_buffer *buf)
{
    return (C*)(buf->buffer.data);
}

template<typename C>
u64 _buffer_character_count(const tformat_buffer *buf)
{
    return (buf->buffer.size / sizeof(C)) - 1;
}

template<typename C>
u64 _buffer_offset(const tformat_buffer *buf)
{
    return buf->offset / sizeof(C);
}

template<typename C>
void _buffer_advance(tformat_buffer *buf, u64 count)
{
    buf->offset += (count * sizeof(C));

    while (buf->offset > buf->buffer.size)
        buf->offset -= buf->buffer.size;
}

tformat_buffer *_get_tformat_buffer_char();
tformat_buffer *_get_tformat_buffer_wchar();

template<typename C>
tformat_buffer *_get_tformat_buffer()
{
    if constexpr (is_same(C, char))
        return _get_tformat_buffer_char();
    else
        return _get_tformat_buffer_wchar();
}
}

template<typename C, typename... Ts>
const_string_base<C> tformat(const_string_base<C> fmt, Ts &&...args)
{
    internal::tformat_buffer *buf = internal::_get_tformat_buffer<C>();

    if (buf == nullptr)
        return const_string_base<C>{nullptr, 0};

    C *data = internal::_buffer_data<C>(buf);
    u64 size = internal::_buffer_character_count<C>(buf);
    u64 offset = internal::_buffer_offset<C>(buf);

    s64 written = format(data + offset, size, 0, fmt, forward<Ts>(args)...);

    if (written < 0)
        return const_string_base<C>{nullptr, 0};

    data[offset + written] = '\0';

    const_string_base<C> ret{data + offset, (u64)(written)};

    internal::_buffer_advance<C>(buf, (u64)(written) + 1);

    return ret;
}

template<typename C, typename... Ts>
const_string_base<C> tformat(const C *fmt, Ts &&...args)
{
    return tformat(to_const_string(fmt), forward<Ts>(args)...);
}

u64 get_tformat_buffer_size();
