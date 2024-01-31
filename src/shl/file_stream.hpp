#pragma once

/* file_stream
v1.3
refactor API again
v1.2
add template write and write_at
add seek_next_alignment

TODO: docs
 */

#include "shl/io.hpp"
#include "shl/number_types.hpp"
#include "shl/error.hpp"

#define PERMISSION_READ    1
#define PERMISSION_WRITE   2
#define PERMISSION_EXECUTE 4

#define MODE_READ        1
#define MODE_WRITE       2
#define MODE_WRITE_TRUNC 4

struct file_stream
{
    io_handle handle;
    s64 cached_size;
};

// default mode is writing and reading
bool init(file_stream *stream, const char *path, error *err = nullptr);
bool init(file_stream *stream, const wchar_t *path, error *err = nullptr);
bool init(file_stream *stream, const char *path, int mode, int permissions = PERMISSION_READ | PERMISSION_WRITE, error *err = nullptr);
bool init(file_stream *stream, const wchar_t *path, int mode, int permissions = PERMISSION_READ | PERMISSION_WRITE, error *err = nullptr);
bool free(file_stream *stream, error *err = nullptr);

bool is_open(file_stream *stream);
bool is_at_end(file_stream *stream, error *err = nullptr);

// is_open && !is_at_end
bool is_ok(file_stream *stream, error *err = nullptr);

// also updates the cached size
s64 get_file_size(file_stream *stream, error *err = nullptr);
s64 block_count(file_stream *stream, u64 block_size);
s64 current_block_number(file_stream *stream, u64 block_size, error *err = nullptr);
s64 current_block_offset(file_stream *stream, u64 block_size, error *err = nullptr);

s64 seek(file_stream *stream, s64 offset, int whence = IO_SEEK_SET, error *err = nullptr);
s64 seek_offset(file_stream *stream, s64 offset, error *err = nullptr);
s64 seek_from_start(file_stream *stream, s64 offset, error *err = nullptr);
s64 seek_from_end(file_stream *stream, s64 offset, error *err = nullptr);
s64 seek_block(file_stream *stream, s64 nth_block, u64 block_size, int whence = IO_SEEK_SET, error *err = nullptr);
s64 seek_block_offset(file_stream *stream, s64 nth_block, u64 block_size, error *err = nullptr);
s64 seek_block_from_start(file_stream *stream, s64 nth_block, u64 block_size, error *err = nullptr);
s64 seek_block_from_end(file_stream *stream, s64 nth_block, u64 block_size, error *err = nullptr);
s64 seek_next_alignment(file_stream *stream, u64 alignment, error *err = nullptr);
s64 tell(file_stream *stream, error *err = nullptr);
bool rewind(file_stream *stream, error *err = nullptr);

// returns bytes read
s64 read(file_stream *stream, void *out, u64 size, error *err = nullptr);
// returns number of items read
s64 read(file_stream *stream, void *out, u64 size, u64 nmemb, error *err = nullptr);

template<typename T>
s64 read(file_stream *stream, T *out, error *err = nullptr)
{
    return read(stream, out, sizeof(T), err);
}

// returns bytes read
s64 read_at(file_stream *stream, void *out, u64 offset, u64 size, error *err = nullptr);
// returns number of items read
s64 read_at(file_stream *stream, void *out, u64 offset, u64 size, u64 nmemb, error *err = nullptr);

template<typename T>
s64 read_at(file_stream *stream, T *out, u64 offset, error *err = nullptr)
{
    return read_at(stream, out, offset, sizeof(T), err);
}

// returns bytes read
s64 read_block(file_stream *stream, void *out, u64 block_size, error *err = nullptr);
s64 read_block(file_stream *stream, void *out, s64 nth_block, u64 block_size, error *err = nullptr);
// returns number of items read
s64 read_blocks(file_stream *stream, void *out, s64 block_count, u64 block_size, error *err = nullptr);
s64 read_blocks(file_stream *stream, void *out, s64 nth_block, s64 block_count, u64 block_size, error *err = nullptr);

// uses stream->size, make sure its set and out has enough space
s64 read_entire_file(file_stream *stream, void *out, u64 max_size = -1u, error *err = nullptr);

// returns number of bytes written
s64 write(file_stream *stream, const void *in, u64 size, error *err = nullptr);
// returns number of items written
s64 write(file_stream *stream, const void *in, u64 size, u64 nmemb, error *err = nullptr);

template<typename T>
s64 write(file_stream *stream, const T *in, error *err = nullptr)
{
    return write(stream, in, sizeof(T), err);
}

s64 write_at(file_stream *stream, const void *in, u64 offset, u64 size, error *err = nullptr);
s64 write_at(file_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb, error *err = nullptr);

template<typename T>
s64 write_at(file_stream *stream, const T *in, u64 offset, error *err = nullptr)
{
    return write_at(stream, in, offset, sizeof(T), err);
}

s64 write_block(file_stream *stream, const void *in, u64 block_size, error *err = nullptr);
s64 write_block(file_stream *stream, const void *in, s64 nth_block, u64 block_size, error *err = nullptr);
s64 write_blocks(file_stream *stream, const void *in, s64 block_count, u64 block_size, error *err = nullptr);
s64 write_blocks(file_stream *stream, const void *in, s64 nth_block, s64 block_count, u64 block_size, error *err = nullptr);

