
#pragma once

/* memory.hpp

Poor memory management header. Currently just wrappers around malloc, realloc and free.

Example:

    int *x = allocate_memory<int>();
    free_memory(x);

    char *data = (char*)allocate_memory(255);
    free_memory(data);

Default implementations of allocate_memory and free_memory are
malloc and free respectively, currently.

allocate_memory(N)  returns a pointer to uninitialized, writable memory
                    that is N bytes long.

allocate_memory<T>  returns a pointer to type T of uninitialized,
                    writable memory that is sizeof(T) bytes long.
                    
reallocate_memory(Ptr, N)   reallocates Ptr and returns a pointer to memory
                            that is N bytes long.

move_memory(From, To, N)    effectively copies N bytes from From to To.
copy_memory(From, To, N)    effectively copies N bytes from From to To.

free_memory(Ptr)    frees Ptr.

fill_memory(Ptr, Byte, N)   fills Ptr with N bytes with the value Byte.
fill_memory<T>(T *Ptr, Byte)    fills Ptr with sizeof(T) bytes with the
                                value Byte.


*/

#include "shl/number_types.hpp"

// these 3 will be removed at some point
void *_libc_malloc(s64 size);
void *_libc_realloc(void *p, s64 new_size);
void  _libc_free(void *p);

void *allocate_memory(s64 size);

template<typename T>
T *allocate_memory()
{
    return reinterpret_cast<T*>(allocate_memory(sizeof(T)));
}

template<typename T>
T *allocate_memory(s64 n_elements)
{
    return reinterpret_cast<T*>(allocate_memory(sizeof(T) * n_elements));
}

void *reallocate_memory(void *ptr, s64 old_size, s64 new_size);
void free_memory(void *ptr, s64 size);

// unfortunately, this must be called _T because otherwise the
// function infers the type from any pointer passed to it.
template<typename T>
void free_memory_T(T *ptr)
{
    free_memory(reinterpret_cast<void*>(ptr), sizeof(T));
}

void *move_memory(const void *from, void *to, s64 size);
void *copy_memory(const void *from, void *to, s64 size);

void fill_memory(void *ptr, u8 byte, s64 size);

template<typename T>
void fill_memory(T *ptr, u8 byte)
{
    fill_memory(reinterpret_cast<void*>(ptr), byte, sizeof(T));
}
