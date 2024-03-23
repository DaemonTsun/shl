
#include "shl/memory.hpp"
#include "shl/allocator.hpp"

void *default_alloc([[maybe_unused]] void *data, void *ptr, [[maybe_unused]] s64 old_size, s64 new_size)
{
    if (ptr == nullptr)
        return _libc_malloc(new_size);

    if (new_size == 0)
    {
        _libc_free(ptr);
        return nullptr;
    }

    return _libc_realloc(ptr, new_size);
}
