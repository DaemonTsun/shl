
// v1.2
// add error handling
// v1.1
// add seek_next_alignment

#include <string.h>
#include <errno.h>
#include <stdlib.h> // wcstombs

#include "shl/assert.hpp"
#include "shl/platform.hpp"
#include "shl/memory.hpp"
#include "shl/bits.hpp"
#include "shl/math.hpp"

#if Windows
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h> // close
#endif

#include "shl/file_stream.hpp"

template<typename C>
s64 _string_len(const C *str)
{
    s64 ret = 0;

    while (*str++ != '\0')
        ret++;

    return ret;
}

bool init(file_stream *stream, const char *path, error *err)
{
    return init(stream, path, MODE_READ | MODE_WRITE, PERMISSION_READ | PERMISSION_WRITE, err);
}

bool init(file_stream *stream, const wchar_t *path, error *err)
{
    return init(stream, path, MODE_READ | MODE_WRITE, PERMISSION_READ | PERMISSION_WRITE, err);
}

bool init(file_stream *stream, const char *path, int mode, error *err)
{
    return init(stream, path, mode, PERMISSION_READ | PERMISSION_WRITE, err);
}

bool init(file_stream *stream, const wchar_t *path, int mode, error *err)
{
    return init(stream, path, mode, PERMISSION_READ | PERMISSION_WRITE, err);
}

bool init(file_stream *stream, const char *path, int mode, int permissions, error *err)
{
    assert(stream != nullptr);

#if Windows
    s64 char_count = _string_len(path);
    s64 sz = (char_count + 1) * sizeof(wchar_t);
    wchar_t *tmp = (wchar_t*)::allocate_memory(sz);

    ::fill_memory((void*)tmp, 0, sz);
    ::mbstowcs(tmp, path, char_count * sizeof(char));
    
    bool ok = init(stream, tmp, mode, permissions, err);

    free_memory(tmp);

    return ok;
#else
    stream->handle = -1;

    int _flags = 0;
    int _mode = 0;
    bool _rd = mode & MODE_READ;
    bool _wr = (mode & MODE_WRITE) || (mode & MODE_WRITE_TRUNC);

    if (_wr)
        _flags |= O_CREAT;

    if (mode & MODE_WRITE_TRUNC)
        _flags |= O_TRUNC;

    if (_rd && _wr) _flags |= O_RDWR;
    else if (_rd)   _flags |= O_RDONLY;
    else if (_wr)   _flags |= O_WRONLY;

    if (permissions & PERMISSION_READ)    _mode |= 0400;
    if (permissions & PERMISSION_WRITE)   _mode |= 0200;
    if (permissions & PERMISSION_EXECUTE) _mode |= 0100;

    int fd = open(path, _flags, _mode);

    if (fd == -1)
    {
        set_errno_error(err);
        return false;
    }

    stream->handle = fd;
    
    if (get_file_size(stream, err) < 0)
        return false;

    return true;
#endif
}

bool init(file_stream *stream, const wchar_t *path, int mode, int permissions, error *err)
{
    assert(stream != nullptr);

#if Windows
    int _access = 0;
    int _share = 0;
    int _creation = 0;
    bool _rd = mode & MODE_READ;
    bool _wr = (mode & MODE_WRITE) || (mode & MODE_WRITE_TRUNC);

    if (permissions & PERMISSION_READ)    _access |= GENERIC_READ;
    if (permissions & PERMISSION_WRITE)   _access |= GENERIC_WRITE;
    if (permissions & PERMISSION_EXECUTE) _access |= GENERIC_EXECUTE;

    if (_rd && _wr)
    {
        _access |= GENERIC_READ | GENERIC_WRITE;
        _share = 0;
    }
    else if (_rd)
    {
        _access = GENERIC_READ;
        _share = FILE_SHARE_READ;
        _creation = OPEN_EXISTING;
    }
    else if (_wr)
    {
        _access = GENERIC_WRITE;
        _share = 0;
    }

    if (mode & MODE_WRITE)
    {
        _creation = OPEN_ALWAYS;
    }
    else if (mode & MODE_WRITE_TRUNC)
    {
        _creation = CREATE_ALWAYS;
    }

    io_handle h = CreateFile(path,
                             _access,
                             _share,
                             nullptr,
                             _creation,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);

    if (h == nullptr || h == INVALID_HANDLE_VALUE)
    {
        set_GetLastError_error(err);
        return false;
    }

    stream->handle = h;
    
    if (get_file_size(stream, err) < 0)
        return false;

    return true;
#else
    s64 char_count = _string_len(path);
    s64 sz = (char_count + 1) * sizeof(char);
    char *tmp = (char*)::allocate_memory(sz);

    ::fill_memory((void*)tmp, 0, sz);
    ::wcstombs(tmp, path, char_count * sizeof(wchar_t));
    
    bool ok = init(stream, tmp, mode, permissions, err);

    free_memory(tmp);

    return ok;
#endif
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
    if (stream->handle != -1 && close(stream->handle) == -1)
    {
        set_errno_error(err);
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
