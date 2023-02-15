
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

void free_memory(void *ptr)
{
    ::free(ptr);
}
