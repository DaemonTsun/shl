
#pragma once

#include <stdio.h>
#include "shl/format.hpp"

s64 put(FILE *f, char    c);
s64 put(FILE *f, wchar_t c);
s64 put(FILE *f, const char    *s);
s64 put(FILE *f, const wchar_t *s);
s64 put(FILE *f, const_string  s);
s64 put(FILE *f, const_wstring s);
s64 put(FILE *f, const string  *s);
s64 put(FILE *f, const wstring *s);
s64 put(char    c);
s64 put(wchar_t c);
s64 put(const char    *s);
s64 put(const wchar_t *s);
s64 put(const_string  s);
s64 put(const_wstring s);
s64 put(const string  *s);
s64 put(const wstring *s);

template<typename C, typename... Ts>
s64 tprint(FILE *f, const_string_base<C> fmt, Ts &&...args)
{
    auto res = tformat(fmt, forward<Ts>(args)...);

    if (!res)
        return -1;

    return put(f, res);
}

template<typename C, typename... Ts>
s64 tprint(FILE *f, const C *fmt, Ts &&...args)
{
    return tprint(f, to_const_string(fmt), forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 tprint(const_string_base<C> fmt, Ts &&...args)
{
    return tfprint(stdout, fmt, forward<Ts>(args)...);
}

template<typename C, typename... Ts>
s64 tprint(const C *fmt, Ts &&...args)
{
    return tprint(stdout, to_const_string(fmt), forward<Ts>(args)...);
}
