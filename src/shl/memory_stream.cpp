
// v1.3
// redesign API again
// v1.2
// add seek_next_alignment
// fix string writing

#include <assert.h>

#include "shl/memory.hpp"
#include "shl/memory_stream.hpp"

memory_stream::operator char*() const
{
    return current(this);
}

void init(memory_stream *stream, u64 size)
{
    assert(stream != nullptr);

    stream->data = (char*)allocate_memory(size);
    stream->size = size;
    stream->position = 0;
}

void free(memory_stream *stream)
{
    if (stream == nullptr)
        return;

    if (stream->data != nullptr)
        free_memory(stream->data);

    stream->data = nullptr;
    stream->size = 0;
    stream->position = 0;
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

s64 block_count(const memory_stream *stream, u64 block_size)
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

s64 current_block_number(const memory_stream *stream, u64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return (s64)(stream->position / block_size);
}

s64 current_block_offset(const memory_stream *stream, u64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return (s64)((stream->position / block_size) * block_size);
}

s64 seek(memory_stream *stream, s64 offset, int whence)
{
    assert(stream != nullptr);

    if (whence == IO_SEEK_SET)
        stream->position = offset;
    else if (whence == IO_SEEK_CUR)
        stream->position = stream->position + offset;
    else if (whence == IO_SEEK_END)
        stream->position = stream->size + offset;
    else
        return -1;

    if (stream->position > stream->size)
        stream->position = stream->size;

    return (s64)stream->position;
}

s64 seek_offset(memory_stream *stream, s64 offset)
{
    return seek(stream, offset, IO_SEEK_CUR);
}

s64 seek_from_start(memory_stream *stream, s64 offset)
{
    return seek(stream, offset, IO_SEEK_SET);
}

s64 seek_from_end(memory_stream *stream, s64 offset)
{
    return seek(stream, offset, IO_SEEK_END);
}

s64 seek_block(memory_stream *stream, s64 nth_block, u64 block_size, int whence)
{
    assert(stream != nullptr);
    
    if (whence != IO_SEEK_CUR)
        return seek(stream, nth_block * block_size, whence);

    return seek_block_offset(stream, nth_block, block_size);
}

s64 seek_block_offset(memory_stream *stream, s64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    
    s64 cur = current_block_offset(stream, block_size);
    return seek_from_start(stream, cur + nth_block * block_size);
}

s64 seek_block_from_start(memory_stream *stream, s64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    
    return seek_from_start(stream, nth_block * block_size);
}

s64 seek_block_from_end(memory_stream *stream, s64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    
    return seek_from_end(stream, nth_block * block_size);
}

s64 seek_next_alignment(memory_stream *stream, u64 alignment)
{
    assert(stream != nullptr);
    assert(alignment > 0);

    u64 npos = stream->position;
    npos = (npos + alignment - 1) / alignment * alignment;

    return seek(stream, npos);
}

s64 tell(memory_stream *stream)
{
    assert(stream != nullptr);
    return stream->position;
}

void rewind(memory_stream *stream)
{
    assert(stream != nullptr);
    stream->position = 0;
}

s64 read(memory_stream *stream, void *out, u64 size)
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
        copy_memory(stream->data + stream->position, out, read_size);
        stream->position += read_size;
    }

    return read_size;
}

s64 read(memory_stream *stream, void *out, u64 size, u64 nmemb)
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
        copy_memory(stream->data + stream->position, out, read_size);
        stream->position += read_size;
    }

    return read_size / size;
}

s64 read_at(memory_stream *stream, void *out, u64 offset, u64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    
    seek_from_start(stream, offset);
    return read(stream, out, size);
}

s64 read_at(memory_stream *stream, void *out, u64 offset, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    seek_from_start(stream, offset);
    return read(stream, out, size, nmemb);
}

s64 read_block(memory_stream *stream, void *out, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    return read(stream, out, block_size);
}

s64 read_block(memory_stream *stream, void *out, u64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    seek_block_from_start(stream, nth_block, block_size);
    return read(stream, out, block_size);
}

s64 read_blocks(memory_stream *stream, void *out, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    return read(stream, out, block_size, block_count);
}

s64 read_blocks(memory_stream *stream, void *out, u64 nth_block, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    assert(stream->position < stream->size);

    seek_block_from_start(stream, nth_block, block_size);
    return read(stream, out, block_size, block_count);
}

s64 copy_entire_stream(memory_stream *stream, void *out, u64 max_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (max_size > stream->size)
        max_size = stream->size;

    if (max_size > 0)
        copy_memory(stream->data, out, max_size);

    return (s64)max_size;
}

s64 write(memory_stream *stream, const void *in, u64 size)
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
        copy_memory(in, stream->data + stream->position, write_size);
        stream->position += write_size;
    }

    return (s64)write_size;
}

s64 write(memory_stream *stream, const void *in, u64 size, u64 nmemb)
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
        copy_memory(in, stream->data + stream->position, write_size);
        stream->position += write_size;
    }

    return (s64)(write_size / size);
}

s64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_from_start(stream, offset);
    return write(stream, in, size);
}

s64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_from_start(stream, offset);
    return write(stream, in, size, nmemb);
}

s64 write_block(memory_stream *stream, const void *in, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);
    assert(stream->position < stream->size);

    return write(stream, in, block_size);
}

s64 write_block(memory_stream *stream, const void *in, u64 nth_block, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_block_from_start(stream, nth_block, block_size);
    return write(stream, in, block_size);
}

s64 write_blocks(memory_stream *stream, const void *in, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);
    assert(stream->position < stream->size);

    return write(stream, in, block_size, block_count);
}

s64 write_blocks(memory_stream *stream, const void *in, u64 nth_block, u64 block_count, u64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_block_from_start(stream, nth_block, block_size);
    return write(stream, in, block_size, block_count);
}

hash_t hash(const memory_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);

    return hash_data(stream->data, stream->size);
}
