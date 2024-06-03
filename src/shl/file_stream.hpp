#pragma once

/* file_stream
v1.3
refactor API again
v1.2
add template write and write_at
add seek_next_alignment

File stream API for writing data to file I/O handles.

The file_stream struct has a handle member of type io_handle, and a cached size.
The cached size is updated with get_file_size(file_stream*).

Example:

    file_stream fs{};
    error err{};

    if (!init(&fs, "test.txt", &err))
        exit(err.error_code);

    char buf[256] = {0};

    s64 len = read(&fs, buf, 256, &err);

    if (len < 0)
        exit(err.error_code);

    ...
    free(&fs);

Functions

init(*Stream, Path[, *err])
    Initializes the stream with a handle to the file at Path, opening the file with
    Read and Write permissions.
    Returns false if unsuccessful and optionally writes error codes
    and messages to *err.
    Returns true if successful.

init(*Stream, Path, Mode[, *err])
    Initializes Stream with a handle to the file at Path with access mode Mode.
    MODE_READ reads an existing file.
    MODE_WRITE writes to a new or existing file, not truncating its contents.
    MODE_WRITE_TRUNC writes to a new or existing file, truncating its contents.

init(*Stream, Path, Mode, Permissions[, *err])
    initializes Stream with a handle to the file at Path with access mode Mode and
    Permissions permissions.
    Available permissions are PERMISSION_READ, PERMISSION_WRITE and PERMISSION_EXECUTE.
    
is_open(*Stream) returns if Stream has a valid handle.
is_at_end(*Stream[, *err]) returns if the Stream is at the end.

is_ok(*Stream[, *err]) returns if the Stream is open and not at the end.

get_file_size(*Stream[, *err])
    returns and caches the file size of the file handle in Stream.
    If possible, uses operating system functions to determine the size,
    otherwise calculates sizes by using seek and tell.
    On error, returns -1.

block_count(*Stream, Blocksize)
    returns the number of blocks in Stream given the block size Blocksize and
    the cached size in Stream.
    Does not calculate or query the file size and only uses the cached size. 

current_block_number(*Stream, Blocksize[, *err])
    returns the number of the block where the Stream position is currently
    located at.
    Basically tell(Stream) / Blocksize.

current_block_offset(*Stream, Blocksize[, *err])
    returns the start offset of the current block in Stream.

current_block_offset2(*Stream, Blocksize[, *err])
    optimized version of current_block_offset where Blocksize is a power of 2.

seek(*Stream, Offset, Whence[, *err])
    sets the Stream position to a value depending on Offset and Whence.
    If Whence is IO_SEEK_SET, sets the Stream position to Offset.
    If Whence is IO_SEEK_CUR, adds Offset to the Stream position.
    If Whence is IO_SEEK_END, sets the Stream position to (Stream size + Offset).
    Returns the new position.

seek_offset(*Stream, Offset[, *err])     basically seek(Stream, Offset, IO_SEEK_CUR, err).
seek_from_start(*Stream, Offset[, *err]) basically seek(Stream, Offset, IO_SEEK_SET, err).
seek_from_end(*Stream, Offset[, *err])   basically seek(Stream, Offset, IO_SEEK_END, err).

seek_block(*Stream, NthBlock, Blocksize, Whence[, *err])
    sets the Stream position to a value depending on Nthblock, Blocksize and Whence.
    If Whence is IO_SEEK_SET, sets the Stream position to NthBlock * Blocksize.
    If Whence is IO_SEEK_CUR, adds NthBlock * Blocksize to the offset of the current block.
    If Whence is IO_SEEK_END, sets the Stream position to (Stream size + NthBlock * Blocksize).
    Returns the new position.

seek_block_offset(*Stream, NthBlock, Blocksize[, *err])
    basically seek_block(Stream, NthBlock, Blocksize, IO_SEEK_CUR, err).

seek_block_from_start(*Stream, NthBlock, Blocksize[, *err])
    basically seek_block(Stream, NthBlock, Blocksize, IO_SEEK_SET, err).

seek_block_from_end(*Stream, NthBlock, Blocksize[, *err])
    basically seek_block(Stream, NthBlock, Blocksize, IO_SEEK_END, err).

seek_next_alignment(*Stream, Alignment[, *err])
    sets the Stream position to the start of the next Alignment.
    Returns the new position.

seek_next_alignment2(*Stream, Alignment[, *err])
    sets the Stream position to the start of the next Alignment.
    Always assumes Alignment is a power of 2.
    Returns the new position.

tell(*Stream[, *err])
    Returns the Stream position.

rewind(*Stream[, *err])
    Sets the Stream position to the beginning, which is probably 0.
    Returns true when successful.

read(*Stream, *Out, Size[, *err])
    read Size bytes from Stream and writes them to Out.
    Returns the number of bytes read (and written to Out), or -1 on error.
    The Stream position is advanced by the number of bytes read.
    The returned number may be less than Size.

read(*Stream, *Out, Size, N[, *err])
    reads Size * N bytes from Stream and writes them to Out.
    Returns the number of bytes / Size read, or -1 on error.
    The Stream position is advanced by the number of bytes read.
    The returned number may be less than Size * N.

read<T>(*Stream, T *Out[, *err])
    basically read(Stream, Out, sizeof(T), err)

read_at(*Stream, *Out, Offset, Size[, *Err])
    sets the Stream position to Offset before reading up to Size bytes from
    Stream to Out.
    Returns the number of bytes read, or -1 on error.

read_at(*Stream, *Out, Offset, Size, N[, *Err])
    sets the Stream position to Offset before reading up to Size * N bytes
    from Stream to Out.
    Returns the number of bytes / Size read, or -1 on error.

read_at<T>(*Stream, T *Out, Offset[, *err])
    basically read_at(Stream, Out, Offset, sizeof(T), err)

read_block(*Stream, *Out, Blocksize[, *err])
    reads one block of size Blocksize at the current Stream position from
    Stream to Out.
    Returns the number of bytes read, which is either Blocksize if the entire
    block could be read, 0 if Blocksize bytes could not be read, or -1 on error.

read_blocks(*Stream, *Out, N, Blocksize[, *err])
    reads N blocks of size Blocksize at the current Stream position from
    Stream to Out.
    Returns the number of blocks read, or -1 on error.

read_blocks(*Stream, *Out, NthBlock, N, Blocksize[, *err])
    reads N blocks of size Blocksize starting from the offset of the NthBlock
    from Stream to Out.
    Returns the number of blocks read, or -1 on error.

read_entire_file(*Stream, *Out, MaxSize[, *err])
    reads the entire contents of Stream into Out, up to MaxSize bytes.
    If MaxSize is -1, reads the entire Stream.
    Returns the number of bytes read, or -1 on error.
    On success, does not modify the Stream position.

write(*Stream, *In, Size[, *err])
    writes Size bytes from In to Stream at the current Stream position.
    Returns the number of bytes written, or -1 on error.

write(*Stream, *In, Size, N[, *err])
    writes Size * N bytes from In to Stream at the current Stream position.
    Returns the number of bytes / Size written, or -1 on error.

write<T>(*Stream, T *In[, *err])
    basically write(Stream, In, sizeof(T), err)

write_at(*Stream, *In, Offset, Size[, *err])
    sets Stream position to Offset, then writes Size bytes from In to Stream.
    Returns the number of bytes written, or -1 on error.

write_at(*Stream, *In, Offset, Size, N[, *err])
    sets Stream position to Offset, then writes Size * N bytes from In to Stream.
    Returns the number of bytes / Size written, or -1 on error.

write_at<T>(*Stream, T *In, Offset[, *err])
    basically write_at(Stream, In, Offset, sizeof(T), err)

write_block(*Stream, *In, Blocksize[, *err])
    writes one block of Blocksize from In to Stream at the current Stream position.
    Returns the number of bytes written, or -1 on error.

write_block(*Stream, *In, NthBlock, Blocksize[, *err])
    writes one block of Blocksize from In to Stream.
    Returns the number of bytes written, or -1 on error.

write_blocks(*Stream, *In, N, Blocksize[, *err])
    writes N blocks of size Blocksize from In to Stream at the current Stream position.
    Returns the number of blocks written, or -1 on error.

write_blocks(*Stream, *In, NthBlock, N, Blocksize[, *err])
    writes N blocks of size Blocksize from In to Stream starting at the offset of
    the NthBlock in Stream.
    Returns the number of blocks written, or -1 on error.
*/

#include "shl/io.hpp"
#include "shl/number_types.hpp"
#include "shl/error.hpp"

struct file_stream
{
    io_handle handle;
    s64 cached_size;
};

// default mode is writing and reading
bool init(file_stream *stream, const char *path, error *err = nullptr);
bool init(file_stream *stream, const char *path, int flags, error *err = nullptr);
bool init(file_stream *stream, const char *path, int flags, int mode, error *err = nullptr);
bool init(file_stream *stream, const char *path, int flags, int mode, int permissions, error *err = nullptr);
bool init(file_stream *stream, const wchar_t *path, error *err = nullptr);
bool init(file_stream *stream, const wchar_t *path, int flags, error *err = nullptr);
bool init(file_stream *stream, const wchar_t *path, int flags, int mode, error *err = nullptr);
bool init(file_stream *stream, const wchar_t *path, int flags, int mode, int permissions, error *err = nullptr);
bool init(file_stream *stream, io_handle handle, error *err = nullptr);
bool free(file_stream *stream, error *err = nullptr);

bool is_open(file_stream *stream);
bool is_at_end(file_stream *stream, error *err = nullptr);

// is_open && !is_at_end
bool is_ok(file_stream *stream, error *err = nullptr);

// also updates the cached size
s64 get_file_size(file_stream *stream, error *err = nullptr);
s64 block_count(file_stream *stream, s64 block_size);
s64 current_block_number(file_stream *stream, s64 block_size, error *err = nullptr);
s64 current_block_offset(file_stream *stream, s64 block_size, error *err = nullptr);
s64 current_block_offset2(file_stream *stream, s64 block_size, error *err = nullptr);

s64 seek(file_stream *stream, s64 offset, int whence = IO_SEEK_SET, error *err = nullptr);
s64 seek_offset(file_stream *stream, s64 offset, error *err = nullptr);
s64 seek_from_start(file_stream *stream, s64 offset, error *err = nullptr);
s64 seek_from_end(file_stream *stream, s64 offset, error *err = nullptr);
s64 seek_block(file_stream *stream, s64 nth_block, s64 block_size, int whence = IO_SEEK_SET, error *err = nullptr);
s64 seek_block_offset(file_stream *stream, s64 nth_block, s64 block_size, error *err = nullptr);
s64 seek_block_from_start(file_stream *stream, s64 nth_block, s64 block_size, error *err = nullptr);
s64 seek_block_from_end(file_stream *stream, s64 nth_block, s64 block_size, error *err = nullptr);
s64 seek_next_alignment(file_stream *stream, s64 alignment, error *err = nullptr);
s64 seek_next_alignment2(file_stream *stream, s64 alignment, error *err = nullptr);
s64 tell(file_stream *stream, error *err = nullptr);
bool rewind(file_stream *stream, error *err = nullptr);

// returns bytes read
s64 read(file_stream *stream, void *out, s64 size, error *err = nullptr);
// returns number of items read
s64 read(file_stream *stream, void *out, s64 size, s64 nmemb, error *err = nullptr);

template<typename T>
s64 read(file_stream *stream, T *out, error *err = nullptr)
{
    return read(stream, out, sizeof(T), err);
}

// returns bytes read
s64 read_at(file_stream *stream, void *out, s64 offset, s64 size, error *err = nullptr);
// returns number of items read
s64 read_at(file_stream *stream, void *out, s64 offset, s64 size, s64 nmemb, error *err = nullptr);

template<typename T>
s64 read_at(file_stream *stream, T *out, s64 offset, error *err = nullptr)
{
    return read_at(stream, out, offset, sizeof(T), err);
}

// returns bytes read
s64 read_block(file_stream *stream, void *out, s64 block_size, error *err = nullptr);
s64 read_block(file_stream *stream, void *out, s64 nth_block, s64 block_size, error *err = nullptr);
// returns number of items read
s64 read_blocks(file_stream *stream, void *out, s64 block_count, s64 block_size, error *err = nullptr);
s64 read_blocks(file_stream *stream, void *out, s64 nth_block, s64 block_count, s64 block_size, error *err = nullptr);

s64 read_entire_file(file_stream *stream, void *out, s64 max_size = max_value(s64), error *err = nullptr);

// returns number of bytes written
s64 write(file_stream *stream, const void *in, s64 size, error *err = nullptr);
// returns number of items written
s64 write(file_stream *stream, const void *in, s64 size, s64 nmemb, error *err = nullptr);

template<typename T>
s64 write(file_stream *stream, const T *in, error *err = nullptr)
{
    return write(stream, in, sizeof(T), err);
}

s64 write_at(file_stream *stream, const void *in, s64 offset, s64 size, error *err = nullptr);
s64 write_at(file_stream *stream, const void *in, s64 offset, s64 size, s64 nmemb, error *err = nullptr);

template<typename T>
s64 write_at(file_stream *stream, const T *in, s64 offset, error *err = nullptr)
{
    return write_at(stream, in, offset, sizeof(T), err);
}

s64 write_block(file_stream *stream, const void *in, s64 block_size, error *err = nullptr);
s64 write_block(file_stream *stream, const void *in, s64 nth_block, s64 block_size, error *err = nullptr);
s64 write_blocks(file_stream *stream, const void *in, s64 block_count, s64 block_size, error *err = nullptr);
s64 write_blocks(file_stream *stream, const void *in, s64 nth_block, s64 block_count, s64 block_size, error *err = nullptr);

