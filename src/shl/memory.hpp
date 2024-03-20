
#pragma once

/* memory.hpp

Poor memory management header. Currently just wrappers around malloc, realloc and free.

Example:

    int *x = allocate_memory<int>();
    free_memory(x);

    char *data = allocate_memory(255);
    free_memory(data);

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
#include "shl/type_functions.hpp"
#include "shl/macros.hpp"

void *allocate_memory(s64 size);
void *allocate_zeroed_memory(s64 size);

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

void *reallocate_memory(void *ptr, s64 size);

template<typename T>
T *reallocate_memory(T *ptr, s64 n_elements)
{
    return reinterpret_cast<T*>(reallocate_memory(reinterpret_cast<void*>(ptr), sizeof(T) * n_elements));
}

void *move_memory(const void *from, void *to, s64 size);
void *copy_memory(const void *from, void *to, s64 size);
void free_memory(void *ptr);

template<typename T>
void free_memory(T *ptr)
{
    free_memory(reinterpret_cast<void*>(ptr));
}

void fill_memory(void *ptr, u8 byte, s64 size);

template<typename T>
void fill_memory(T *ptr, u8 byte)
{
    fill_memory(reinterpret_cast<void*>(ptr), byte, sizeof(T));
}

template<typename T>
void fill_memory(T *ptr, u8 byte, s64 count)
{
    fill_memory(reinterpret_cast<void*>(ptr), byte, count * sizeof(T));
}


// allocators
typedef void *(*alloc_function)(void *context, void *ptr, s64 old_size, s64 new_size);

struct allocator
{
    alloc_function alloc;   
    void *context;
};

void *default_alloc(void *context, void *ptr, s64 old_size, s64 new_size);

const allocator default_allocator{.alloc = default_alloc, .context = nullptr};

// helpers
#define Alloc(A, Size)\
    ((A).alloc((A).context, nullptr, 0, Size))

#define _AllocT(A, Type)\
    (reinterpret_cast<add_pointer(Type)>(Alloc((A), sizeof(Type))))

#define _AllocTCount(A, Type, Count)\
    (reinterpret_cast<add_pointer(Type)>(Alloc((A), sizeof(Type) * (Count))))

#define AllocT(...) GET_MACRO2(__VA_ARGS__, _AllocTCount, _AllocT)(__VA_ARGS__)

#define Realloc(A, Ptr, OldSize, NewSize)\
    ((A).alloc((A).context, (Ptr), (OldSize), (NewSize)))

#define ReallocT(A, Ptr, Type, OldCount, NewCount)\
    (reinterpret_cast<add_pointer(Type)>((A).alloc((A).context, (Ptr), sizeof(Type) * (OldCount), sizeof(Type) * (NewCount))))

#define Free(A, Ptr, Size)\
    ((A).alloc((A).context, (Ptr), (Size), 0))

#define _FreeTNotEnoughArgs(A, B)\
    static_assert(false && "FreeT takes at least 3 arguments: allocator, pointer, type [, count]");

#define _FreeT(A, Ptr, Type)\
    (reinterpret_cast<typeof(Ptr)>(Free((A), (Ptr), sizeof(Type))))

#define _FreeTCount(A, Ptr, Type, Count)\
    (reinterpret_cast<typeof(Ptr)>(Free((A), (Ptr), sizeof(Type) * (Count))))

#define FreeT(...) GET_MACRO3(__VA_ARGS__, _FreeTCount, _FreeT, _FreeTNotEnoughArgs)(__VA_ARGS__)
