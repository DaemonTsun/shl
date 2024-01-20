
#include <assert.h>

#include "shl/pipe.hpp"

#if Windows
#include <namedpipeapi.h>
#else
#include <errno.h>
#include <string.h>

// beautiful
#define pipe __default_pipe
#include <unistd.h> // pipe2
#undef pipe
#endif

bool init(pipe *p, error *err)
{
    return init(p, 0, true, err);
}

bool init(pipe *p, int flags, bool inherit, error *err)
{
    assert(p != nullptr);

#if Windows
    SECURITY_ATTRIBUTES attr{};

    attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    attr.bInheritHandle = inherit; 
    attr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&p->read,
                    &p->write,
                    &attr,
                    (DWORD)flags))
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

bool free(pipe *p, error *err)
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
