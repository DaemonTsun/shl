
#pragma once

/* scratch_buffer.hpp

contiguous dynamic memory structure with a stack buffer. 

Sometimes, the buffer size required for an operation is only known until the
operation is executed, and if the given buffer size does not suffice, these
operations will fail with an appropriate error code saying that the buffer
was not big enough. Examples of this include system calls like getdents (Linux),
or standard functions like getcwd.
If you want to reuse the results of these buffers, you will require a copy of
the contents of these buffers anyway (e.g. the path from getcwd), in which
case it's probably better to use a dynamic array, but in cases where the
result of the operation is discarded, a scratch buffer may reduce heap memory
usage.

A scratch buffer works by having a fixed size stack buffer of size N (template
parameter which you may set). The 'data' member by default points to the
stack buffer, and the size member is set to the size of the stack buffer.
When the size of the stack buffer no longer suffices, calling the grow()
function allocates a new heap buffer and sets the 'data' member to point
to the heap buffer instead. Calling grow() when 'data' points to a
heap buffer causes the heap buffer to be reallocated to the sufficient size.

Functions

init(*buf)  initializes the given scratch_buffer, sets the data pointer to
            the stack buffer, and the size to the stack buffers size.
free(*buf)  frees the scratch_buffer. If data does not point to the stack
            buffer, data is also freed. Call init before reusing.
grow_by(*buf, Factor) grows the scratch_buffer by a factor Factor
                      (multiplied by). If the stack buffer no longer suffices,
                      allocates heap memory and sets data to a pointer to the
                      heap memory.
grow(*buf)  grows the scratch_buffer by a factor of 2.
 */

#include <assert.h>

#include "shl/number_types.hpp"
#include "shl/memory.hpp"

template<u64 N>
struct scratch_buffer
{
    static constexpr u64 stack_size = N;

    char *data;
    u64 size;
    char stack_buffer[N];
};

template<u64 N>
void init(scratch_buffer<N> *buf)
{
    assert(buf != nullptr);

    buf->data = buf->stack_buffer;
    buf->size = N;
}

template<u64 N>
void free(scratch_buffer<N> *buf)
{
    if (buf == nullptr)
        return;

    if (buf->data != buf->stack_buffer)
        free_memory(buf->data);

    buf->data = nullptr;
    buf->size = 0;
}

template<u64 N>
u64 grow_by(scratch_buffer<N> *buf, s64 factor)
{
    assert(buf != nullptr);

    u64 nsize = (u64)(buf->size * factor);

    // overflow, highly unlikely
    if (nsize < buf->size)
        return buf->size;

    // no need to allocate anything, stack size is large enough
    if (nsize <= N)
    {
        assert(buf->data == buf->stack_buffer);
        buf->size = nsize;
        return nsize;
    }

    if (buf->data == buf->stack_buffer)
    {
        // stack size no longer suffices, allocate more
        buf->data = allocate_memory<char>(nsize);
        buf->size = nsize;
    }
    else
    {
        buf->data = reallocate_memory<char>(buf->data, nsize);
        buf->size = nsize;
    }

    return nsize;
}

template<u64 N>
u64 grow(scratch_buffer<N> *buf)
{
    return grow_by(buf, 2);
}
