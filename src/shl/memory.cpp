
#include <string.h>
#include <stdlib.h>

#include "shl/memory.hpp"

void *allocate_memory(s64 size)
{
    return ::malloc(size);
}

void *allocate_zeroed_memory(s64 size)
{
    return ::calloc(1, size);
}

void *reallocate_memory(void *ptr, s64 size)
{
    return ::realloc(ptr, size);
}

void *move_memory(const void *from, void *to, s64 size)
{
    return ::memmove(to, from, size);
}

void *copy_memory(const void *from, void *to, s64 size)
{
    return ::memcpy(to, from, size);
}

void free_memory(void *ptr)
{
    ::free(ptr);
}

void free_memory(void *ptr, [[maybe_unused]] s64 size)
{
    // TODO: do the thing
    free_memory(ptr);
}

void fill_memory(void *ptr, u8 byte, s64 size)
{
    ::memset(ptr, (int)byte, size);
}

void *default_alloc([[maybe_unused]] void *context, void *ptr, [[maybe_unused]] s64 old_size, s64 new_size)
{
    if (ptr == nullptr)
        return allocate_memory(new_size);

    if (new_size == 0)
    {
        free_memory(ptr);
        return nullptr;
    }

    return reallocate_memory(ptr, new_size);
}
