
#pragma once

/* memory.hpp

Memory management header. Uses the current program_context's allocator.

Example:

    int *x = alloc<int>();
    dealloc(x);

    char *data = (char*)alloc(255);
    dealloc(data);

Default implementations of alloc and dealloc are malloc and free respectively, currently.

alloc(N)   returns a pointer to uninitialized, writable memory
           that is N bytes long.

alloc<T>   returns a pointer to type T of uninitialized,
           writable memory that is sizeof(T) bytes long.

alloc<T>(Count)   returns a pointer to type T of uninitialized,
                  writable memory that is sizeof(T) * Count bytes long.

alloc_T<T>(...) same as alloc<T>(...)
                    
realloc(Ptr, OldSize, NewSize)
    reallocates Ptr and returns a pointer to memory that is NewSize bytes long.
                    
realloc_T<T>(Ptr, OldCount, NewCount)
    reallocates Ptr of type T* and returns a pointer to memory that is
    NewCount * sizeof(T) bytes long.

dealloc(Ptr, Size)      frees Ptr.
dealloc_T<T>(Ptr)       frees Ptr of type T*, with sizeof(T).
dealloc_T<T>(Ptr, N)    frees Ptr of type T*, with sizeof(T) * N.
                    
move_memory(From, To, N)    effectively copies N bytes from From to To.
copy_memory(From, To, N)    effectively copies N bytes from From to To.

fill_memory(Ptr, Byte, N)   fills Ptr with N bytes with the value Byte.
fill_memory<T>(T *Ptr, Byte)    fills Ptr with sizeof(T) bytes with the
                                value Byte.
*/

#include "shl/number_types.hpp"

// these 3 will be removed at some point
void *_libc_malloc(s64 size);
void *_libc_realloc(void *p, s64 new_size);
void  _libc_free(void *p);

void *alloc(s64 size);

template<typename T>
T *alloc()
{
    return reinterpret_cast<T*>(alloc(sizeof(T)));
}

template<typename T>
T *alloc(s64 n_elements)
{
    return reinterpret_cast<T*>(alloc(sizeof(T) * n_elements));
}

#define alloc_T alloc

void *realloc(void *ptr, s64 old_size, s64 new_size);

template<typename T>
T *realloc_T(T *ptr, s64 old_count, s64 new_count)
{
    return reinterpret_cast<T*>(realloc((void*)ptr, sizeof(T) * old_count, sizeof(T) * new_count));
}

void dealloc(void *ptr, s64 size);

// unfortunately, this must be called _T because otherwise the
// function infers the type from any pointer passed to it, causing all
// dealloc calls to free only the size of T.
template<typename T>
void dealloc_T(T *ptr)
{
    dealloc(reinterpret_cast<void*>(ptr), sizeof(T));
}

template<typename T>
void dealloc_T(T *ptr, s64 n_elements)
{
    dealloc(reinterpret_cast<void*>(ptr), sizeof(T) * n_elements);
}

void *move_memory(const void *from, void *to, s64 size);
void *copy_memory(const void *from, void *to, s64 size);

void fill_memory(void *ptr, u8 byte, s64 size);

template<typename T>
void fill_memory(T *ptr, u8 byte)
{
    fill_memory(reinterpret_cast<void*>(ptr), byte, sizeof(T));
}
