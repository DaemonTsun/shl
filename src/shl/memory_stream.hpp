#pragma once

/* memory_stream
 * v1.2
 * add seek_next_alignment
 *
 * defines the memory_stream struct and its functions.
 *
 * use init(stream) before using a new memory_stream instance.
 *
 * use open(stream, size, ...) create a memory_stream with newly allocated memory of
 * the given size.
 * use open(stream, data, size, ...) to create a memory_stream of existing data.
 *
 * both open functions have check_open and free_on_close parameters to check if
 * the memory_stream should free the current data before using the new one.
 * free_on_close does not automatically free the data when calling close(), as
 * close() has its own free parameter.
 */

#include "shl/hash.hpp"
#include "shl/number_types.hpp"
#include "shl/error.hpp"

struct memory_stream
{
    char *data;
    u64 size;
    u64 block_size; // TODO: get rid of this
    u64 position;

    explicit operator char*() const;
};

void init(memory_stream *stream);

bool open(memory_stream *stream, u64 size, bool check_open_and_close = false, bool free_on_close = true, error *err = nullptr);
bool open(memory_stream *stream, char *in, u64 size, bool check_open_and_close = false, bool free_on_close = true, error *err = nullptr);
bool close(memory_stream *stream, bool free = true);
bool is_open(const memory_stream *stream);
bool is_at_end(const memory_stream *stream);

// is_open && !is_at_end
bool is_ok(const memory_stream *stream);

u64 block_count(const memory_stream *stream);
// returns a pointer in the data at the current position
char *current(const memory_stream *stream);
char *current_block_start(const memory_stream *stream);
u64 current_block_number(const memory_stream *stream);
u64 current_block_offset(const memory_stream *stream);

void seek(memory_stream *stream, u64 position);
int seek(memory_stream *stream, s64 offset, int whence);
void seek_offset(memory_stream *stream, s64 offset);
void seek_from_end(memory_stream *stream, s64 offset);
void seek_block(memory_stream *stream, u64 nth_block);
int seek_block(memory_stream *stream, s64 nth_block, int whence);
void seek_block_offset(memory_stream *stream, s64 nth_block);
void seek_block_from_end(memory_stream *stream, s64 nth_block);
// seeks to next alignment if unaligned or does nothing if aligned
void seek_next_alignment(memory_stream *stream, u64 alignment);
u64 tell(memory_stream *stream);
void rewind(memory_stream *stream);

// read & write perform memcpy
// returns number of bytes read
u64 read(memory_stream *stream, void *out, u64 size);
// returns number of items read
u64 read(memory_stream *stream, void *out, u64 size, u64 nmemb);

template<typename T>
u64 read(memory_stream *stream, T *out)
{
    return read(stream, out, sizeof(T));
}

// returns bytes read
u64 read_at(memory_stream *stream, void *out, u64 offset, u64 size);
// returns number of items read
u64 read_at(memory_stream *stream, void *out, u64 offset, u64 size, u64 nmemb);

template<typename T>
u64 read_at(memory_stream *stream, T *out, u64 offset)
{
    return read_at(stream, out, offset, sizeof(T));
}

// returns bytes read
u64 read_block(memory_stream *stream, void *out);
u64 read_block(memory_stream *stream, void *out, u64 nth_block);
// returns number of items read
u64 read_blocks(memory_stream *stream, void *out, u64 block_count);
u64 read_blocks(memory_stream *stream, void *out, u64 nth_block, u64 block_count);

u64 copy_entire_stream(memory_stream *stream, void *out, u64 max_size = -1u);

// returns number of bytes written
u64 write(memory_stream *stream, const void *in, u64 size);
// returns number of items written
u64 write(memory_stream *stream, const void *in, u64 size, u64 nmemb);
u64 write(memory_stream *stream, const char *str);

template<typename T>
u64 write(memory_stream *stream, const T *in)
{
    return write(stream, in, sizeof(T));
}

u64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size);
u64 write_at(memory_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb);
u64 write_at(memory_stream *stream, const char *str, u64 offset);

template<typename T>
u64 write_at(memory_stream *stream, const T *in, u64 offset)
{
    return write_at(stream, in, offset, sizeof(T));
}

u64 write_block(memory_stream *stream, const void *in);
u64 write_block(memory_stream *stream, const void *in, u64 nth_block);
u64 write_blocks(memory_stream *stream, const void *in, u64 block_count);
u64 write_blocks(memory_stream *stream, const void *in, u64 nth_block, u64 block_count);

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
