
#if Linux
#include <unistd.h>
#endif

#include "shl/io.hpp"

io_handle stdin_handle()
{
#if Windows
    return GetStdHandle(STD_INPUT_HANDLE);
#else
    return STDIN_FILENO;
#endif
}

io_handle stdout_handle()
{
#if Windows
    return GetStdHandle(STD_OUTPUT_HANDLE);
#else
    return STDOUT_FILENO;
#endif
}

io_handle stderr_handle()
{
#if Windows
    return GetStdHandle(STD_ERROR_HANDLE);
#else
    return STDERR_FILENO;
#endif
}

bool set_handle_inheritance(io_handle handle, bool inherit, error *err)
{
#if Windows
    if (!SetHandleInformation(handle, HANDLE_FLAG_INHERIT, inherit ? HANDLE_FLAG_INHERIT : 0))
    {
        set_GetLastError_error(err);
        return false;
    }
#else
    // TODO: implement?
#endif

    return true;
}
