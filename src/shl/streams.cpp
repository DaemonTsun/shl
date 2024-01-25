
#include <assert.h>

#include "shl/chunk_array.hpp"
#include "shl/defer.hpp"
#include "shl/streams.hpp"

bool read_entire_file(const char *path, memory_stream *out, error *err)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream{};
    defer { close(&fstream); };

    if (!open(&fstream, path, MODE_READ, false, false, err))
        return false;

    if (!read_entire_file(&fstream, out, err))
        return false;
    
    return true;
}

bool read_entire_file(file_stream *stream, memory_stream *out, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    u64 sz = calculate_file_size(stream);

    if (sz == 0)
        return true;

    init(out, sz);

    if (read_entire_file(stream, out->data, sz, err) == 0)
        return false;

    return true;
}

bool read_entire_file(const char *path, string *out, error *err)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream{};
    defer { close(&fstream); };

    if (!open(&fstream, path, MODE_READ, false, false, err))
        return false;

    if (!read_entire_file(&fstream, out, err))
        return false;
    
    return true;
}

bool read_entire_file(file_stream *stream, string *out, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    s64 sz = calculate_file_size(stream);

    if (sz < 0)
        return false;

    if (sz == 0)
        return true;

    reserve((array<char>*)out, sz + 1);
    out->data[sz] = '\0';
    out->size = sz;

    if (read_entire_file(stream, out->data, sz, err) == 0)
        return false;

    return true;
}

bool read_entire_io(io_handle h, memory_stream *out, error *err)
{
    assert(out != nullptr);

    s64 sz = io_size(h, err);

    if (sz < 0)
        return false;

    init(out, sz);

    if (sz == 0)
        return true;

    if (io_is_pipe(h) && !io_poll_read(h, 0, err))
        return false;

    if (io_read(h, out->data, sz, err) < 0)
        return false;

    return true;
}

bool read_entire_io(io_handle h, string *out, error *err)
{
    assert(out != nullptr);

    s64 sz = io_size(h, err);

    if (sz < 0)
        return false;

    reserve((array<char>*)out, sz + 1);
    out->data[sz] = '\0';
    out->size = sz;

    if (sz == 0)
        return true;

    if (io_is_pipe(h) && !io_poll_read(h, 0, err))
        return false;

    s64 read_size = io_read(h, out->data, sz, err);

    if (read_size < 0)
        return false;

    assert(read_size <= sz);

    if (read_size != sz)
    {
        out->size = read_size;
        out->data[read_size] = '\0';
    }

    return true;
}

bool read_entire_pipe(pipe *p, memory_stream *out, error *err)
{
    assert(p != nullptr);

    return read_entire_io(p->read, out, err);
}

bool read_entire_pipe(pipe *p, string *out, error *err)
{
    assert(p != nullptr);

    return read_entire_io(p->read, out, err);
}

template<typename C>
s64 _write_fs(file_stream *stream, const_string_base<C> cs, error *err)
{
    assert(stream != nullptr);

    s64 ret = write(stream, (const void*)cs.c_str, cs.size * sizeof(C));

    if (!is_ok(stream))
        return -1;

    return ret;
}

s64 _write(file_stream *stream, const_string  s, error *err)
{
    return _write_fs(stream, s, err);
}

s64 _write(file_stream *stream, const_wstring s, error *err)
{
    return _write_fs(stream, s, err);
}

template<typename C>
s64 _write_ms(memory_stream *stream, const_string_base<C> cs, error *err)
{
    assert(stream != nullptr);

    s64 ret = write(stream, (const void*)cs.c_str, cs.size * sizeof(C));

    if (!is_ok(stream))
        return -1;

    return ret;
}

s64 _write(memory_stream *stream, const_string  s, error *err)
{
    return _write_ms(stream, s, err);
}

s64 _write(memory_stream *stream, const_wstring s, error *err)
{
    return _write_ms(stream, s, err);
}

s64 _write(io_handle h, const_string  s, error *err)
{
    return io_write(h, s.c_str, s.size, err);
}

s64 _write(io_handle h, const_wstring s, error *err)
{
    return io_write(h, (const char*)s.c_str, s.size * sizeof(wchar_t), err);
}

s64 _write(pipe *p, const_string  s, error *err)
{
    return _write(p->write, s, err);
}

s64 _write(pipe *p, const_wstring s, error *err)
{
    return _write(p->write, s, err);
}
