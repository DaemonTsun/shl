
#include "shl/defer.hpp"
#include "shl/string.hpp"
#include "shl/process.hpp"

typedef string_base<sys_char> sys_string;

u64 _get_argument_count(const sys_char **args)
{
    u64 arg_count = 0;

    while (*args != nullptr)
    {
        args++;
        arg_count += 1;
    }

    return arg_count;
}

void _args_to_cmdline(const sys_char **args, sys_string *cmdline)
{
    u64 arg_count = _get_argument_count(args);

    if (arg_count > 0)
    {
        append_string(cmdline, SYS_CHAR("\""));
        join(args, arg_count, SYS_CHAR("\" \""), cmdline);
        append_string(cmdline, SYS_CHAR("\""));
    }
}

void _cmdline_to_args(const sys_char *cmdline, array<sys_char*> *args)
{
    /* TODO: implement
    u64 arg_start = 0;
    u64 arg_end = 0;

    s64 len = string_length(cmdline);
    s64 i = 0;

    while (i < len)
    {
        while (cmdline[i] == SYS_CHAR('\\'))
        

        i += 1;
    }

    arg_end = i;
    */
}

bool start_process(process *p, const process_start_info *info, error *err)
{
#if Windows
    if (!CreateProcess(info->path,
                       info->args,
                       nullptr,
                       nullptr,
                       info->inherit_handles,
                       0,
                       info->environment,
                       nullptr,
                       (LPSTARTUPINFO)&info->detail,
                       &p->detail))
    {
        set_GetLastError_error(err);
        return false;
    }
#else
    // TODO: implement linux execve
#endif

    return true;
}

#if Windows
#define _set_args_from_array(Info, Args)\
    sys_string cmdline{};\
    _args_to_cmdline(Args, &cmdline);\
    defer { free(&cmdline); };\
    Info.args = cmdline.data;
#else
#define _set_args_from_array(Info, Args)\
    Info.args = Args;
#endif

#if Windows
#define _set_args_from_string(Info, Args)\
    sys_string cmdline = copy_string(Args);\
    defer { free(&cmdline); };\
    Info.args = cmdline.data;
#else
#define _set_args_from_string(Info, Args)\
    array<sys_char*> _args{};\
    _cmdline_to_args(Args, &_args);\
\
    defer\
    {\
        for_array(sstr, &_args)\
            free_memory(*sstr);\
\
        free(&_args);\
    };\
\
    Info.args = _args.data;
#endif

#if Windows
#define _set_io_handles(Info, In, Out, ErrOut)\
    info.detail.dwFlags |= STARTF_USESTDHANDLES;\
    info.detail.hStdInput = in;\
    info.detail.hStdOutput = out;\
    info.detail.hStdError = err_out;
#else
#define _set_io_handles(Info, In, Out, ErrOut)
// TODO: define
#endif

bool start_process(process *p, const sys_char *path, const sys_char *args[], const sys_char *environment[], io_handle in, io_handle out, io_handle err_out, error *err)
{
    process_start_info info{};
    info.path = path;
    _set_args_from_array(info, args);
    info.environment = environment;
    info.inherit_handles = true;

    _set_io_handles(info, in, out, err_out);

    return start_process(p, &info, err);
}

bool start_process(process *p, const sys_char *path, const sys_char *args, const sys_char *environment[], io_handle in, io_handle out, io_handle err_out, error *err)
{
    process_start_info info{};
    info.path = path;
    _set_args_from_string(info, args);
    info.environment = environment;
    info.inherit_handles = true;

    _set_io_handles(info, in, out, err_out);

    return start_process(p, &info, err);
}

bool start_process(process *p, const sys_char *path, const sys_char *args[], const sys_char *environment[], error *err)
{
    process_start_info info{};
    info.path = path;
    _set_args_from_array(info, args);
    info.environment = environment;

    return start_process(p, &info, err);
}

bool start_process(process *p, const sys_char *path, const sys_char *args, const sys_char *environment[], error *err)
{
    process_start_info info{};
    info.path = path;
    _set_args_from_string(info, args);
    info.environment = environment;

    return start_process(p, &info, err);
}
