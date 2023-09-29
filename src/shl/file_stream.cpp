
// v1.1
// add seek_next_alignment

#include <assert.h>
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

bool open(file_stream *stream, const char *path, const char *mode, bool check_open, bool calc_size)
{
    assert(stream != nullptr);

    if (check_open && !close(stream))
        return false;

    assert(path != nullptr);
    assert(mode != nullptr);

#if Windows
    errno_t err = fopen_s(&stream->handle, path, mode);

    if (err != 0)
        return false;
#else
    stream->handle = fopen(path, mode);
#endif

    if (stream->handle == nullptr)
        return false;

    if (calc_size)
        calculate_file_size(stream);

    return true;
}

bool close(file_stream *stream)
{
    assert(stream != nullptr);

    if (stream->handle == nullptr)
        return true;

    bool ret = fclose(stream->handle) == 0;

    if (!ret)
        return ret;

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

u64 calculate_file_size(file_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    u64 curpos = tell(stream);
    seek(stream, 0, SEEK_END);
    u64 sz = tell(stream);
    seek(stream, curpos);

    stream->size = sz;
    return sz;
}

u64 block_count(const file_stream *stream)
{
    assert(stream != nullptr);
    return stream->size / stream->block_size;
}

int seek(file_stream *stream, s64 offset, int whence)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    return fseeko(stream->handle, offset, whence);
}

int seek_block(file_stream *stream, s64 nth_block, int whence)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    u64 bs = stream->block_size;
    u64 block_pos = nth_block * bs;

    if (whence != SEEK_CUR)
        return fseeko(stream->handle, block_pos, whence);

    // SEEK_CUR
    u64 cur = (ftello(stream->handle) / bs) * bs;
    return fseeko(stream->handle, cur + block_pos, SEEK_SET);
}

int seek_next_alignment(file_stream *stream, u64 alignment)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(alignment > 0);

    u64 npos = tell(stream);
    npos = (npos + alignment - 1) / alignment * alignment;

    return fseeko(stream->handle, npos, SEEK_SET);
}

u64 tell(file_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);

    return ftello(stream->handle);
}

bool getpos(file_stream *stream, fpos_t *pos)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(pos != nullptr);
    
    return fgetpos(stream->handle, pos) == 0;
}

bool rewind(file_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    
    return fseeko(stream->handle, 0L, SEEK_SET) == 0;
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
    
    seek_block(stream, nth_block, SEEK_SET);
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

    seek_block(stream, nth_block, SEEK_SET);
    return fread(out, stream->block_size, block_count, stream->handle);
}

u64 read_entire_file(file_stream *stream, void *out, u64 max_size)
{
    assert(stream != nullptr);
    assert(stream->handle != nullptr);
    assert(out != nullptr);
    
    u64 old_pos = ftello(stream->handle);
    fseeko(stream->handle, 0, SEEK_SET);

    u64 sz = stream->size;

    if (sz > max_size)
        sz = max_size;

    u64 ret = fread(out, 1, sz, stream->handle);
    fseeko(stream->handle, old_pos, SEEK_SET);

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

int flush(file_stream *stream)
{
    return fflush(stream->handle);
}
