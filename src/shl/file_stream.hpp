#pragma once

/* file_stream
 * v1.2
 * add template write and write_at
 * add seek_next_alignment
 *
 * more consistent file stream functions
 */

#include <stdio.h>
#include "shl/number_types.hpp"

#define MODE_READ "r"
#define MODE_WRITE "w"
#define MODE_BINARY "b"

struct file_stream
{
    FILE *handle;
    u64 size;
    u64 block_size;

    explicit operator FILE*() const;
};

void init(file_stream *stream);

bool open(file_stream *stream, const char *path, const char *mode = MODE_READ, bool check_open = false, bool calc_size = true);
bool close(file_stream *stream);
bool is_open(const file_stream *stream);
bool is_at_end(file_stream *stream);
bool has_error(file_stream *stream);
void clear_error(file_stream *stream);

// is_open && !is_at_end && !has_error
bool is_ok(file_stream *stream);

// this calculates and sets the size of the stream
u64 calculate_file_size(file_stream *stream);
u64 block_count(const file_stream *stream);

int seek(file_stream *stream, long offset, int whence = SEEK_SET);
int seek_block(file_stream *stream, long nth_block, int whence = SEEK_SET);
int seek_next_alignment(file_stream *stream, u64 alignment);
u64 tell(file_stream *stream);
bool getpos(file_stream *stream, fpos_t *pos);
bool rewind(file_stream *stream);

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
u64 read_entire_file(file_stream *stream, void *out, u64 max_size = -1u);

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

int format(FILE *stream, const char *format_string, ...);
int format(file_stream *stream, const char *format_string, ...);

int flush(file_stream *stream);
