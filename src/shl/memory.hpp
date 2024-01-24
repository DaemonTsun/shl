
#pragma once

/* memory.hpp

memory management header.

Default implementations of allocate_memory and free_memory are
malloc and free respectively, currently.

allocate_memory(N)  returns a pointer to uninitialized, writable memory
                    that is N bytes long.
allocate_zeroed_memory(N)   returns a pointer to zeroed, writable memory
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

fill_memory<T>(T *Ptr, Byte, N) fills Ptr with N * sizeof(T) bytes with the
                                value Byte.


 */

#include "shl/number_types.hpp"

void *allocate_memory(u64 size);
void *allocate_zeroed_memory(u64 size);

template<typename T, bool Zero = false>
T *allocate_memory()
{
    if constexpr (Zero)
        return reinterpret_cast<T*>(allocate_zeroed_memory(sizeof(T)));
    else
        return reinterpret_cast<T*>(allocate_memory(sizeof(T)));
}

template<typename T, bool Zero = false>
T *allocate_memory(u64 n_elements)
{
    if constexpr (Zero)
        return reinterpret_cast<T*>(allocate_zeroed_memory(sizeof(T) * n_elements));
    else
        return reinterpret_cast<T*>(allocate_memory(sizeof(T) * n_elements));
}

void *reallocate_memory(void *ptr, u64 size);

template<typename T>
T *reallocate_memory(T *ptr, u64 n_elements)
{
    return reinterpret_cast<T*>(reallocate_memory(reinterpret_cast<void*>(ptr), sizeof(T) * n_elements));
}

void *move_memory(const void *from, void *to, u64 size);
void *copy_memory(const void *from, void *to, u64 size);
void free_memory(void *ptr);

template<typename T>
void free_memory(T *ptr)
{
    free_memory(reinterpret_cast<void*>(ptr));
}

void fill_memory(void *ptr, u8 byte, u64 size);

template<typename T>
void fill_memory(T *ptr, u8 byte)
{
    fill_memory(reinterpret_cast<void*>(ptr), byte, sizeof(T));
}

template<typename T>
void fill_memory(T *ptr, u8 byte, u64 count)
{
    fill_memory(reinterpret_cast<void*>(ptr), byte, count * sizeof(T));
}

