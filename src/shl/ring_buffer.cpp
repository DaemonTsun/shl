

#include "shl/assert.hpp"
#include "shl/platform.hpp"
#include "shl/number_types.hpp"

#if Linux
#include <sys/mman.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static int _memfd_create(const char *name, u32 flags)
{
    return syscall(SYS_memfd_create, name, flags);
}

#elif Windows
#include <windows.h>
#include <memoryapi.h>
#endif

#include "shl/math.hpp"
#include "shl/compare.hpp"
#include "shl/defer.hpp"
#include "shl/ring_buffer.hpp"

#define ALLOC_RETRY_COUNT 10

bool init(ring_buffer *buf, u64 min_size, u32 mapping_count, error *err)
{
    assert(buf != nullptr);
    assert(mapping_count > 0);

    buf->data = nullptr;

    u64 pagesize = get_system_pagesize();
    u64 actual_size = ceil_multiple2(min_size, pagesize);
    assert(actual_size > 0);

    u64 total_size = actual_size * mapping_count;

#if Linux
    int anonfd = _memfd_create("ringbuf", 0);

    if (anonfd == -1)
    {
        set_errno_error(err);
        return false;
    }

    defer { ::close(anonfd); };

    if (::ftruncate(anonfd, actual_size) == -1)
    {
        set_errno_error(err);
        return false;
    }

    void *ptr = nullptr;
    int retry_count = 0;

    while (retry_count < ALLOC_RETRY_COUNT)
    {
        bool all_ranges_mapped = true;

        // find an address that works
        ptr = ::mmap(nullptr, total_size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

        if (ptr == MAP_FAILED)
        {
            // no pointer found
            set_errno_error(err);
            return false;
        }

        for (u32 i = 0; i < mapping_count; ++i)
        {
            void *mapped_ptr = ::mmap(((char*)ptr) + (i * actual_size), actual_size,
                                      PROT_READ | PROT_WRITE,
                                      MAP_FIXED | MAP_SHARED,
                                      anonfd, 0);

            if (mapped_ptr == MAP_FAILED)
            {
                // un-roll
                for (u32 j = 0; j < i; ++j)
                if (::munmap(((char*)ptr) + (j * actual_size), actual_size) != 0)
                {
                    set_errno_error(err);
                    return false;
                }

                retry_count += 1;
                all_ranges_mapped = false;
                break;
            }
        }

        if (all_ranges_mapped)
            break;
    }

    buf->data = (char*)ptr;
    buf->size = actual_size;
    buf->mapping_count = mapping_count;

    return true;
#elif Windows
    HANDLE fd = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE,
                                   (DWORD)(actual_size >> 32), (DWORD)(actual_size & 0xffffffff), 0);

    if (fd == INVALID_HANDLE_VALUE)
    {
        set_GetLastError_error(err);
        return false;
    }

    char *ptr = (char*)VirtualAlloc2(0, 0, total_size, MEM_RESERVE | MEM_RESERVE_PLACEHOLDER, PAGE_NOACCESS, 0, 0);
    bool mapped = true;

    for (u32 i = 0; i < mapping_count; ++i)
    {
        u64 offset = i * actual_size;
        VirtualFree(ptr + offset, actual_size, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);

        void *mapping = MapViewOfFile3(fd, 0, ptr + offset, 0, actual_size, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, 0, 0);

        if (mapping == nullptr)
        {
            // un-roll
            for (u32 j = 0; j < i; ++j)
                UnmapViewOfFile(ptr + (j * actual_size));

            mapped = false;

            break;
        }
    }

    CloseHandle(fd);

    buf->data = (char*)ptr;
    buf->size = actual_size;
    buf->mapping_count = mapping_count;

    return true;
#else
    return false;
#endif
}

bool free(ring_buffer *buf, error *err)
{
    if (buf == nullptr)
        return true;

    if (buf->data == nullptr)
        return true;

#if Linux
    for (u32 i = 0; i < buf->mapping_count; ++i)
    if (::munmap(buf->data + (i * buf->size), buf->size) != 0)
    {
        set_errno_error(err);
        return false;
    }

    buf->data = nullptr;

    return true;
#elif Windows
    for (u32 i = 0; i < buf->mapping_count; ++i)
        UnmapViewOfFile(buf->data + (i * buf->size));

    buf->data = nullptr;

    return true;
#else
    return false;
#endif
}

bool resize(ring_buffer *buf, u64 min_size, u32 mapping_count, error *err)
{
    assert(buf != nullptr);

    u64 pagesize = get_system_pagesize();
    u64 actual_size = ceil_multiple2(min_size, pagesize);

    if (buf->size == actual_size && buf->mapping_count == mapping_count)
        return true;

    ring_buffer nbuf{};
    
    if (!init(&nbuf, min_size, mapping_count, err))
        return false;

    u64 size_to_copy = Min(buf->size, nbuf.size);

    memcpy(nbuf.data, buf->data, size_to_copy);
    free(buf);
    *buf = nbuf;

    return true;
}

u64 get_system_pagesize()
{
#if Windows
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    return (u64)info.dwAllocationGranularity;
#else
    return (u64)sysconf(_SC_PAGESIZE);
#endif
}
