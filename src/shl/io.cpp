
#include "shl/io.hpp"

#if Linux
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/ioctl.h> // ioctl
#include <fcntl.h>
#endif

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
    (void)(handle);
    (void)(inherit);
    (void)(err);
    // TODO: implement? is there something to implement?
#endif

    return true;
}

s64 io_read(io_handle h, char *buf, u64 size, error *err)
{
    s64 ret = 0;

#if Windows

    if (!ReadFile(h, buf, (DWORD)size, (LPDWORD)&ret, nullptr))
    {
        set_GetLastError_error(err);
        return -1;
    }
#else
    ret = read(h, buf, size);

    if (ret == -1)
    {
        set_errno_error(err);
        return -1;
    }
#endif

    return ret;
}

s64 io_write(io_handle h, const char *buf, u64 size, error *err)
{
    s64 ret = 0;

#if Windows
    if (!WriteFile(h, buf, (DWORD)size, (LPDWORD)&ret, nullptr))
    {
        set_GetLastError_error(err);
        return -1;
    }
#else
    ret = write(h, buf, size);

    if (ret == -1)
    {
        set_errno_error(err);
        return -1;
    }
#endif

    return ret;
}

s64 io_seek(io_handle h, s64 offset, int whence, error *err)
{
    s64 ret = 0;

#if Windows
    LARGE_INTEGER _off;
    _off.QuadPart = offset;

    if (!SetFilePointerEx(h, _off, (PLARGE_INTEGER)(&ret), whence))
    {
        set_GetLastError_error(err);
        return -1;
    }

#else
    ret = lseek(h, offset, whence);

    if (ret == -1)
    {
        set_errno_error(err);
        return -1;
    }
#endif

    return ret;
}

s64 io_tell(io_handle h, error *err)
{
    return io_seek(h, 0, IO_SEEK_CUR, err);
}

#if Linux
void _set_timeval(u32 ms, struct timeval *t)
{
    u64 usec = (u64)ms * 1000;

    if (usec > 1000000)
    {
        t->tv_sec  = usec / 1000000;
        t->tv_usec = usec % 1000000;
    }
    else
    {
        t->tv_sec = 0;
        t->tv_usec = usec;
    }
}
#endif

bool io_poll_read(io_handle h, u32 timeout_ms, error *err)
{
#if Windows
    DWORD ret;

    if (!PeekNamedPipe(h, nullptr, 0, nullptr, &ret, nullptr))
    {
        set_GetLastError_error(err);
        return false;
    }

    return ret > 0;
#else
    struct timeval t;
    _set_timeval(timeout_ms, &t);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(h, &set);

    int r = select(h + 1, &set, nullptr, nullptr, &t);

    // timeout
    if (r == 0)
        return false;

    if (r == -1)
    {
        set_errno_error(err);
        return false;
    }

    return true;
#endif
}

bool io_poll_write(io_handle h, u32 timeout_ms, error *err)
{
#if Windows
    // is there such a thing?
#else
    struct timeval t;
    _set_timeval(timeout_ms, &t);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(h, &set);

    int r = select(h + 1, nullptr, &set, nullptr, &t);

    // timeout
    if (r == 0)
        return false;

    if (r == -1)
    {
        set_errno_error(err);
        return false;
    }
#endif

    return true;
}

s64 io_size(io_handle h, error *err)
{
    if (io_is_pipe(h))
    {
#if Windows
        DWORD ret;

        if (!PeekNamedPipe(h, nullptr, 0, nullptr, &ret, nullptr))
        {
            set_GetLastError_error(err);
            return -1;
        }

        return ret;
#else
        s64 ret = 0;

        if (ioctl(h, FIONREAD, &ret) == -1)
        {
            set_errno_error(err);
            return -1;
        }

        return ret;
#endif
    }
    else
    {
        // probably a file

#if Windows
        LARGE_INTEGER sz;

        if (!GetFileSizeEx(h, &sz))
        {
            set_GetLastError_error(err);
            return -1;
        }

        return sz.QuadPart;
#elif Linux
        struct statx info;

        if (::statx(h, "", AT_EMPTY_PATH, STATX_SIZE, &info) != 0)
        {
            set_errno_error(err);
            return -1;
        }

        return (s64)info.stx_size;
#else
        // this is the fallback when there's no implementation available
        s64 curpos = io_tell(h, err);

        if (curpos < 0)
            return -1;

        if (io_seek(h, 0, IO_SEEK_END, err) < 0)
            return -1;

        s64 sz = io_tell(h, err);

        if (sz < 0)
            return sz;

        io_seek(h, curpos);

        return sz;
#endif
    }
}

bool io_is_pipe(io_handle h)
{
#if Windows
    return GetFileType(h) == FILE_TYPE_PIPE;
#else
    struct statx info;

    if (::statx(h, "", AT_EMPTY_PATH, STATX_MODE, &info) != 0)
        return false;

    return S_ISFIFO(info.stx_mode);
#endif
}
