
#include <string.h>
#include <stdlib.h>

#include "shl/memory.hpp"

void *allocate_memory(u64 size)
{
    return ::malloc(size);
}

void *reallocate_memory(void *ptr, u64 size)
{
    return ::realloc(ptr, size);
}

void *move_memory(const void *from, void *to, u64 size)
{
    return ::memmove(to, from, size);
}

void *copy_memory(const void *from, void *to, u64 size)
{
    return ::memcpy(to, from, size);
}

void free_memory(void *ptr)
{
    ::free(ptr);
}
