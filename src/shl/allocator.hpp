
#pragma once

/* allocator.hpp

Defines the allocator type, alloc function type and the default allocator.

Example usage:

    allocator a = default_allocator;
    
    int *x = (int*)allocator_alloc(a, sizeof(int));
    int *y = allocator_alloc_T(a, int);

    // realloc must have old size
    x = allocator_realloc(a, x, sizeof(int), sizeof(int) * 2);
    y = allocator_realloc_T(a, y, int, 1, 2);

    // dealloc must have size
    allocator_dealloc(a, x, sizeof(int) * 2);
    allocator_dealloc_T(a, y, int, 2);


Typedefs and Structs

alloc_function is the generic allocation function type used to allocate,
deallocate and reallocate memory.

allocator is a struct with an alloc_function and a user data pointer which
is passed to the alloc function.

default_alloc is an alloc_function that uses the default global heap allocator,
currently malloc.

default_allocator is an allocator that uses default_alloc as allocation function.
null_allocator is an allocator that uses no allocation function, causing a crash
when used.

Other Allocators

arena_allocator (defined in shl/allocator_arena.hpp) - stack-like allocator.
*/

#include "shl/macros.hpp"
#include "shl/type_functions.hpp"
#include "shl/number_types.hpp"

typedef void *(*alloc_function)(void *data, void *ptr, s64 old_size, s64 new_size);

struct allocator
{
    alloc_function alloc;   
    void *data;
};

void *default_alloc(void *data, void *ptr, s64 old_size, s64 new_size);

const allocator default_allocator{.alloc = default_alloc, .data = nullptr};
const allocator null_allocator{.alloc = (alloc_function)nullptr, .data = nullptr};

// helpers
#define allocator_alloc(A, Size)\
    ((A).alloc((A).data, nullptr, 0, Size))

#define _allocator_alloc_T(A, Type)\
    (reinterpret_cast<add_pointer(Type)>(allocator_alloc((A), sizeof(Type))))

#define _allocator_alloc_TCount(A, Type, Count)\
    (reinterpret_cast<add_pointer(Type)>(allocator_alloc((A), sizeof(Type) * (Count))))

#define allocator_alloc_T(...) GET_MACRO2(__VA_ARGS__, _allocator_alloc_TCount, _allocator_alloc_T)(__VA_ARGS__)

#define allocator_realloc(A, Ptr, OldSize, NewSize)\
    ((A).alloc((A).data, (Ptr), (OldSize), (NewSize)))

#define allocator_realloc_T(A, Ptr, Type, OldCount, NewCount)\
    (reinterpret_cast<add_pointer(Type)>((A).alloc((A).data, (Ptr), sizeof(Type) * (OldCount), sizeof(Type) * (NewCount))))

#define allocator_dealloc(A, Ptr, Size)\
    ((A).alloc((A).data, (Ptr), (Size), 0))

#define _allocator_dealloc_TNotEnoughArgs(A, B)\
    static_assert(false && "allocator_dealloc_T takes at least 3 arguments: allocator, pointer, type [, count]");

#define _allocator_dealloc_T(A, Ptr, Type)\
    (reinterpret_cast<decltype(Ptr)>(allocator_dealloc((A), (Ptr), sizeof(Type))))

#define _allocator_dealloc_TCount(A, Ptr, Type, Count)\
    (reinterpret_cast<decltype(Ptr)>(allocator_dealloc((A), (Ptr), sizeof(Type) * (Count))))

#define allocator_dealloc_T(...) GET_MACRO3(__VA_ARGS__, _allocator_dealloc_TCount, _allocator_dealloc_T, _allocator_dealloc_TNotEnoughArgs)(__VA_ARGS__)
