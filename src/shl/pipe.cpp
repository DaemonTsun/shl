
#include "shl/assert.hpp"
#include "shl/pipe.hpp"

#if Windows
#include <namedpipeapi.h>
#include "shl/format.hpp"
#else
#include <errno.h>

#include <unistd.h> // pipe2
#endif

bool init(pipe_t *p, error *err)
{
    return init(p, 0, true, err);
}

#if Windows
bool _CreatePipeEx(LPHANDLE out_read,
                   LPHANDLE out_write,
                   LPSECURITY_ATTRIBUTES attrs,
                   DWORD preferred_size,
                   DWORD read_mode,
                   DWORD write_mode)
{
    static long pipeId = 0; 
    HANDLE read_handle, write_handle;
    DWORD dwError;
    sys_char pipe_name[48] = {0};

    format(pipe_name, 48, SYS_CHAR(R"(\\.\Pipe\_anonymousPipe.%08x.%08x)"),
             (u32)GetCurrentProcessId(), (u64)InterlockedIncrement(&pipeId));

    if (preferred_size <= 0)
        preferred_size = 4096;

    read_handle = CreateNamedPipe(
        pipe_name,
        PIPE_ACCESS_INBOUND | read_mode,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        preferred_size,
        preferred_size,
        60 * 1000,
        attrs
    );

    if (!read_handle)
        return false;

    write_handle = CreateFile(
        pipe_name,
        GENERIC_WRITE,
        0,
        attrs,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | write_mode,
        nullptr
    );

    if (INVALID_HANDLE_VALUE == write_handle)
    {
        dwError = GetLastError();
        CloseHandle(read_handle);
        SetLastError(dwError);
        return false;
    }

    *out_read = read_handle;
    *out_write = write_handle;
    return true;
}
#endif

bool init(pipe_t *p, int flags, [[maybe_unused]] bool inherit, error *err)
{
    assert(p != nullptr);

#if Windows
    SECURITY_ATTRIBUTES attr{};

    attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    attr.bInheritHandle = inherit; 
    attr.lpSecurityDescriptor = NULL;

    if (!_CreatePipeEx(&p->read,
                       &p->write,
                       &attr,
                       (DWORD)flags,
                       0,
                       0))
    {
        set_GetLastError_error(err);
        return false;
    }
#else
    if (pipe2((int*)p, flags) == -1)
    {
        set_errno_error(err);
        return false;
    }
#endif

    return true;
}

bool free(pipe_t *p, error *err)
{
    assert(p != nullptr);

#if Windows
    if (p->read != nullptr && !CloseHandle(p->read))
    {
        set_GetLastError_error(err);
        return false;
    }

    p->read = nullptr;

    if (p->write != nullptr && !CloseHandle(p->write))
    {
        set_GetLastError_error(err);
        return false;
    }
    
    p->write = nullptr;

#else
    if (p->read != -1 && close(p->read) == -1)
    {
        set_errno_error(err);
        return false;
    }

    p->read = -1;

    if (p->write != -1 && close(p->write) == -1)
    {
        set_errno_error(err);
        return false;
    }

    p->write = -1;
#endif

    return true;
}
