
#pragma once

/* io.hpp

I/O handles and functions.

This header provides the io_handle type which refers to kernel I/O handles.
On Linux, this is int (file descriptor), on Windows this is HANDLE.
For obtaining file handles, see shl/file_stream.hpp.

Functions:

stdin_handle()      obtains the stdin handle
stdout_handle()     obtains the stdout handle
stderr_handle()     obtains the stderr handle

INVALID_IO_HANDLE is the constant for invalid handles.

set_handle_inheritance(h, enable)   enables or disables inheritance on the given handle.
    LINUX: does nothing.

io_read(h, *buffer, max_read_size[, *err])  reads up to max_read_size bytes from the
    handle into the buffer. Returns number of bytes read, or -1 on error.

io_write(h, *buffer, write_size[, *err])    writes up to write_size bytes from the
    buffer into the handle. Returns number of bytes written, or -1 on error.

io_seek(h, offset, [whence[, *err]])    offsets the handle position by the offset,
    depending on whence. whence may be one of IO_SEEK_SET, IO_SEEK_CUR or IO_SEEK_END.
    Returns the new position, or -1 on error.
    If the handle h is not seekable (e.g. pipes), returns -1.

io_tell(h[, *err])  returns the current handle position of h, or -1 on error.

io_poll_read(h, timeout_ms[, *err]) returns whether or not the handle h has available
    data to be read by io_read, with an optional timeout in milliseconds.
    If timeout_ms is 0, returns immediately.
    WINDOWS: timeout_ms does nothing.

io_size(h[, *err])  if h is a handle to a file, returns the file size, if h
    is a pipe or socket, returns the number of available bytes to be read,
    or -1 on error.

io_is_pipe(h)   returns whether h is a pipe or not.
*/

#include "shl/platform.hpp"
#include "shl/string_encoding.hpp"
#include "shl/number_types.hpp"
#include "shl/enum_flag.hpp"
#include "shl/error.hpp"

#if Windows
#  include <windows.h>

typedef HANDLE io_handle;

#  ifndef IO_SEEK_SET
#    define IO_SEEK_SET FILE_BEGIN 
#    define IO_SEEK_CUR FILE_CURRENT 
#    define IO_SEEK_END FILE_END 
#  endif

#  define INVALID_IO_HANDLE INVALID_HANDLE_VALUE
#else
typedef int io_handle;

#  ifndef IO_SEEK_SET 
#    define IO_SEEK_SET 0 
#    define IO_SEEK_CUR 1 
#    define IO_SEEK_END 2 
#  endif

#  define INVALID_IO_HANDLE -1
#endif

io_handle stdin_handle();
io_handle stdout_handle();
io_handle stderr_handle();

// LINUX: does nothing
bool set_handle_inheritance(io_handle handle, bool inherit, error *err = nullptr);

enum class open_permission
{
    None    = 0,
    Read    = 1,
    Write   = 2,
    Execute = 4
};

enum_flag(open_permission);

constexpr open_permission open_permission_default = open_permission::Read | open_permission::Write;

enum class open_mode
{
    None        = 0,
    Read        = 1,
    Write       = 2,
    WriteTrunc  = 4
};

enum_flag(open_mode);

constexpr open_mode open_mode_default = open_mode::Read | open_mode::Write;

enum class open_flag
{
    None    = 0,
    Direct  = 1,
    Async   = 2
};

enum_flag(open_flag);

constexpr open_flag open_flag_default = open_flag::None;

io_handle io_open(const c8 *path, error *err = nullptr);
io_handle io_open(const c8 *path, open_mode mode, error *err = nullptr);
io_handle io_open(const c8 *path, open_mode mode, open_flag flags, error *err = nullptr);
io_handle io_open(const c8 *path, open_mode mode, open_flag flags, open_permission permissions, error *err = nullptr);
io_handle io_open(const c16 *path, error *err = nullptr);
io_handle io_open(const c16 *path, open_mode mode, error *err = nullptr);
io_handle io_open(const c16 *path, open_mode mode, open_flag flags, error *err = nullptr);
io_handle io_open(const c16 *path, open_mode mode, open_flag flags, open_permission permissions, error *err = nullptr);
io_handle io_open(const c32 *path, error *err = nullptr);
io_handle io_open(const c32 *path, open_mode mode, error *err = nullptr);
io_handle io_open(const c32 *path, open_mode mode, open_flag flags, error *err = nullptr);
io_handle io_open(const c32 *path, open_mode mode, open_flag flags, open_permission permissions, error *err = nullptr);

bool io_close(io_handle h, error *err = nullptr);

s64 io_read(io_handle h, char *buf, u64 size, error *err = nullptr);
s64 io_write(io_handle h, const char *buf, u64 size, error *err = nullptr);
s64 io_seek(io_handle h, s64 offset, int whence = IO_SEEK_SET, error *err = nullptr);
s64 io_tell(io_handle h, error *err = nullptr);

// WINDOWS: currently, timeout_ms does nothing and the function returns immediately
bool io_poll_read(io_handle h, u32 timeout_ms = 0, error *err = nullptr);
bool io_poll_write(io_handle h, u32 timeout_ms = 0, error *err = nullptr);

// file size for files, available bytes for pipes
s64 io_size(io_handle h, error *err = nullptr);

bool io_is_pipe(io_handle h);
