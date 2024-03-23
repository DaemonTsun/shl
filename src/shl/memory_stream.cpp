
// v1.3
// redesign API again
// v1.2
// add seek_next_alignment
// fix string writing

#include "shl/assert.hpp"
#include "shl/bits.hpp"
#include "shl/math.hpp"
#include "shl/program_context.hpp"
#include "shl/memory.hpp"
#include "shl/memory_stream.hpp"

void init(memory_stream *stream, s64 size)
{
    assert(stream != nullptr);

    stream->allocator = get_context_pointer()->allocator;
    stream->data = (char*)allocator_alloc(stream->allocator, size);
    stream->size = size;
    stream->position = 0;
}

void free(memory_stream *stream)
{
    if (stream == nullptr)
        return;

    if (stream->allocator.alloc == nullptr)
        stream->allocator = get_context_pointer()->allocator;

    if (stream->data != nullptr)
        allocator_dealloc(stream->allocator, stream->data, stream->size);

    stream->data = nullptr;
    stream->size = 0;
    stream->position = 0;
}

bool is_open(memory_stream *stream)
{
    assert(stream != nullptr);

    return stream->data != nullptr;
}

bool is_at_end(memory_stream *stream)
{
    assert(stream != nullptr);

    return stream->position >= stream->size;
}

bool is_ok(memory_stream *stream)
{
    assert(stream != nullptr);

    return (stream->data != nullptr)
        && (stream->position < stream->size);
}

s64 block_count(memory_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    return stream->size / block_size;
}

char *current(memory_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    
    return stream->data + stream->position;
}

char *current_block_start(memory_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(block_size > 0);

    return stream->data + floor_multiple(stream->position, block_size);
}

char *current_block_start2(memory_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(block_size > 0);

    return stream->data + floor_multiple2(stream->position, block_size);
}

s64 current_block_number(memory_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return (s64)(stream->position / block_size);
}

s64 current_block_offset(memory_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return (s64)floor_multiple(stream->position, block_size);
}

s64 current_block_offset2(memory_stream *stream, s64 block_size)
{
    assert(stream != nullptr);
    assert(block_size > 0);

    return (s64)floor_multiple2(stream->position, block_size);
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

s64 seek_block(memory_stream *stream, s64 nth_block, s64 block_size, int whence)
{
    assert(stream != nullptr);
    
    if (whence != IO_SEEK_CUR)
        return seek(stream, nth_block * block_size, whence);

    return seek_block_offset(stream, nth_block, block_size);
}

s64 seek_block_offset(memory_stream *stream, s64 nth_block, s64 block_size)
{
    assert(stream != nullptr);
    
    s64 cur = 0;

    if (is_pow2(block_size))
        cur = current_block_offset2(stream, block_size);
    else
        cur = current_block_offset(stream, block_size);

    return seek_from_start(stream, cur + nth_block * block_size);
}

s64 seek_block_from_start(memory_stream *stream, s64 nth_block, s64 block_size)
{
    assert(stream != nullptr);
    
    return seek_from_start(stream, nth_block * block_size);
}

s64 seek_block_from_end(memory_stream *stream, s64 nth_block, s64 block_size)
{
    assert(stream != nullptr);
    
    return seek_from_end(stream, nth_block * block_size);
}

s64 seek_next_alignment(memory_stream *stream, s64 alignment)
{
    assert(stream != nullptr);
    assert(alignment > 0);

    s64 npos = stream->position;

    if (is_pow2(alignment))
        npos = ceil_multiple2(npos + 1, alignment);
    else
        npos = ceil_multiple(npos +1, alignment);

    return seek(stream, npos);
}

s64 seek_next_alignment2(memory_stream *stream, s64 alignment)
{
    assert(stream != nullptr);
    assert(alignment > 0);

    s64 npos = stream->position;

    npos = ceil_multiple2(npos + 1, alignment);

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

s64 read(memory_stream *stream, void *out, s64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (stream->position >= stream->size)
        return 0;

    s64 read_size = size;
    s64 rest = stream->size - stream->position;

    if (read_size > rest)
        read_size = rest;

    if (read_size > 0)
    {
        copy_memory(stream->data + stream->position, out, read_size);
        stream->position += read_size;
    }

    return read_size;
}

s64 read(memory_stream *stream, void *out, s64 size, s64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (stream->position >= stream->size)
        return 0;

    s64 read_size = size * nmemb;
    s64 rest = stream->size - stream->position;

    if (read_size > rest)
        read_size = (rest / size) * size;

    if (read_size > 0)
    {
        copy_memory(stream->data + stream->position, out, read_size);
        stream->position += read_size;
    }

    return read_size / size;
}

s64 read_at(memory_stream *stream, void *out, s64 offset, s64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);
    
    seek_from_start(stream, offset);
    return read(stream, out, size);
}

s64 read_at(memory_stream *stream, void *out, s64 offset, s64 size, s64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    seek_from_start(stream, offset);
    return read(stream, out, size, nmemb);
}

s64 read_block(memory_stream *stream, void *out, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (stream->position >= stream->size)
        return 0;

    s64 ret = read(stream, out, block_size);

    if (ret == -1)
        return -1;

    if (ret < block_size)
        return 0;

    return ret;
}

s64 read_block(memory_stream *stream, void *out, s64 nth_block, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    seek_block_from_start(stream, nth_block, block_size);
    return read_block(stream, out, block_size);
}

s64 read_blocks(memory_stream *stream, void *out, s64 block_count, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (stream->position >= stream->size)
        return 0;

    return read(stream, out, block_size, block_count);
}

s64 read_blocks(memory_stream *stream, void *out, s64 nth_block, s64 block_count, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(out != nullptr);

    if (stream->position >= stream->size)
        return 0;

    seek_block_from_start(stream, nth_block, block_size);
    return read(stream, out, block_size, block_count);
}

s64 copy_entire_stream(memory_stream *stream, void *out, s64 max_size)
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

s64 write(memory_stream *stream, const void *in, s64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    if (stream->position >= stream->size)
        return 0;

    s64 write_size = size;
    s64 rest = stream->size - stream->position;

    if (write_size > rest)
        write_size = rest;

    if (write_size > 0)
    {
        copy_memory(in, stream->data + stream->position, write_size);
        stream->position += write_size;
    }

    return (s64)write_size;
}

s64 write(memory_stream *stream, const void *in, s64 size, s64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    if (stream->position >= stream->size)
        return 0;

    s64 write_size = size * nmemb;
    s64 rest = stream->size - stream->position;

    if (write_size > rest)
        write_size = (rest / size) * size;

    if (write_size > 0)
    {
        copy_memory(in, stream->data + stream->position, write_size);
        stream->position += write_size;
    }

    return (s64)(write_size / size);
}

s64 write_at(memory_stream *stream, const void *in, s64 offset, s64 size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_from_start(stream, offset);
    return write(stream, in, size);
}

s64 write_at(memory_stream *stream, const void *in, s64 offset, s64 size, s64 nmemb)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_from_start(stream, offset);
    return write(stream, in, size, nmemb);
}

s64 write_block(memory_stream *stream, const void *in, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    if (stream->position + block_size > stream->size)
        return 0;

    return write(stream, in, block_size);
}

s64 write_block(memory_stream *stream, const void *in, s64 nth_block, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_block_from_start(stream, nth_block, block_size);
    return write(stream, in, block_size);
}

s64 write_blocks(memory_stream *stream, const void *in, s64 block_count, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    if (stream->position >= stream->size)
        return 0;

    return write(stream, in, block_size, block_count);
}

s64 write_blocks(memory_stream *stream, const void *in, s64 nth_block, s64 block_count, s64 block_size)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);
    assert(in != nullptr);

    seek_block_from_start(stream, nth_block, block_size);
    return write(stream, in, block_size, block_count);
}

hash_t hash(memory_stream *stream)
{
    assert(stream != nullptr);
    assert(stream->data != nullptr);

    return hash_data(stream->data, stream->size);
}
