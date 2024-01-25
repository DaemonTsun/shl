#pragma once

/* memory_stream
v1.3
redesign API again
v1.2
add seek_next_alignment

defines the memory_stream struct and its functions.

TODO: docs
*/

#include "shl/io.hpp" // IO_SEEK_...
#include "shl/hash.hpp"
#include "shl/number_types.hpp"

struct memory_stream
{
    char *data;
    u64 size;
    u64 position;

    explicit operator char*() const;
};

void init(memory_stream *stream, u64 size);
void free(memory_stream *stream);

bool is_open(const memory_stream *stream);
bool is_at_end(const memory_stream *stream);

// is_open && !is_at_end
bool is_ok(const memory_stream *stream);

s64 block_count(const memory_stream *stream, u64 block_size);
// returns a pointer in the data at the current position
char *current(const memory_stream *stream);
char *current_block_start(const memory_stream *stream, u64 block_size);
s64 current_block_number(const memory_stream *stream, u64 block_size);
s64 current_block_offset(const memory_stream *stream, u64 block_size);

s64 seek(memory_stream *stream, s64 offset, int whence = IO_SEEK_SET);
s64 seek_offset(memory_stream *stream, s64 offset);
s64 seek_from_start(memory_stream *stream, s64 offset);
s64 seek_from_end(memory_stream *stream, s64 offset);
s64 seek_block(memory_stream *stream, s64 nth_block, u64 block_size, int whence = IO_SEEK_SET);
s64 seek_block_offset(memory_stream *stream, s64 nth_block, u64 block_size);
s64 seek_block_from_start(memory_stream *stream, s64 nth_block, u64 block_size);
s64 seek_block_from_end(memory_stream *stream, s64 nth_block, u64 block_size);
// seeks to next alignment if unaligned or does nothing if aligned
s64 seek_next_alignment(memory_stream *stream, u64 alignment);
s64 tell(memory_stream *stream);
void rewind(memory_stream *stream);

// read & write perform memcpy
// returns number of bytes read
s64 read(memory_stream *stream, void *out, u64 size);
// returns number of items read
s64 read(memory_stream *stream, void *out, u64 size, u64 nmemb);

template<typename T>
s64 read(memory_stream *stream, T *out)
{
    return read(stream, out, sizeof(T));
}

// returns bytes read
s64 read_at(memory_stream *stream, void *out, u64 offset, u64 size);
// returns number of items read
s64 read_at(memory_stream *stream, void *out, u64 offset, u64 size, u64 nmemb);

template<typename T>
s64 read_at(memory_stream *stream, T *out, u64 offset)
{
    return read_at(stream, out, offset, sizeof(T));
}

// returns bytes read
s64 read_block(memory_stream *stream, void *out, u64 block_size);
s64 read_block(memory_stream *stream, void *out, u64 nth_block, u64 block_size);
// returns number of items read
s64 read_blocks(memory_stream *stream, void *out, u64 block_count, u64 block_size);
s64 read_blocks(memory_stream *stream, void *out, u64 nth_block, u64 block_count, u64 block_size);

s64 copy_entire_stream(memory_stream *stream, void *out, u64 max_size = -1u);

// returns number of bytes written
s64 write(memory_stream *stream, const void *in, u64 size);
// returns number of items written
s64 write(memory_stream *stream, const void *in, u64 size, u64 nmemb);

template<typename T>
s64 write(memory_stream *stream, const T *in)
{
    return write(stream, in, sizeof(T));
}

s64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size);
s64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb);

template<typename T>
s64 write_at(memory_stream *stream, const T *in, u64 offset)
{
    return write_at(stream, in, offset, sizeof(T));
}

s64 write_block(memory_stream *stream, const void *in, u64 block_size);
s64 write_block(memory_stream *stream, const void *in, u64 nth_block, u64 block_size);
s64 write_blocks(memory_stream *stream, const void *in, u64 block_count, u64 block_size);
s64 write_blocks(memory_stream *stream, const void *in, u64 nth_block, u64 block_count, u64 block_size);

// get does not perform memcpy
template<typename T>
void get(const memory_stream *stream, T **out)
{
    *out = reinterpret_cast<T*>(current(stream));
}

template<typename T>
void get_aligned(const memory_stream *stream, T **out)
{
    u64 pos = (stream->position / sizeof(T)) * sizeof(T)
    *out = reinterpret_cast<T*>(stream->data + pos);
}

template<typename T>
void get_at(const memory_stream *stream, T **out, u64 offset)
{
    *out = reinterpret_cast<T*>(stream->data + offset);
}

// etc
hash_t hash(const memory_stream *stream);
