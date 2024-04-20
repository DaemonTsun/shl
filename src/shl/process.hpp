
#pragma once

/* process.hpp

Process header. Defines structs and functions to work with processes.

Example usage:

    process p;
    process_create(&p);

    set_process_executable(&p, "/usr/bin/echo");
    set_process_arguments(&p, "hello world");

    process_start(&p);

Functions:
init(*process)  initializes the process struct with default values.
free(*process)  deallocates any memory used by the process struct internally
                and makes the process struct available to be init'd again.

set_process_executable(*process, Exe)   sets the executable to be run for the
    process to Exe.
    Does nothing to the process if it's already running.

set_process_arguments(*process, Args)   sets the arguments for _starting_ the
    process to the given arguments. In most cases, the arguments are copied,
    unless the overload has a "raw" flag to be set.
    Does nothing to the process if it's already running.
    WINDOWS: arguments are always copied because CreateProcess expects
             arguments to be writable.

set_process_io(*process, In, Out, Err)  sets the io_handles of the process
    to In, Out and Err.
    Does nothing to the process if it's already running.

get_process_io(*process, In, Out, Err)  gets the io_handles of the process.

start_process(*process[, err])  starts the process with the parameters set
    and returns whether or not starting was successful.

stop_process(*process[, err])   stops the given process that is associated
    with the *process struct and returns whether or not stopping was
    successful.

stop_process(Pid[, err])    stops the process that has the given process Id
    Pid and returns whether or not stopping was successful.

get_pid()   gets the process ID of the currently running process.
get_pid(*process)  gets the process ID of the given process.
get_parent_pid()   gets the parent process ID of the currently running process.
*/

#include "shl/platform.hpp"
#include "shl/io.hpp"
#include "shl/error.hpp"

#if Windows
#include <windows.h>

typedef STARTUPINFO process_start_detail;
typedef PROCESS_INFORMATION process_detail;

#elif Linux
struct process_start_detail
{
    io_handle std_in;
    io_handle std_out;
    io_handle std_err;
};

struct process_detail
{
    int pid;
};
#endif

struct process_start_info
{
    // executable path
    const sys_char *path;
    
#if Windows
    sys_char *args;
#else
    const sys_char **args;
#endif

    const sys_char **environment;
    bool  inherit_handles; // used only on windows

    bool _free_exe_path; // if converted
    s64  _free_exe_path_size;
    bool _free_args; // if allocated by set_process_arguments or converted
    s64 *_free_args_sizes;
    process_start_detail detail;
};

void init(process_start_info *info);
void free(process_start_info *info);

struct process
{
    process_start_info start_info;

    // populated once started
    process_detail detail;
};

void process_create (process *p);
void process_destroy(process *p);

void set_process_executable(process *p, const char    *exe);
void set_process_executable(process *p, const wchar_t *exe);
void set_process_arguments (process *p, const char    *args);
void set_process_arguments (process *p, const wchar_t *args);
void set_process_arguments (process *p, const char    **args, bool raw = false);
void set_process_arguments (process *p, const wchar_t **args);

void set_process_io(process *p, io_handle  in, io_handle  out, io_handle  err_out);
void get_process_io(process *p, io_handle *in, io_handle *out, io_handle *err_out);

bool process_start(process *p, error *err);
bool process_stop (process *p, error *err);
bool process_stop (int pid, error *err);

int get_process_id();
int get_process_id(const process *p);

int get_parent_process_id();


