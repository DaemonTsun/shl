
#include <string.h>
#include <stdlib.h>

#include "shl/program_context.hpp"
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
    program_context *ctx = get_context_pointer();   

    return Alloc(ctx->allocator, size);
}

void *reallocate_memory(void *ptr, s64 old_size, s64 new_size)
{
    program_context *ctx = get_context_pointer();   

    return Realloc(ctx->allocator, ptr, old_size, new_size);
}

void free_memory(void *ptr, s64 size)
{
    program_context *ctx = get_context_pointer();   

    Free(ctx->allocator, ptr, size);
}

void *move_memory(const void *from, void *to, s64 size)
{
    return ::memmove(to, from, size);
}

void *copy_memory(const void *from, void *to, s64 size)
{
    return ::memcpy(to, from, size);
}

void fill_memory(void *ptr, u8 byte, s64 size)
{
    ::memset(ptr, (int)byte, size);
}
