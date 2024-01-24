
#pragma once

/* print.hpp

Printing header. Defines functions to print or format strings to an io_handle.

Functions:

put(h, x[, err])    prints x to io_handle h. x may be a character or a string.
                    Does NOT write a newline to h.

put(x[, err])   prints x to stdout_handle(). Same rules apply as put(h, x).

tprint(h, fmt, ...) formats a string using tformat(fmt, ...) and prints the
                    string to io_handle h.

tprint(fmt, ...)    formats a string using tformat(fmt, ...) and prints the
                    string to stdout_handle().
*/

#include "shl/io.hpp"
#include "shl/format.hpp"

s64 put(io_handle h, char    c, error *err);
s64 put(io_handle h, wchar_t c, error *err);
s64 _put(io_handle h, const_string  s, error *err);
s64 _put(io_handle h, const_wstring s, error *err);

template<typename T>
auto put(io_handle h, T str, error *err = nullptr)
    -> decltype(_put(h, to_const_string(str), err))
{
    return _put(h, to_const_string(str), err);
}

s64 put(char    c, error *err);
s64 put(wchar_t c, error *err);
s64 _put(const_string  s, error *err);
s64 _put(const_wstring s, error *err);

template<typename T>
auto put(T str, error *err = nullptr)
    -> decltype(_put(to_const_string(str), err))
{
    return _put(to_const_string(str), err);
}

template<typename C, typename... Ts>
s64 tprint(io_handle h, const_string_base<C> fmt, Ts &&...args)
{
    auto res = tformat(fmt, forward<Ts>(args)...);

    if (!res)
        return -1;

    return put(h, res);
}

template<typename C, typename... Ts>
s64 tprint(io_handle h, const C *fmt, Ts &&...args)
{
    return tprint(h, to_const_string(fmt), forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 tprint(const_string_base<C> fmt, Ts &&...args)
{
    return tfprint(stdout_handle(), fmt, forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 tprint(const C *fmt, Ts &&...args)
{
    return tprint(stdout_handle(), to_const_string(fmt), forward<Ts>(args)...);
}
