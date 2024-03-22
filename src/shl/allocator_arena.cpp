
#include "shl/assert.hpp"
#include "shl/memory.hpp"
#include "shl/allocator_arena.hpp"

#define Alignment 8

void init(arena *a, s64 size)
{
    a->start = (char*)allocate_memory(size);
    a->end = a->start != nullptr ? (a->start + size) : nullptr;
}

void free(arena *a)
{
    if (a == nullptr)
        return;

    if (a->start != nullptr)
        free_memory(a->start, arena_remaining_size(*a));

    a->start = nullptr;
    a->end = nullptr;
}

s64 arena_remaining_size(arena a)
{
    return a.end - a.start;
}

static void *_arena_alloc(arena *a, s64 size)
{
    if (size == 0)
        return nullptr;

    s64 padding = -(u64)a->start & (Alignment - 1);
    s64 size_left = arena_remaining_size(*a) - padding;

    if (size_left < 0)
        return nullptr;

    void *ret = a->start + padding;
    a->start += (padding + size);

    return ret;
}

static void *_arena_free(arena *a, char *ptr, s64 size)
{
    if (ptr + size == a->start)
        a->start -= size;

    return nullptr;
}

static void *_arena_realloc(arena *a, char *ptr, s64 old_size, s64 new_size)
{
    if (old_size == new_size)
        return ptr;

    if (ptr + old_size == a->start)   
    {
        // if ptr is the last thing in the arena, we don't need to reallocate,
        // we just need to check if the new size fits into the remaining arena
        // if we're growing, or adjust a->start backwards if we're shrinking.
        s64 diff = new_size - old_size;

        if (new_size > old_size)
        {
            if (arena_remaining_size(*a) < diff)
                return nullptr;
        }

        a->start += diff;
        return ptr;
    }
    else
    {
        // not at end

        if (new_size < old_size)
            return ptr; // nothing to do

        void *ret = _arena_alloc(a, new_size);

        if (ret == nullptr)
            return ret;

        copy_memory(ptr, ret, old_size);
        return ret;
    }
}

void *arena_alloc(void *context, void *ptr, s64 old_size, s64 new_size)
{
    assert(context != nullptr);
    assert(old_size >= 0);
    assert(new_size >= 0);

    arena *a = (arena*)context;

    if (ptr == nullptr)
        return _arena_alloc(a, new_size);

    if (new_size == 0)
        return _arena_free(a, (char*)ptr, old_size);

    return _arena_realloc(a, (char*)ptr, old_size, new_size);
}
