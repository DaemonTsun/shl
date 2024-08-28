
#include "shl/string_encoding.hpp"
#include "shl/print.hpp"

s64 put(io_handle h, c8 c, error *err)
{
    return io_write(h, &c, sizeof(c8), err);
}

template<typename C>
s64 _put_cs(io_handle h, const_string_base<C> s, error *err)
{
    internal::tformat_buffer *buf = internal::_get_tformat_buffer_c8();

    c8 *start = buf->buffer.data + buf->offset;
    s64 bytes_written = string_convert(s.c_str, s.size, start, buf->buffer.size);

    if (bytes_written < 0)
        return -1;

    buf->buffer.data[buf->offset + bytes_written] = '\0';
    
    internal::_buffer_advance<c8>(buf, bytes_written + 1);

    return _put(h, const_string{start, bytes_written}, err);
}

s64 _put(io_handle h, const_string  s, error *err)
{
    return io_write(h, s.c_str, s.size, err);
}

s64 _put(io_handle h, const_u16string s, error *err) { return _put_cs(h, s, err); }
s64 _put(io_handle h, const_u32string s, error *err) { return _put_cs(h, s, err); }

s64 put(c8 c, error *err) { return put(stdout_handle(), c, err); }
s64 _put(const_string    s, error *err) { return _put(stdout_handle(), s, err); }
s64 _put(const_u16string s, error *err) { return _put(stdout_handle(), s, err); }
s64 _put(const_u32string s, error *err) { return _put(stdout_handle(), s, err); }
