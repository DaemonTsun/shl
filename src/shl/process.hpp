
#pragma once

#include "shl/platform.hpp"
#include "shl/error.hpp"

#if Windows
#include <windows.h>
#elif Linux

#endif

// platform-specific structs
// struct process_start_detail;
// struct process;

#if Windows
typedef STARTUPINFO process_start_detail;
#elif Linux
struct process_start_detail
{
    // TODO: implement
};
#endif

struct process_start_info
{
    // sys_char (defined in platform.hpp) is char on linux,
    // and wchar_t on windows ONLY if UNICODE preprocessor constant is set
    const sys_char *path;

#if Windows
    // cannot be const because Windows bad
    sys_char *args;
#else
    const sys_char **args;
#endif

    const sys_char **environment;

    process_start_detail detail;
};

struct process
{

#if Windows
    PROCESS_INFORMATION detail;
#endif
};

bool start_process(process *p, const process_start_info *info, error *err = nullptr);
bool start_process(process *p, const sys_char *path, const sys_char *args[], const sys_char *environment[] = nullptr, error *err = nullptr);
bool start_process(process *p, const sys_char *path, const sys_char *args = nullptr, const sys_char *environment[] = nullptr, error *err = nullptr);
