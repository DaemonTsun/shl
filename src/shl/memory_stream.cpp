
// v1.2
// add seek_next_alignment
// fix string writing

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shl/memory_stream.hpp"

memory_stream::operator char*() const
{
    return current(this);
}

void init(memory_stream *stream)
{
    assert(stream != nullptr);

    stream->data = nullptr;
    stream->size = 0;
    stream->position = 0;
}

bool open(memory_stream *stream, u64 size, bool check_open_and_close, bool free_on_close, error *err)
{
    assert(stream != nullptr);
    
    if (check_open_and_close && !close(stream, free_on_close))
        return false;

    stream->data = reinterpret_cast<char*>(malloc(size));

    if (stream->data == nullptr)
    {
        format_error(err, -1, "could not open memory stream %p (data %p): %s", stream, stream->data, strerror(errno));
        return false;
    }

    stream->size = size;
    stream->position = 0;

    return true;
}

bool open(memory_stream *stream, char *in, u64 size, bool check_open_and_close, bool free_on_close, error *err)
{
    assert(stream != nullptr);
    assert(in != nullptr);
    
    if (check_open_and_close && !close(stream, free_on_close))
    {
        format_error(err, -1, "could not close memory stream %p (data %p)", stream, stream->data);
        return false;
    }

    stream->data = in;

    if (stream->data == nullptr)
        return false;

    stream->size = size;
    stream->position = 0;

    return true;
}

bool close(memory_stream *stream, bool free_)
{
    assert(stream != nullptr);

    if (stream->data == nullptr)
        return true;

    if (free_)
        free(stream->data);

    stream->data = nullptr;

    return true;
}

bool is_open(const memory_stream *stream)
{
    assert(stream != nullptr);

    return stream->data != nullptr;
}

bool is_at_end(const memory_stream *stream)
{
    assert(stream != nullptr);

    return stream->position >= stream->size;
}

bool is_ok(const memory_stream *stream)
{
    assert(stream != nullptr);

    return (stream->data != nullptr)
        && (stream->position < stream->size);
}

u64 block_count(const memory_stream *stream, u64 block_size)
{
    assert(stream != nullptr);
    return stream->size / block_size;
}

char *current(const memory_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    
    return stream->data + stream->position;
}

char *current_block_start(const memory_stream *stream, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(block_size > 0);

    return stream->data + ((stream->position / block_size) * block_size);
}

u64 current_block_number(const memory_stream *stream, u64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return stream->position / block_size;
}

u64 current_block_offset(const memory_stream *stream, u64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return (stream->position / block_size) * block_size;
}

void seek(memory_stream *stream, u64 pos)
{
    assert(stream != nullptr);

    stream->position = pos;

    if (stream->position > stream->size)
        stream->position = stream->size;
}

int seek(memory_stream *stream, s64 offset, int whence)
{
    assert(stream != nullptr);

    if (whence == SEEK_SET)
        stream->position = offset;
    else if (whence == SEEK_CUR)
        stream->position = stream->position + offset;
    else if (whence == SEEK_END)
        stream->position = stream->size + offset;
    else
        return -1;

    if (stream->position > stream->size)
        stream->position = stream->size;

    return 0;
}

void seek_offset(memory_stream *stream, s64 offset)
{
    assert(stream != nullptr);

    stream->position = stream->position + offset;

    if (stream->position > stream->size)
        stream->position = stream->size;
}

void seek_from_end(memory_stream *stream, s64 offset)
{
    assert(stream != nullptr);

    stream->position = stream->size + offset;

    if (stream->position > stream->size)
        stream->position = stream->size;
}

void seek_block(memory_stream *stream, u64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    
    seek(stream, nth_block * block_size);
}

int seek_block(memory_stream *stream, s64 nth_block, u64 block_size, int whence)
{
    assert(stream != nullptr);

    if (whence != SEEK_CUR)
        return seek(stream, nth_block * block_size, whence);

    // SEEK_CUR
    u64 cur = (stream->position / block_size) * block_size;
    return seek(stream, cur + nth_block * block_size, SEEK_SET);
}

void seek_block_offset(memory_stream *stream, s64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    
    u64 cur = (stream->position / block_size) * block_size;
    seek(stream, cur + nth_block * block_size);
}

void seek_block_from_end(memory_stream *stream, s64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    
    seek_from_end(stream, nth_block * block_size);
}

void seek_next_alignment(memory_stream *stream, u64 alignment)
{
    assert(stream != nullptr);
    assert(alignment > 0);

    u64 npos = stream->position;
    npos = (npos + alignment - 1) / alignment * alignment;

    seek(stream, npos);
}

u64 tell(memory_stream *stream)
{
    assert(stream != nullptr);
    return stream->position;
}

void rewind(memory_stream *stream)
{
    assert(stream != nullptr);
    stream->position = 0;
}

u64 read(memory_stream *stream, void *out, u64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    u64 read_size = size;
    u64 rest = stream->size - stream->position;

    if (read_size > rest)
        read_size = rest;

    if (read_size > 0)
    {
        memcpy(out, stream->data + stream->position, read_size);
        stream->position += read_size;
    }

    return read_size;
}

u64 read(memory_stream *stream, void *out, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    u64 read_size = size * nmemb;
    u64 rest = stream->size - stream->position;

    if (read_size > rest)
        read_size = (rest / size) * size;

    if (read_size > 0)
    {
        memcpy(out, stream->data + stream->position, read_size);
        stream->position += read_size;
    }

    return read_size / size;
}

u64 read_at(memory_stream *stream, void *out, u64 offset, u64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    
    seek(stream, offset, SEEK_SET);
    return read(stream, out, size);
}

u64 read_at(memory_stream *stream, void *out, u64 offset, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    seek(stream, offset, SEEK_SET);
    return read(stream, out, size, nmemb);
}

u64 read_block(memory_stream *stream, void *out, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    return read(stream, out, block_size);
}

u64 read_block(memory_stream *stream, void *out, u64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    seek_block(stream, nth_block, block_size);
    return read(stream, out, block_size);
}

u64 read_blocks(memory_stream *stream, void *out, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    return read(stream, out, block_size, block_count);
}

u64 read_blocks(memory_stream *stream, void *out, u64 nth_block, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    seek_block(stream, nth_block, block_size);
    return read(stream, out, block_size, block_count);
}

u64 copy_entire_stream(memory_stream *stream, void *out, u64 max_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (max_size > stream->size)
        max_size = stream->size;

    if (max_size > 0)
        memcpy(out, stream->data, max_size);

    return max_size;
}

u64 write(memory_stream *stream, const void *in, u64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);
    assert(stream->position < stream->size);

    u64 write_size = size;
    u64 rest = stream->size - stream->position;

    if (write_size > rest)
        write_size = rest;

    if (write_size > 0)
    {
        memcpy(stream->data + stream->position, in, write_size);
        stream->position += write_size;
    }

    return write_size;
}

u64 write(memory_stream *stream, const void *in, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);
    assert(stream->position < stream->size);

    u64 write_size = size * nmemb;
    u64 rest = stream->size - stream->position;

    if (write_size > rest)
        write_size = (rest / size) * size;

    if (write_size > 0)
    {
        memcpy(stream->data + stream->position, in, write_size);
        stream->position += write_size;
    }

    return write_size / size;
}

u64 write(memory_stream *stream, const char *str)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(str != nullptr);
    assert(stream->position < stream->size);
    
    u64 write_size = strlen(str);
    return write(stream, str, write_size);
}

u64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek(stream, offset, SEEK_SET);
    return write(stream, in, size);
}

u64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek(stream, offset, SEEK_SET);
    return write(stream, in, size, nmemb);
}

u64 write_at(memory_stream *stream, const char *str, u64 offset)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(str != nullptr);

    u64 write_size = strlen(str);
    return write_at(stream, str, write_size, offset);
}

u64 write_block(memory_stream *stream, const void *in, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);
    assert(stream->position < stream->size);

    return write(stream, in, block_size);
}

u64 write_block(memory_stream *stream, const void *in, u64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_block(stream, nth_block, block_size);
    return write(stream, in, block_size);
}

u64 write_blocks(memory_stream *stream, const void *in, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);
    assert(stream->position < stream->size);

    return write(stream, in, block_size, block_count);
}

u64 write_blocks(memory_stream *stream, const void *in, u64 nth_block, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_block(stream, nth_block, block_size);
    return write(stream, in, block_size, block_count);
}

hash_t hash(const memory_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);

    return hash_data(stream->data, stream->size);
}
