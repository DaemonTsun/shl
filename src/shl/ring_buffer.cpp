
#include "shl/assert.hpp"
#include "shl/platform.hpp"
#include "shl/number_types.hpp"
#include "shl/memory.hpp" // copy_memory, get_system_allocation_granularity

#if Linux
#include <errno.h>
#include <unistd.h>

#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/memory.hpp" // mmap

static int _memfd_create(const char *name, u32 flags)
{
    return (int)(sys_int)linux_syscall2(SYS_memfd_create, (void*)name, (void*)(sys_int)flags);
}

#elif Windows
#include <windows.h>
typedef PVOID (*f_VirtualAlloc2)(
  HANDLE                 Process,
  PVOID                  BaseAddress,
  SIZE_T                 Size,
  ULONG                  AllocationType,
  ULONG                  PageProtection,
  MEM_EXTENDED_PARAMETER *ExtendedParameters,
  ULONG                  ParameterCount
);
typedef PVOID (*f_MapViewOfFile3)(
  HANDLE                 FileMapping,
  HANDLE                 Process,
  PVOID                  BaseAddress,
  ULONG64                Offset,
  SIZE_T                 ViewSize,
  ULONG                  AllocationType,
  ULONG                  PageProtection,
  MEM_EXTENDED_PARAMETER *ExtendedParameters,
  ULONG                  ParameterCount
);
#endif

#include "shl/math.hpp"
#include "shl/compare.hpp"
#include "shl/defer.hpp"
#include "shl/ring_buffer.hpp"

#define ALLOC_RETRY_COUNT 10

bool init(ring_buffer *buf, s64 min_size, s32 mapping_count, error *err)
{
    assert(buf != nullptr);
    assert(mapping_count > 0);

    buf->data = nullptr;

    s64 granularity = get_system_allocation_granularity();
    s64 actual_size = ceil_multiple2(min_size, granularity);
    assert(actual_size > 0);

    s64 total_size = actual_size * mapping_count;

#if Linux
    int anonfd = _memfd_create("ringbuf", 0);

    if (anonfd < 0)
    {
        set_error_by_code(err, -anonfd);
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
        ptr = mmap(nullptr, total_size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

        if (MMAP_IS_ERROR(ptr))
        {
            // no pointer found
            set_error_by_code(err, -(sys_int)ptr);
            return false;
        }

        for (s32 i = 0; i < mapping_count; ++i)
        {
            void *mapped_ptr = mmap(((char*)ptr) + (i * actual_size), actual_size,
                                      PROT_READ | PROT_WRITE,
                                      MAP_FIXED | MAP_SHARED,
                                      anonfd, 0);

            if (MMAP_IS_ERROR(mapped_ptr))
            {
                // un-roll
                for (s32 j = 0; j < i; ++j)
                if (sys_int ret = munmap(((char*)ptr) + (j * actual_size), actual_size); ret < 0)
                {
                    set_error_by_code(err, -ret);
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

    static HMODULE _kernel = LoadLibraryW(L"kernelbase.dll");

    f_VirtualAlloc2  VirtualAlloc2  = (f_VirtualAlloc2)(void*)GetProcAddress(_kernel,  "VirtualAlloc2");
    f_MapViewOfFile3 MapViewOfFile3 = (f_MapViewOfFile3)(void*)GetProcAddress(_kernel, "MapViewOfFile3");

    char *ptr = nullptr;

    if (VirtualAlloc2 != nullptr && MapViewOfFile3 != nullptr)
    {
        ptr = (char*)VirtualAlloc2(0, 0, total_size, MEM_RESERVE | MEM_RESERVE_PLACEHOLDER, PAGE_NOACCESS, 0, 0);
        bool mapped = true;

        if (ptr)
        {
            for (s32 i = 0; i < mapping_count; ++i)
            {
                s64 offset = i * actual_size;
                VirtualFree(ptr + offset, actual_size, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);

                void *mapping = MapViewOfFile3(fd, 0, ptr + offset, 0, actual_size, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, 0, 0);

                if (mapping == nullptr)
                {
                    // un-roll
                    for (s32 j = 0; j < i; ++j)
                        UnmapViewOfFile(ptr + (j * actual_size));

                    mapped = false;

                    break;
                }
            }
        }
    }
    else
    {
        // Mingw or older Windows...
        ptr = (char*)VirtualAlloc(0, total_size, MEM_RESERVE, PAGE_NOACCESS);
        bool mapped = true;

        for (s32 i = 0; i < mapping_count; ++i)
        {
            s64 offset = i * actual_size;
            VirtualFree(ptr + offset, actual_size, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);

            void *mapping = MapViewOfFileEx(fd, FILE_MAP_ALL_ACCESS, 0, 0, actual_size, ptr + offset);

            if (mapping == nullptr)
            {
                // un-roll
                for (s32 j = 0; j < i; ++j)
                    UnmapViewOfFile(ptr + (j * actual_size));

                mapped = false;

                break;
            }
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
    for (s32 i = 0; i < buf->mapping_count; ++i)
    if (sys_int ret = munmap(buf->data + (i * buf->size), buf->size); ret < 0)
    {
        set_error_by_code(err, -ret);
        return false;
    }

    buf->data = nullptr;

    return true;
#elif Windows
    (void)err;
    for (s32 i = 0; i < buf->mapping_count; ++i)
        UnmapViewOfFile(buf->data + (i * buf->size));

    buf->data = nullptr;

    return true;
#else
    return false;
#endif
}

bool resize(ring_buffer *buf, s64 min_size, s32 mapping_count, error *err)
{
    assert(buf != nullptr);

    s64 pagesize = get_system_pagesize();
    s64 actual_size = ceil_multiple2(min_size, pagesize);

    if (buf->size == actual_size && buf->mapping_count == mapping_count)
        return true;

    ring_buffer nbuf{};
    
    if (!init(&nbuf, min_size, mapping_count, err))
        return false;

    s64 size_to_copy = Min(buf->size, nbuf.size);

    copy_memory(buf->data, nbuf.data, size_to_copy);
    free(buf);
    *buf = nbuf;

    return true;
}
