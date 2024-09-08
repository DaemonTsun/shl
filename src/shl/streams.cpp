
#include "shl/assert.hpp"
#include "shl/chunk_array.hpp"
#include "shl/streams.hpp"

template<typename C>
bool _read_entire_file_ms(const C *path, memory_stream *out, error *err)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream{};

    if (!init(&fstream, path, open_mode::Read, err))
        return false;

    bool ret = read_entire_file(&fstream, out, err);
    bool free_ret = free(&fstream, err);

    return ret && free_ret;
}

bool read_entire_file(const c8 *path, memory_stream *out, error *err)
{
    return _read_entire_file_ms(path, out, err);
}

bool read_entire_file(const c16 *path, memory_stream *out, error *err)
{
    return _read_entire_file_ms(path, out, err);
}

bool read_entire_file(const c32 *path, memory_stream *out, error *err)
{
    return _read_entire_file_ms(path, out, err);
}

bool read_entire_file(file_stream *stream, memory_stream *out, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    s64 sz = get_file_size(stream, err);

    if (sz == -1)
        return false;

    init(out, sz);

    if (sz == 0)
        return true;

    if (read_entire_file(stream, out->data, sz, err) == -1)
        return false;

    return true;
}

template<typename C>
bool _read_entire_file_s(const C *path, string *out, error *err)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream{};

    if (!init(&fstream, path, open_mode::Read, err))
        return false;

    bool ret = read_entire_file(&fstream, out, err);
    bool free_ret = free(&fstream, err);
    
    return ret && free_ret;
}

bool read_entire_file(const c8 *path, string *out, error *err)
{
    return _read_entire_file_s(path, out, err);
}

bool read_entire_file(const c16 *path, string *out, error *err)
{
    return _read_entire_file_s(path, out, err);
}

bool read_entire_file(const c32 *path, string *out, error *err)
{
    return _read_entire_file_s(path, out, err);
}

bool read_entire_file(file_stream *stream, string *out, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    s64 sz = get_file_size(stream);

    if (sz < 0)
        return false;

    reserve((array<char>*)out, sz + 1);
    out->data[sz] = '\0';
    out->size = sz;

    if (sz == 0)
        return true;

    if (read_entire_file(stream, out->data, sz, err) == -1)
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

bool read_entire_pipe(pipe_t *p, memory_stream *out, error *err)
{
    assert(p != nullptr);

    return read_entire_io(p->read, out, err);
}

bool read_entire_pipe(pipe_t *p, string *out, error *err)
{
    assert(p != nullptr);

    return read_entire_io(p->read, out, err);
}

template<typename C>
inline s64 _write_fs(file_stream *stream, const_string_base<C> cs, error *err)
{
    assert(stream != nullptr);

    s64 ret = write(stream, (const void*)cs.c_str, cs.size * sizeof(C), err);

    if (!is_ok(stream))
        return -1;

    return ret;
}

s64 _write(file_stream *stream, const_string  s, error *err)
{
    return _write_fs(stream, s, err);
}

template<typename C>
s64 _write_ms(memory_stream *stream, const_string_base<C> cs)
{
    assert(stream != nullptr);

    s64 ret = write(stream, (const void*)cs.c_str, cs.size * sizeof(C));

    if (!is_ok(stream))
        return -1;

    return ret;
}

s64 _write(memory_stream *stream, const_string  s)
{
    return _write_ms(stream, s);
}

s64 _write(io_handle h, const_string  s, error *err)
{
    return io_write(h, s.c_str, s.size, err);
}

s64 _write(pipe_t *p, const_string  s, error *err)
{
    return _write(p->write, s, err);
}

