
#pragma once

/* allocator_arena.hpp

Defines arena and arena_allocator.
*/

#include "shl/allocator.hpp"
#include "shl/number_types.hpp"

struct arena
{
    char *start;
    char *end;
};

void init(arena *a, s64 size);
void free(arena *a);

s64 arena_remaining_size(arena a);

void *arena_alloc(void *data, void *ptr, s64 old_size, s64 new_size);

inline static allocator arena_allocator(arena *a)
{
    return allocator{.alloc = arena_alloc, .data = (void*)a};
}
