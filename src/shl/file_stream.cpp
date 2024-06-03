
// v1.2
// add error handling
// v1.1
// add seek_next_alignment

#include "shl/assert.hpp"
#include "shl/platform.hpp"
#include "shl/memory.hpp"
#include "shl/bits.hpp"
#include "shl/math.hpp"

#if Windows
#include <windows.h>
#else
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/io.hpp"
#endif

#include "shl/file_stream.hpp"

bool init(file_stream *stream, const char *path, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, err));
}

bool init(file_stream *stream, const char *path, int flags, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, flags, err));
}

bool init(file_stream *stream, const char *path, int flags, int mode, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, flags, mode, err));
}

bool init(file_stream *stream, const char *path, int flags, int mode, int permissions, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, flags, mode, permissions, err));
}

bool init(file_stream *stream, const wchar_t *path, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, err));
}

bool init(file_stream *stream, const wchar_t *path, int flags, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, flags, err));
}

bool init(file_stream *stream, const wchar_t *path, int flags, int mode, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, flags, mode, err));
}

bool init(file_stream *stream, const wchar_t *path, int flags, int mode, int permissions, error *err)
{
    assert(stream != nullptr);
    return init(stream, io_open(path, flags, mode, permissions, err));
}

bool init(file_stream *stream, io_handle handle, error *err)
{
    assert(stream != nullptr);
    stream->handle = handle;
    
    if (handle == INVALID_IO_HANDLE || get_file_size(stream, err) < 0)
        return false;

    return true;
}

bool free(file_stream *stream, error *err)
{
    if (stream == nullptr)
        return true;

#if Windows
    if (stream->handle != nullptr && !CloseHandle(stream->handle))
    {
        set_GetLastError_error(err);
        return false;
    }

    stream->handle = nullptr;
#else
    if (stream->handle != -1)
    if (sys_int ret = close(stream->handle); ret < 0)
    {
        set_error_by_code(err, -ret);
        return false;
    }

    stream->handle = -1;
#endif

    return true;
}

bool is_open(file_stream *stream)
{
    assert(stream != nullptr);

    return stream->handle != INVALID_IO_HANDLE;
}

bool is_at_end(file_stream *stream, error *err)
{
    assert(stream != nullptr);

    s64 cur = io_tell(stream->handle, err);

    if (cur == -1 || stream->cached_size == -1)
        return false;

    return cur >= stream->cached_size;
}

bool is_ok(file_stream *stream, error *err)
{
    assert(stream != nullptr);

    return is_open(stream) && (!is_at_end(stream, err));
}

s64 get_file_size(file_stream *stream, error *err)
{
    assert(stream != nullptr);

    stream->cached_size = io_size(stream->handle, err);

    return stream->cached_size;
}

s64 block_count(const file_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    return stream->cached_size / block_size;
}

s64 current_block_number(file_stream *stream, s64 block_size, error *err)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    s64 pos = tell(stream, err);

    if (pos == -1)
        return -1;

    return (s64)(pos / block_size);
}

s64 current_block_offset(file_stream *stream, s64 block_size, error *err)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    s64 cur = tell(stream, err);

    if (cur < 0)
        return -1;

    return floor_multiple(cur, block_size);
}

s64 current_block_offset2(file_stream *stream, s64 block_size, error *err)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    s64 cur = tell(stream, err);

    if (cur < 0)
        return -1;

    return (s64)floor_multiple2(cur, block_size);
}

s64 seek(file_stream *stream, s64 offset, int whence, error *err)
{
    assert(stream != nullptr);

    return io_seek(stream->handle, offset, whence, err);
}

s64 seek_offset(file_stream *stream, s64 offset, error *err)
{
    return seek(stream, offset, IO_SEEK_CUR, err);
}

s64 seek_from_start(file_stream *stream, s64 offset, error *err)
{
    return seek(stream, offset, IO_SEEK_SET, err);
}

s64 seek_from_end(file_stream *stream, s64 offset, error *err)
{
    return seek(stream, offset, IO_SEEK_END, err);
}

s64 seek_block(file_stream *stream, s64 nth_block, s64 block_size, int whence, error *err)
{
    assert(stream != nullptr);

    if (whence != IO_SEEK_CUR)
        return seek(stream, nth_block * block_size, whence, err);

    return seek_block_offset(stream, nth_block, block_size, err);
}

s64 seek_block_offset(file_stream *stream, s64 nth_block, s64 block_size, error *err)
{
    s64 cur = -1;

    if (is_pow2(block_size))
        cur = current_block_offset2(stream, block_size);
    else
        cur = current_block_offset(stream, block_size);

    if (cur < 0)
        return -1;

    return seek_from_start(stream, cur + nth_block * block_size, err);
}

s64 seek_block_from_start(file_stream *stream, s64 nth_block, s64 block_size, error *err)
{
    return seek_from_start(stream, nth_block * block_size, err);
}

s64 seek_block_from_end(file_stream *stream, s64 nth_block, s64 block_size, error *err)
{
    return seek_from_end(stream, nth_block * block_size, err);
}

s64 seek_next_alignment(file_stream *stream, s64 alignment, error *err)
{
    assert(stream != nullptr);
    assert(alignment > 0);

    s64 npos = tell(stream, err);

    if (npos < 0)
        return -1;

    if (is_pow2(alignment))
        npos = ceil_multiple2(npos + 1, alignment);
    else
        npos = ceil_multiple(npos +1, alignment);

    return seek_from_start(stream, npos, err);
}

s64 seek_next_alignment2(file_stream *stream, s64 alignment, error *err)
{
    assert(stream != nullptr);
    assert(alignment > 0);

    s64 npos = tell(stream, err);

    if (npos < 0)
        return -1;

    npos = ceil_multiple2(npos + 1, alignment);

    return seek_from_start(stream, npos, err);
}

s64 tell(file_stream *stream, error *err)
{
    assert(stream != nullptr);

    return io_tell(stream->handle, err);
}

bool rewind(file_stream *stream, error *err)
{
    assert(stream != nullptr);
    
    return seek(stream, 0L, IO_SEEK_SET, err) == 0;
}

s64 read(file_stream *stream, void *out, s64 size, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);
    
    return io_read(stream->handle, (char*)out, size, err);
}

s64 read(file_stream *stream, void *out, s64 size, s64 nmemb, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);
    
    s64 ret = io_read(stream->handle, (char*)out, size * nmemb, err);
    
    if (ret < 0)
        return -1;

    return ret / size;
}

s64 read_at(file_stream *stream, void *out, s64 offset, s64 size, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);
    
    if (seek_from_start(stream, offset, err) == -1)
        return -1;

    return read(stream, out, size, err);
}

s64 read_at(file_stream *stream, void *out, s64 offset, s64 size, s64 nmemb, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);
    
    if (seek_from_start(stream, offset, err) == -1)
        return -1;

    return read(stream, out, size, nmemb, err);
}

s64 read_block(file_stream *stream, void *out, s64 block_size, error *err)
{
    s64 ret = read(stream, out, block_size, err);

    if (ret == -1)
        return -1;

    if (ret < block_size)
        return 0;

    return ret;
}

s64 read_block(file_stream *stream, void *out, s64 nth_block, s64 block_size, error *err)
{
    if (seek_block_from_start(stream, nth_block, block_size, err) == -1)
        return -1;

    return read_block(stream, out, block_size, err);
}

s64 read_blocks(file_stream *stream, void *out, s64 block_count, s64 block_size, error *err)
{
    return read(stream, out, block_size, block_count, err);
}

s64 read_blocks(file_stream *stream, void *out, s64 nth_block, s64 block_count, s64 block_size, error *err)
{
    assert(out != nullptr);

    if (seek_block_from_start(stream, nth_block, block_size, err) == -1)
        return -1;

    return read(stream, out, block_size, block_count, err);
}

s64 read_entire_file(file_stream *stream, void *out, s64 max_size, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    s64 sz = get_file_size(stream, err);

    if (sz > max_size)
        sz = max_size;

    if (sz == -1)
        return -1;
    
    s64 old_pos = tell(stream, err);

    if (old_pos < 0)
        return -1;

    if (seek(stream, 0, IO_SEEK_SET, err) == -1) 
        return -1;

    s64 ret = io_read(stream->handle, (char*)out, sz, err);

    if (seek(stream, old_pos, IO_SEEK_SET, err) == -1)
        return -1;

    return ret;
}

s64 write(file_stream *stream, const void *in, s64 size, error *err)
{
    assert(in != nullptr);

    return io_write(stream->handle, (const char*)in, size, err);
}

s64 write(file_stream *stream, const void *in, s64 size, s64 nmemb, error *err)
{
    assert(in != nullptr);

    s64 ret = io_write(stream->handle, (const char*)in, size * nmemb, err);

    if (ret == -1)
        return -1;

    return ret / size;
}

s64 write_at(file_stream *stream, const void *in, s64 offset, s64 size, error *err)
{
    if (seek(stream, offset, IO_SEEK_SET, err) == -1)
        return -1;

    return write(stream, in, size, err);
}

s64 write_at(file_stream *stream, const void *in, s64 offset, s64 size, s64 nmemb, error *err)
{
    if (seek(stream, offset, IO_SEEK_SET, err) == -1)
        return -1;

    return write(stream, in, size, nmemb, err);
}

s64 write_block(file_stream *stream, const void *in, s64 block_size, error *err)
{
    return write(stream, in, block_size, err);
}

s64 write_block(file_stream *stream, const void *in, s64 nth_block, s64 block_size, error *err)
{
    if (seek_block_from_start(stream, nth_block, block_size, err) == -1)
        return -1;

    return write(stream, in, block_size, err);
}

s64 write_blocks(file_stream *stream, const void *in, s64 block_count, s64 block_size, error *err)
{
    return write(stream, in, block_size, block_count, err);
}

s64 write_blocks(file_stream *stream, const void *in, s64 nth_block, s64 block_count, s64 block_size, error *err)
{
    if (seek_block_from_start(stream, nth_block, block_size, err) == -1)
        return -1;

    return write(stream, in, block_size, block_count, err);
}
