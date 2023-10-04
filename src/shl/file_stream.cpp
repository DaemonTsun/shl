
// v1.2
// add error handling
// v1.1
// add seek_next_alignment

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "shl/file_stream.hpp"
#include "shl/platform.hpp"

#if Windows
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

file_stream::operator FILE*() const
{
    return this->handle;
}

void init(file_stream *stream)
{
    assert(stream != nullptr);

    stream->handle = nullptr;
    stream->size = 0;
    stream->block_size = 1;
}

bool open(file_stream *stream, const char *path, const char *mode, bool check_open_and_close, bool calc_size, error *err)
{
    assert(stream != nullptr);

    if (check_open_and_close && !close(stream, err))
        return false;

    assert(path != nullptr);
    assert(mode != nullptr);

#if Windows
    errno_t errcode = fopen_s(&stream->handle, path, mode);

    if (err != 0)
    {
        get_error(err, "could not open file '%s' (stream %p): %s", path, stream, strerror(errcode));
        return false;
    }
#else
    stream->handle = fopen(path, mode);
#endif

    if (stream->handle == nullptr)
    {
        get_error(err, "could not open file '%s' (stream %p): %s", path, stream, strerror(errno));
        return false;
    }

    if (calc_size)
        calculate_file_size(stream, err);

    return true;
}

bool close(file_stream *stream, error *err)
{
    assert(stream != nullptr);

    if (stream->handle == nullptr)
        return true;

    bool ret = fclose(stream->handle) == 0;

    if (!ret)
    {
        get_error(err, "could not close file stream %p (FILE %p): %s", stream, stream->handle, strerror(errno));
        return ret;
    }

    stream->handle = nullptr;

    return ret;
}

bool is_open(const file_stream *stream)
{
    assert(stream != nullptr);

    return stream->handle != nullptr;
}

bool is_at_end(file_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    return feof(stream->handle) != 0;
}

bool has_error(file_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    return ferror(stream->handle) != 0;
}

void clear_error(file_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    clearerr(stream->handle);
}

bool is_ok(file_stream *stream)
{
    assert(stream != nullptr);

    return (stream->handle != nullptr)
        && (feof(stream->handle) == 0)
        && (ferror(stream->handle) == 0);
}

s64 calculate_file_size(file_stream *stream, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    s64 curpos = tell(stream, err);

    if (curpos < 0)
        return -1;

    if (seek(stream, 0, SEEK_END, err) < 0)
        return -1;

    s64 sz = tell(stream, err);

    if (sz < 0)
        return sz;

    // if this one fails we're just screwed
    seek(stream, curpos);

    stream->size = (u64)sz;
    return sz;
}

u64 block_count(const file_stream *stream)
{
    assert(stream != nullptr);
    return stream->size / stream->block_size;
}

int seek(file_stream *stream, s64 offset, int whence, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    int ret = fseeko(stream->handle, offset, whence);

    if (ret < 0)
        get_error(err, "could not seek file stream %p (FILE %p): %s", stream, stream->handle, strerror(errno));

    return ret;
}

int seek_block(file_stream *stream, s64 nth_block, int whence, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    u64 bs = stream->block_size;
    u64 block_pos = nth_block * bs;

    if (whence != SEEK_CUR)
        return seek(stream, block_pos, whence, err);

    // SEEK_CUR
    s64 cur = tell(stream, err);

    if (cur < 0)
        return -1;

    s64 cur_block = (cur / bs) * bs;

    return seek(stream, cur_block + block_pos, SEEK_SET, err);
}

int seek_next_alignment(file_stream *stream, u64 alignment, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(alignment > 0);

    s64 npos = tell(stream, err);

    if (npos < 0)
        return -1;

    npos = (npos + alignment - 1) / alignment * alignment;

    return seek(stream, npos, SEEK_SET, err);
}

s64 tell(file_stream *stream, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    s64 ret = ftello(stream->handle);

    if (ret < 0)
    {
        get_error(err, "could not seek file stream %p (FILE %p): %s", stream, stream->handle, strerror(errno));
        return -1;
    }

    return ret;
}

bool getpos(file_stream *stream, fpos_t *pos)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(pos != nullptr);
    
    return fgetpos(stream->handle, pos) == 0;
}

bool rewind(file_stream *stream, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    
    return seek(stream, 0L, SEEK_SET, err) == 0;
}

u64 read(file_stream *stream, void *out, u64 size)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);
    
    return fread(out, 1, size, stream->handle);
}

u64 read(file_stream *stream, void *out, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);
    
    return fread(out, size, nmemb, stream->handle);
}

u64 read_at(file_stream *stream, void *out, u64 offset, u64 size)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);
    
    if (fseeko(stream->handle, offset, SEEK_SET) != 0)
        return 0;

    return fread(out, 1, size, stream->handle);
}

u64 read_at(file_stream *stream, void *out, u64 offset, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);
    
    if (fseeko(stream->handle, offset, SEEK_SET) != 0)
        return 0;

    return fread(out, size, nmemb, stream->handle);
}

u64 read_block(file_stream *stream, void *out)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);

    return fread(out, 1, stream->block_size, stream->handle);
}

u64 read_block(file_stream *stream, void *out, u64 nth_block)
{
    assert(out != nullptr);
    
    if (seek_block(stream, nth_block, SEEK_SET) < 0)
        return 0;

    return fread(out, 1, stream->block_size, stream->handle);
}

u64 read_blocks(file_stream *stream, void *out, u64 block_count)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);

    return fread(out, stream->block_size, block_count, stream->handle);
}

u64 read_blocks(file_stream *stream, void *out, u64 nth_block, u64 block_count)
{
    assert(out != nullptr);

    if (seek_block(stream, nth_block, SEEK_SET) < 0)
        return 0;

    return fread(out, stream->block_size, block_count, stream->handle);
}

u64 read_entire_file(file_stream *stream, void *out, u64 max_size, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);

    u64 sz = stream->size;

    if (sz > max_size)
        sz = max_size;

    if (sz == 0)
    {
        err->what = nullptr;
        return 0;
    }
    
    s64 old_pos = tell(stream, err);

    if (old_pos < 0)
        return 0;

    if (seek(stream, 0, SEEK_SET, err) < 0) 
        return 0;

    u64 ret = fread(out, 1, sz, stream->handle);

    if (seek(stream, old_pos, SEEK_SET, err))
        return 0;

    return ret;
}

u64 write(file_stream *stream, const void *in, u64 size)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(in != nullptr);

    return fwrite(in, 1, size, stream->handle);
}

u64 write(file_stream *stream, const void *in, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(in != nullptr);

    return fwrite(in, size, nmemb, stream->handle);
}

u64 write(file_stream *stream, const char *str)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(str != nullptr);

    return fputs(str, stream->handle);
}

u64 write_at(file_stream *stream, const void *in, u64 offset, u64 size)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(in != nullptr);

    if (fseeko(stream->handle, offset, SEEK_SET) != 0)
        return 0;

    return fwrite(in, 1, size, stream->handle);
}

u64 write_at(file_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(in != nullptr);

    if (fseeko(stream->handle, offset, SEEK_SET) != 0)
        return 0;

    return fwrite(in, size, nmemb, stream->handle);
}

u64 write_at(file_stream *stream, const char *str, u64 offset)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(str != nullptr);

    if (fseeko(stream->handle, offset, SEEK_SET) != 0)
        return 0;

    return fputs(str, stream->handle);
}

u64 write_block(file_stream *stream, const void *in)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(in != nullptr);

    return fwrite(in, 1, stream->block_size, stream->handle);
}

u64 write_block(file_stream *stream, const void *in, u64 nth_block)
{
    assert(in != nullptr);

    seek_block(stream, nth_block, SEEK_SET);
    return fwrite(in, 1, stream->block_size, stream->handle);
}

u64 write_blocks(file_stream *stream, const void *in, u64 block_count)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(in != nullptr);

    return fwrite(in, stream->block_size, block_count, stream->handle);
}

u64 write_blocks(file_stream *stream, const void *in, u64 nth_block, u64 block_count)
{
    assert(in != nullptr);

    seek_block(stream, nth_block, SEEK_SET);
    return fwrite(in, stream->block_size, block_count, stream->handle);
}

int format(FILE *stream, const char *format_string, ...)
{
    va_list arg;
    int ret;

    va_start(arg, format_string);
    ret = vfprintf(stream, format_string, arg);
    va_end(arg);

    return ret;
}

int format(file_stream *stream, const char *format_string, ...)
{
    va_list arg;
    int ret;

    va_start(arg, format_string);
    ret = vfprintf(stream->handle, format_string, arg);
    va_end(arg);

    return ret;
}

int flush(file_stream *stream, error *err)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    int ret = fflush(stream->handle);
    
    if (ret != 0)
    {
        get_error(err, "could not flush file stream %p (FILE %p): %s", stream, stream->handle, strerror(errno));
        return -1;
    }

    return ret;
}
