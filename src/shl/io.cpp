
#include "shl/string_encoding.hpp"
#include "shl/memory.hpp"
#include "shl/io.hpp"

#if Linux
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/statx.hpp"
#include "shl/impl/linux/select.hpp"
#include "shl/impl/linux/io.hpp"
#include "shl/time.hpp" // timespan
#endif

template<typename C>
static inline s64 _string_len(const C *str)
{
    s64 ret = 0;

    while (*str++ != '\0')
        ret++;

    return ret;
}

io_handle stdin_handle()
{
#if Windows
    return GetStdHandle(STD_INPUT_HANDLE);
#else
    return 0; // STDIN_FILENO;
#endif
}

io_handle stdout_handle()
{
#if Windows
    return GetStdHandle(STD_OUTPUT_HANDLE);
#else
    return 1; // STDOUT_FILENO;
#endif
}

io_handle stderr_handle()
{
#if Windows
    return GetStdHandle(STD_ERROR_HANDLE);
#else
    return 2; // STDERR_FILENO;
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

#if Windows
struct _CreateFile_params
{
    int access;
    int share;
    int creation;
    int flags;
};

static void _get_CreateFile_params(_CreateFile_params *out, int flags, int mode, int permissions)
{
    out->_access = 0;
    out->_share = 0;
    out->_creation = 0;
    out->_flags = FILE_ATTRIBUTE_NORMAL;
    bool _rd = mode & OPEN_MODE_READ;
    bool _wr = (mode & OPEN_MODE_WRITE) || (mode & OPEN_MODE_WRITE_TRUNC);

    if (permissions & OPEN_PERMISSION_READ)    out->_access |= GENERIC_READ;
    if (permissions & OPEN_PERMISSION_WRITE)   out->_access |= GENERIC_WRITE;
    if (permissions & OPEN_PERMISSION_EXECUTE) out->_access |= GENERIC_EXECUTE;

    if (_rd && _wr)
    {
        out->_access |= GENERIC_READ | GENERIC_WRITE;
        out->_share = 0;
    }
    else if (_rd)
    {
        out->_access = GENERIC_READ;
        out->_share = FILE_SHARE_READ;
        out->_creation = OPEN_EXISTING;
    }
    else if (_wr)
    {
        out->_access = GENERIC_WRITE;
        out->_share = 0;
    }

    if (mode & OPEN_MODE_WRITE)
    {
        out->_creation = OPEN_ALWAYS;
    }
    else if (mode & OPEN_MODE_WRITE_TRUNC)
    {
        out->_creation = CREATE_ALWAYS;
    }

    if (flags & OPEN_FLAGS_DIRECT)
    {
        out->_flags |= FILE_FLAG_NO_BUFFERING;

        if (_wr)
            out->_flags |= FILE_FLAG_WRITE_THROUGH;
    }

    if (flags & OPEN_FLAGS_ASYNC)
    {
        out->_flags |= FILE_FLAG_OVERLAPPED;
    }
}
#endif

io_handle io_open(const char *path, error *err)
{
    return io_open(path, OPEN_FLAGS_DEFAULT, OPEN_MODE_DEFAULT, OPEN_PERMISSION_DEFAULT, err);
}

io_handle io_open(const char *path, int flags, error *err)
{
    return io_open(path, flags, OPEN_MODE_DEFAULT, OPEN_PERMISSION_DEFAULT, err);
}

io_handle io_open(const char *path, int flags, int mode, error *err)
{
    return io_open(path, flags, mode, OPEN_PERMISSION_DEFAULT, err);
}

io_handle io_open(const char *path, int flags, int mode, int permissions, error *err)
{
#if Windows
    _CreateFile_params p{};
    _get_CreateFile_params(&p, flags, mode, permissions);

    io_handle h = CreateFileA(path,
                              p._access,
                              p._share,
                              nullptr,
                              p._creation,
                              p._flags,
                              nullptr);

    if (h == INVALID_HANDLE_VALUE)
        set_GetLastError_error(err);

    return h;
#else
    int _flags = 0;
    int _mode = 0;
    bool _rd = mode & OPEN_MODE_READ;
    bool _wr = (mode & OPEN_MODE_WRITE) || (mode & OPEN_MODE_WRITE_TRUNC);

    if (_wr)
        _flags |= O_CREAT;

    if (mode & OPEN_MODE_WRITE_TRUNC)
        _flags |= O_TRUNC;

    if (_rd && _wr) _flags |= O_RDWR;
    else if (_rd)   _flags |= O_RDONLY;
    else if (_wr)   _flags |= O_WRONLY;

    if (permissions & OPEN_PERMISSION_READ)    _mode |= 0400;
    if (permissions & OPEN_PERMISSION_WRITE)   _mode |= 0200;
    if (permissions & OPEN_PERMISSION_EXECUTE) _mode |= 0100;

    if (flags & OPEN_FLAGS_DIRECT) _flags |= O_DIRECT;

    /* O_ASYNC is signal I/O, not io_uring. We don't want that.
    if (flags & OPEN_FLAGS_ASYNC) _flags |= O_ASYNC;
    */

    io_handle fd = ::open(path, _flags, _mode);

    if (fd < 0)
        set_error_by_code(err, -fd);

    return fd;
#endif
}

io_handle io_open(const wchar_t *path, error *err)
{
    return io_open(path, OPEN_FLAGS_DEFAULT, OPEN_MODE_DEFAULT, OPEN_PERMISSION_DEFAULT, err);
}

io_handle io_open(const wchar_t *path, int flags, error *err)
{
    return io_open(path, flags, OPEN_MODE_DEFAULT, OPEN_PERMISSION_DEFAULT, err);
}

io_handle io_open(const wchar_t *path, int flags, int mode, error *err)
{
    return io_open(path, flags, mode, OPEN_PERMISSION_DEFAULT, err);
}

io_handle io_open(const wchar_t *path, int flags, int mode, int permissions, error *err)
{
#if Windows
    _CreateFile_params p{};
    _get_CreateFile_params(&p, flags, mode, permissions);

    io_handle h = CreateFileW(path,
                              p._access,
                              p._share,
                              nullptr,
                              p._creation,
                              p._flags,
                              nullptr);

    if (h == INVALID_HANDLE_VALUE)
        set_GetLastError_error(err);

    return h;
#else
    s64 wchar_count = _string_len(path);
    s64 char_count = string_conversion_chars_required(path, wchar_count) + 1;
    char *tmp = ::alloc<char>(char_count);

    ::fill_memory((void*)tmp, 0, char_count);
    string_convert(path, wchar_count, tmp, char_count);
    
    io_handle ret = ::io_open(tmp, flags, mode, permissions);

    dealloc_T<char>(tmp, char_count);

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
        ret = INVALID_IO_HANDLE;
    }

    return ret;
#endif
}

bool io_close(io_handle h, error *err)
{
#if Windows
    if (h != INVALID_IO_HANDLE)
    if (!CloseHandle(h))
    {
        set_GetLastError_error(err);
        return false;
    }

    return true;
#else
    if (h != INVALID_IO_HANDLE)
    if (sys_int ret = ::close(h); ret < 0)
    {
        set_error_by_code(err, -ret);
        return false;
    }

    return true;
#endif
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

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
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

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
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

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
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
static void _set_timeval(u32 ms, timespan *t)
{
    s64 usec = (s64)ms * 1000;

    if (usec > 1000000)
    {
        t->seconds  = usec / 1000000;
        t->nanoseconds = usec % 1000000;
    }
    else
    {
        t->seconds = 0;
        t->nanoseconds = usec;
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
    timespan t{};
    _set_timeval(timeout_ms, &t);

    fd_set set = {};
    FD_SET(h, &set);

    s64 ret = pselect6(h + 1, &set, 0, 0, &t, nullptr);

    // timeout
    if (ret == 0)
        return false;

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
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
    timespan t{};
    _set_timeval(timeout_ms, &t);

    fd_set set{};
    FD_SET(h, &set);

    s64 ret = pselect6(h + 1, 0, &set, 0, &t, nullptr);

    // timeout
    if (ret == 0)
        return false;

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
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
#define FIONREAD 0x541B
        s64 size = 0;
        s64 ret = 0;

        ret = (sys_int)linux_syscall3(SYS_ioctl,
                                      (void*)(sys_int)h,
                                      (void*)FIONREAD,
                                      (void*)&size);

        if (ret < 0)
        {
            set_error_by_code(err, -ret);
            return -1;
        }

        return size;
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

        s64 ret = ::statx(h, "", AT_EMPTY_PATH, STATX_SIZE, &info);

        if (ret < 0)
        {
            set_error_by_code(err, -ret);
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
