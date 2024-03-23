
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

void *alloc(s64 size)
{
    program_context *ctx = get_context_pointer();   

    return allocator_alloc(ctx->allocator, size);
}

void *realloc(void *ptr, s64 old_size, s64 new_size)
{
    program_context *ctx = get_context_pointer();   

    return allocator_realloc(ctx->allocator, ptr, old_size, new_size);
}

void dealloc(void *ptr, s64 size)
{
    program_context *ctx = get_context_pointer();   

    allocator_dealloc(ctx->allocator, ptr, size);
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
