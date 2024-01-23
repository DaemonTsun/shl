
#pragma once

// TODO: docs

#include "shl/platform.hpp"
#include "shl/number_types.hpp"
#include "shl/error.hpp"

#if Windows
#include <windows.h>

typedef HANDLE io_handle;

#ifndef IO_SEEK_SET
#define IO_SEEK_SET FILE_BEGIN 
#define IO_SEEK_CUR FILE_CURRENT 
#define IO_SEEK_END FILE_END 
#endif

#else
typedef int io_handle;

#ifndef IO_SEEK_SET 
#define IO_SEEK_SET 0 
#define IO_SEEK_CUR 1 
#define IO_SEEK_END 2 
#endif
#endif

io_handle stdin_handle();
io_handle stdout_handle();
io_handle stderr_handle();

// LINUX: does nothing
bool set_handle_inheritance(io_handle handle, bool inherit, error *err = nullptr);

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
