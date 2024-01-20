
#pragma once

#include "shl/platform.hpp"
#include "shl/number_types.hpp"
#include "shl/error.hpp"

#if Windows
#include <windows.h>

typedef HANDLE io_handle;
#else
typedef int io_handle;
#endif

io_handle stdin_handle();
io_handle stdout_handle();
io_handle stderr_handle();

bool set_handle_inheritance(io_handle handle, bool inherit, error *err = nullptr);

s64 read(io_handle h, char *buf, u64 size, error *err);
s64 write(io_handle h, const char *buf, u64 size, error *err);
