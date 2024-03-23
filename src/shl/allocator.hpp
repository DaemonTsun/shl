
#pragma once

/* allocator.hpp

Defines the allocator type, alloc function type and the default allocator.

TODO: finish docs
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
    (reinterpret_cast<typeof(Ptr)>(allocator_dealloc((A), (Ptr), sizeof(Type))))

#define _allocator_dealloc_TCount(A, Ptr, Type, Count)\
    (reinterpret_cast<typeof(Ptr)>(allocator_dealloc((A), (Ptr), sizeof(Type) * (Count))))

#define allocator_dealloc_T(...) GET_MACRO3(__VA_ARGS__, _allocator_dealloc_TCount, _allocator_dealloc_T, _allocator_dealloc_TNotEnoughArgs)(__VA_ARGS__)
