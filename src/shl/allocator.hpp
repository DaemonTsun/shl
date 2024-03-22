
#pragma once

#include "shl/macros.hpp"
#include "shl/type_functions.hpp"
#include "shl/number_types.hpp"

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
