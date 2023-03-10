
#include <wchar.h>
#include "shl/print.hpp"

s64 put(FILE *f, char c)
{
    return fputc(c, f);
}

s64 put(FILE *f, wchar_t c)
{
    return fputwc(c, f);
}

s64 put(FILE *f, const char    *s)
{
    return fputs(s, f);
}

s64 put(FILE *f, const wchar_t *s)
{
    return fputws(s, f);
}

template<typename C>
s64 _put(FILE *f, const_string_base<C> s)
{
    // we use fwrite here because s can contain non-terminating null characters
    return fwrite(s.c_str, s.size, sizeof(C), f);
}

s64 put(FILE *f, const_string  s)
{
    return _put(f, s);
}

s64 put(FILE *f, const_wstring s)
{
    return _put(f, s);
}

s64 put(FILE *f, const string  *s)
{
    return _put(f, to_const_string(s));
}

s64 put(FILE *f, const wstring *s)
{
    return _put(f, to_const_string(s));
}

s64 put(char    c) { return put(stdout, c); }
s64 put(wchar_t c)  { return put(stdout, c); }
s64 put(const char    *s) { return put(stdout, s); }
s64 put(const wchar_t *s) { return put(stdout, s); }
s64 put(const_string  s)  { return put(stdout, s); }
s64 put(const_wstring s)  { return put(stdout, s); }
s64 put(const string  *s) { return put(stdout, s); }
s64 put(const wstring *s) { return put(stdout, s); }
