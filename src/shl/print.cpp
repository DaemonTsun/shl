
#include "shl/print.hpp"

s64 put(io_handle h, char c, error *err)
{
    return io_write(h, &c, sizeof(char), err);
}

s64 put(io_handle h, wchar_t c, error *err)
{
    return io_write(h, (char*)&c, sizeof(wchar_t), err);
}

template<typename C>
s64 _put_cs(io_handle h, const_string_base<C> s, error *err)
{
    return io_write(h, (const char*)s.c_str, s.size * sizeof(C), err);
}

s64 _put(io_handle h, const_string  s, error *err)
{
    return _put_cs(h, s, err);
}

s64 _put(io_handle h, const_wstring s, error *err)
{
    return _put_cs(h, s, err);
}

s64 put(char    c, error *err)        { return put(stdout_handle(), c, err); }
s64 put(wchar_t c, error *err)        { return put(stdout_handle(), c, err); }
s64 _put(const_string  s, error *err) { return _put_cs(stdout_handle(), s, err); }
s64 _put(const_wstring s, error *err) { return _put_cs(stdout_handle(), s, err); }
