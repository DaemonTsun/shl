
#include <stdlib.h> // wcstombs
#include "shl/print.hpp"

s64 put(io_handle h, char c, error *err)
{
    return io_write(h, &c, sizeof(char), err);
}

s64 put(io_handle h, wchar_t c, error *err)
{
    return io_write(h, (char*)&c, sizeof(wchar_t), err);
}

s64 _put(io_handle h, const_string  s, error *err)
{
    return io_write(h, s.c_str, s.size, err);
}

s64 _put(io_handle h, const_wstring s, error *err)
{
    internal::tformat_buffer *buf = internal::_get_tformat_buffer_char();

    char *start = buf->buffer.data + buf->offset;
    u64 bytes_written = wcstombs(start, s.c_str, buf->buffer.size);

    if (bytes_written == (u64)-1)
        return -1;

    buf->buffer.data[buf->offset + bytes_written] = '\0';
    
    internal::_buffer_advance<char>(buf, bytes_written);

    return _put(h, const_string{start, bytes_written}, err);
}

s64 put(char    c, error *err)        { return put(stdout_handle(), c, err); }
s64 put(wchar_t c, error *err)        { return put(stdout_handle(), c, err); }
s64 _put(const_string  s, error *err) { return _put(stdout_handle(), s, err); }
s64 _put(const_wstring s, error *err) { return _put(stdout_handle(), s, err); }
