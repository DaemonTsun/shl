
#include <string.h>
#include <stdlib.h>

#include "shl/memory.hpp"

void *_libc_malloc(s64 size)
{
    return ::malloc(size);
}

void *_libc_realloc(void *ptr, s64 size)
{
    return ::realloc(ptr, size);
}

void _libc_free(void *ptr)
{
    ::free(ptr);
}

void *allocate_memory(s64 size)
{
    return _libc_malloc(size);
}

void *reallocate_memory(void *ptr, s64 size)
{
    return _libc_realloc(ptr, size);
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
    _libc_free(ptr);
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
