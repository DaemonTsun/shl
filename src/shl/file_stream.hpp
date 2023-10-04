#pragma once

/* file_stream
 * v1.2
 * add template write and write_at
 * add seek_next_alignment
 *
 * defines the file_stream struct, a lightweight wrapper for FILE*.
 *
 * init(stream) an instance of the struct before using it.
 *
 * use open(stream, path, mode, check if open, calcualte size) to open a file.
 * close(stream) to close the stream and release the handle.
 *
 * by default, the file size will get calculated with open(...) unless calculate_size is false.
 * the file size can also be calculated retroactively by calling calculate_file_size(stream).
 *
 * a block size may be specified for the block functions.
 */

#include <stdio.h>
#include "shl/number_types.hpp"
#include "shl/error.hpp"

#define MODE_READ "r"
#define MODE_WRITE "w"
#define MODE_BINARY "b"

struct file_stream
{
    FILE *handle;
    u64 size;
    u64 block_size; // TODO: get rid of this

    explicit operator FILE*() const;
};

void init(file_stream *stream);

bool open(file_stream *stream, const char *path, const char *mode = MODE_READ, bool check_open_and_close = false, bool calc_size = true, error *err = nullptr);
bool close(file_stream *stream, error *err = nullptr);
bool is_open(const file_stream *stream);
bool is_at_end(file_stream *stream);
bool has_error(file_stream *stream);
void clear_error(file_stream *stream);

// is_open && !is_at_end && !has_error
bool is_ok(file_stream *stream);

// this calculates and sets the size of the stream
s64 calculate_file_size(file_stream *stream, error *err = nullptr);
u64 block_count(const file_stream *stream);

int seek(file_stream *stream, s64 offset, int whence = SEEK_SET, error *err = nullptr);
int seek_block(file_stream *stream, s64 nth_block, int whence = SEEK_SET, error *err = nullptr);
int seek_next_alignment(file_stream *stream, u64 alignment, error *err = nullptr);
s64 tell(file_stream *stream, error *err = nullptr);
bool getpos(file_stream *stream, fpos_t *pos);
bool rewind(file_stream *stream, error *err = nullptr);

// returns bytes read
u64 read(file_stream *stream, void *out, u64 size);
// returns number of items read
u64 read(file_stream *stream, void *out, u64 size, u64 nmemb);

template<typename T>
u64 read(file_stream *stream, T *out)
{
    return read(stream, out, sizeof(T));
}

// returns bytes read
u64 read_at(file_stream *stream, void *out, u64 offset, u64 size);
// returns number of items read
u64 read_at(file_stream *stream, void *out, u64 offset, u64 size, u64 nmemb);

template<typename T>
u64 read_at(file_stream *stream, T *out, u64 offset)
{
    return read_at(stream, out, offset, sizeof(T));
}

// returns bytes read
u64 read_block(file_stream *stream, void *out);
u64 read_block(file_stream *stream, void *out, u64 nth_block);
// returns number of items read
u64 read_blocks(file_stream *stream, void *out, u64 block_count);
u64 read_blocks(file_stream *stream, void *out, u64 nth_block, u64 block_count);

// uses stream->size, make sure its set and out has enough space
u64 read_entire_file(file_stream *stream, void *out, u64 max_size = -1u, error *err = nullptr);

// returns number of bytes written
u64 write(file_stream *stream, const void *in, u64 size);
// returns number of items written
u64 write(file_stream *stream, const void *in, u64 size, u64 nmemb);
u64 write(file_stream *stream, const char *str);

template<typename T>
u64 write(file_stream *stream, const T *in)
{
    return write(stream, in, sizeof(T));
}

u64 write_at(file_stream *stream, const void *in, u64 offset, u64 size);
u64 write_at(file_stream *stream, const void *in, u64 offset, u64 size, u64 nmemb);
u64 write_at(file_stream *stream, const char *str, u64 offset);

template<typename T>
u64 write_at(file_stream *stream, const T *in, u64 offset)
{
    return write_at(stream, in, offset, sizeof(T));
}

u64 write_block(file_stream *stream, const void *in);
u64 write_block(file_stream *stream, const void *in, u64 nth_block);
u64 write_blocks(file_stream *stream, const void *in, u64 block_count);
u64 write_blocks(file_stream *stream, const void *in, u64 nth_block, u64 block_count);

// printf, not format.hpp
// use tprint(stream->handle, fmt, ...) to use format.hpp format.
int format(FILE *stream, const char *format_string, ...);
int format(file_stream *stream, const char *format_string, ...);

int flush(file_stream *stream, error *err = nullptr);

