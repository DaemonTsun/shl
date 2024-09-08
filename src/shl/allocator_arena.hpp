
#pragma once

/* allocator_arena.hpp

Defines arena and arena_allocator.
Example usage:

    arena a{};
    init(&a, 4096);

    allocator al = arena_alloc(&arena);
    int *x = (int*)allocator_alloc(al, sizeof(int));
    // use x...
    allocator_dealloc(al, x, sizeof(int));

    free(&a);
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
