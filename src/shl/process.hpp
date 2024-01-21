
#pragma once

#include "shl/platform.hpp"
#include "shl/pipe.hpp"
#include "shl/error.hpp"

#if Windows
#include <windows.h>
#elif Linux
#define pipe __original_pipe
#include <signal.h>
#undef pipe
#endif

// platform-specific structs
// struct process_start_detail;
// struct process;

#if Windows
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
    const sys_char *path;
    
#if Windows
    sys_char *args;
#else
    const sys_char **args;
#endif

    const sys_char **environment;
    bool inherit_handles; // used only on windows

    bool _free_args; // if allocated by set_process_arguments
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

void init(process *p);
void free(process *p);

void set_process_executable(process *p, const sys_char *exe);
void set_process_arguments(process *p, const sys_char *args);
void set_process_arguments(process *p, const sys_char **args, bool raw = false);

void set_process_io(process *p, io_handle in, io_handle out, io_handle err_out);
void get_process_io(process *p, io_handle *in, io_handle *out, io_handle *err_out);

bool start_process(process *p, error *err);
bool stop_process(process *p, error *err);
bool stop_process(int pid, error *err);

// current process
int get_pid();
int get_parent_pid();
// given process
int get_pid(const process *p);


