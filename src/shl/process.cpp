
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
                       false,
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

bool start_process(process *p, const sys_char *path, const sys_char *args[], const sys_char *environment[], error *err)
{
    process_start_info info{};
    info.path = path;
#if Windows
    sys_string cmdline{};
    _args_to_cmdline(args, &cmdline);
    defer { free(&cmdline); };
    info.args = cmdline.data;
#else
    info.args = args;
#endif
    info.environment = environment;

    return start_process(p, &info, err);
}

bool start_process(process *p, const sys_char *path, const sys_char *args, const sys_char *environment[], error *err)
{
    process_start_info info{};
    info.path = path;

#if Windows
    sys_string cmdline = copy_string(args);
    defer { free(&cmdline); };
    info.args = cmdline.data;
#else
    array<sys_char*> _args{};
    _cmdline_to_args(args, &_args);

    defer
    {
        for_array(sstr, &_args)
            free_memory(*sstr);

        free(&_args);
    };

    info.args = _args.data;
#endif
    info.environment = environment;

    return start_process(p, &info, err);
}
