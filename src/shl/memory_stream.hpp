#pragma once

/* memory_stream
v1.3
redesign API again
v1.2
add seek_next_alignment

Defines the memory_stream struct and its functions.
The memory_stream struct contains 3 members:
    data, a pointer to the data
    size, the total size of data
    position, the current stream position

Functions:

init(*Stream, Size)
    initializes Stream with a new memory buffer of size Size.
    
is_open(*Stream) returns if Stream has a valid handle.
is_at_end(*Stream) returns if the Stream is at the end.

is_ok(*Stream) returns if the Stream is open and not at the end.

block_count(*Stream, Blocksize)
    returns the number of blocks in Stream given the block size Blocksize and
    the size of Stream.
    Does not calculate or query the file size and only uses the cached size. 

current(*Stream)
    returns a pointer to the data of Stream at the current Stream position.

current_block_start(*Stream, Blocksize)
    returns a pointer to the data of Stream at the start of the current
    block of size Blocksize.

current_block_start2(*Stream, Blocksize)
    optimized version of current_block_start where Blocksize is a power of 2.

current_block_number(*Stream, Blocksize)
    returns the number of the block where the Stream position is currently
    located at.
    Basically tell(Stream) / Blocksize.

current_block_offset(*Stream, Blocksize)
    returns the start offset of the current block in Stream.

current_block_offset2(*Stream, Blocksize)
    optimized version of current_block_offset where Blocksize is a power of 2.

seek(*Stream, Offset, Whence)
    sets the Stream position to a value depending on Offset and Whence.
    If Whence is IO_SEEK_SET, sets the Stream position to Offset.
    If Whence is IO_SEEK_CUR, adds Offset to the Stream position.
    If Whence is IO_SEEK_END, sets the Stream position to (Stream size + Offset).
    Returns the new position.

seek_offset(*Stream, Offset)     basically seek(Stream, Offset, IO_SEEK_CUR).
seek_from_start(*Stream, Offset) basically seek(Stream, Offset, IO_SEEK_SET).
seek_from_end(*Stream, Offset)   basically seek(Stream, Offset, IO_SEEK_END).

seek_block(*Stream, NthBlock, Blocksize, Whence)
    sets the Stream position to a value depending on Nthblock, Blocksize and Whence.
    If Whence is IO_SEEK_SET, sets the Stream position to NthBlock * Blocksize.
    If Whence is IO_SEEK_CUR, adds NthBlock * Blocksize to the offset of the current block.
    If Whence is IO_SEEK_END, sets the Stream position to (Stream size + NthBlock * Blocksize).
    Returns the new position.

seek_block_offset(*Stream, NthBlock, Blocksize)
    basically seek_block(Stream, NthBlock, Blocksize, IO_SEEK_CUR).

seek_block_from_start(*Stream, NthBlock, Blocksize)
    basically seek_block(Stream, NthBlock, Blocksize, IO_SEEK_SET).

seek_block_from_end(*Stream, NthBlock, Blocksize)
    basically seek_block(Stream, NthBlock, Blocksize, IO_SEEK_END).

seek_next_alignment(*Stream, Alignment)
    sets the Stream position to the start of the next Alignment.
    Returns the new position.

seek_next_alignment2(*Stream, Alignment)
    sets the Stream position to the start of the next Alignment.
    Always assumes Alignment is a power of 2.
    Returns the new position.

tell(*Stream)
    Returns the Stream position.

rewind(*Stream)
    Sets the Stream position to the beginning, which is probably 0.
    Returns true when successful.

read(*Stream, *Out, Size)
    read Size bytes from Stream and writes them to Out.
    Returns the number of bytes read (and written to Out).
    The Stream position is advanced by the number of bytes read.
    The returned number may be less than Size.

read(*Stream, *Out, Size, N)
    reads Size * N bytes from Stream and writes them to Out.
    Returns the number of bytes / Size read.
    The Stream position is advanced by the number of bytes read.
    The returned number may be less than Size * N.

read<T>(*Stream, T *Out)
    basically read(Stream, Out, sizeof(T))

read_at(*Stream, *Out, Offset, Size[, *Err])
    sets the Stream position to Offset before reading up to Size bytes from
    Stream to Out.
    Returns the number of bytes read.

read_at(*Stream, *Out, Offset, Size, N[, *Err])
    sets the Stream position to Offset before reading up to Size * N bytes
    from Stream to Out.
    Returns the number of bytes / Size read.

read_at<T>(*Stream, T *Out, Offset)
    basically read_at(Stream, Out, Offset, sizeof(T))

read_block(*Stream, *Out, Blocksize)
    reads one block of size Blocksize at the current Stream position from
    Stream to Out.
    Returns the number of bytes read, which is either Blocksize if the entire
    block could be read, 0 if Blocksize bytes could not be read.

read_blocks(*Stream, *Out, N, Blocksize)
    reads N blocks of size Blocksize at the current Stream position from
    Stream to Out.
    Returns the number of blocks read.

read_blocks(*Stream, *Out, NthBlock, N, Blocksize)
    reads N blocks of size Blocksize starting from the offset of the NthBlock
    from Stream to Out.
    Returns the number of blocks read.

read_entire_file(*Stream, *Out, MaxSize)
    reads the entire contents of Stream into Out, up to MaxSize bytes.
    If MaxSize is -1, reads the entire Stream.
    Returns the number of bytes read.
    On success, does not modify the Stream position.

write(*Stream, *In, Size)
    writes Size bytes from In to Stream at the current Stream position.
    Returns the number of bytes written.

write(*Stream, *In, Size, N)
    writes Size * N bytes from In to Stream at the current Stream position.
    Returns the number of bytes / Size written.

write<T>(*Stream, T *In)
    basically write(Stream, In, sizeof(T))

write_at(*Stream, *In, Offset, Size)
    sets Stream position to Offset, then writes Size bytes from In to Stream.
    Returns the number of bytes written.

write_at(*Stream, *In, Offset, Size, N)
    sets Stream position to Offset, then writes Size * N bytes from In to Stream.
    Returns the number of bytes / Size written.

write_at<T>(*Stream, T *In, Offset)
    basically write_at(Stream, In, Offset, sizeof(T))

write_block(*Stream, *In, Blocksize)
    writes one block of Blocksize from In to Stream at the current Stream position.
    Returns the number of bytes written.

write_block(*Stream, *In, NthBlock, Blocksize)
    writes one block of Blocksize from In to Stream.
    Returns the number of bytes written.

write_blocks(*Stream, *In, N, Blocksize)
    writes N blocks of size Blocksize from In to Stream at the current Stream position.
    Returns the number of blocks written.

write_blocks(*Stream, *In, NthBlock, N, Blocksize)
    writes N blocks of size Blocksize from In to Stream starting at the offset of
    the NthBlock in Stream.
    Returns the number of blocks written.
*/

#include "shl/io.hpp" // IO_SEEK_...
#include "shl/hash.hpp"
#include "shl/number_types.hpp"

struct memory_stream
{
    char *data;
    u64 size;
    u64 position;
};

void init(memory_stream *stream, u64 size);
void free(memory_stream *stream);

bool is_open(memory_stream *stream);
bool is_at_end(memory_stream *stream);

// is_open && !is_at_end
bool is_ok(memory_stream *stream);

s64 block_count(memory_stream *stream, u64 block_size);
// returns a pointer in the data at the current position
char *current(memory_stream *stream);
char *current_block_start(memory_stream *stream, u64 block_size);
char *current_block_start2(memory_stream *stream, u64 block_size);
s64 current_block_number(memory_stream *stream, u64 block_size);
s64 current_block_offset(memory_stream *stream, u64 block_size);
s64 current_block_offset2(memory_stream *stream, u64 block_size);

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
void get(memory_stream *stream, T **out)
{
    *out = reinterpret_cast<T*>(current(stream));
}

template<typename T>
void get_aligned(memory_stream *stream, T **out)
{
    u64 pos = (stream->position / sizeof(T)) * sizeof(T)
    *out = reinterpret_cast<T*>(stream->data + pos);
}

template<typename T>
void get_at(memory_stream *stream, T **out, u64 offset)
{
    *out = reinterpret_cast<T*>(stream->data + offset);
}

// etc
hash_t hash(memory_stream *stream);
