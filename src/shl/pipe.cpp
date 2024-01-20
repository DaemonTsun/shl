
#include <assert.h>

#include "shl/pipe.hpp"

#if Windows
#include <namedpipeapi.h>
#endif

bool init(pipe *p, u64 preferred_size, bool inherit, error *err)
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
                    (DWORD)preferred_size))
    {
        set_GetLastError_error(err);
        return false;
    }
#else
    // TODO
#endif

    return true;
}

bool init(pipe *p, error *err)
{
    return init(p, 0, true, err);
}

bool free(pipe *p, error *err)
{
    assert(p != nullptr);

#if Windows
    if (!CloseHandle(p->read))
    {
        set_GetLastError_error(err);
        return false;
    }

    p->read = nullptr;

    if (!CloseHandle(p->write))
    {
        set_GetLastError_error(err);
        return false;
    }
    
    p->write = nullptr;

#else
    // TODO
#endif

    return true;
}
