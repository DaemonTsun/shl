
#include "shl/defer.hpp"
#include "shl/string.hpp"
#include "shl/process.hpp"

#if Linux
#include <unistd.h> // fork, pid_t, _exit, ...
#include <string.h>
#include <errno.h>

const char *_get_exe_name(const char *path)
{
    if (path == nullptr)
        return nullptr;

    const char *lastsep = strrchr(path, '/');

    if (lastsep == nullptr)
        return nullptr;

    if (lastsep[1] == '\0')
        return nullptr;

    return lastsep + 1;
}
#endif

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
        append_string(cmdline, SYS_CHAR('"'));
        join(args, arg_count, SYS_CHAR("\" \""), cmdline);
        append_string(cmdline, SYS_CHAR('"'));
    }
}

void _cmdline_to_args(const sys_char *cmd, const sys_char *exe, array<sys_char*> *args)
{
    // TODO: empty arguments, more tests...
    sys_string arg{};
    sys_char c;

    const char *exe_name = _get_exe_name(exe);
    if (exe_name != nullptr)
    {
        set_string(&arg, exe_name);
        add_at_end(args, arg.data);
        init(&arg);
    }

    s64 len = string_length(cmd);
    s64 i = 0;

    bool in_quotes = false;

    while (i < len)
    {
        c = cmd[i];
        if (c == SYS_CHAR('\\'))
        {
            if (i + 1 >= len)
                break;

            append_string(&arg, cmd[i+1]);
            i += 2;
            continue;
        }
        else if (c == SYS_CHAR('"'))
        {
            in_quotes = !in_quotes;
            i += 1;
            continue;
        }
        else if (c == SYS_CHAR(' '))
        {
            if (!in_quotes)
            {
                if (arg.size > 0)
                {
                    add_at_end(args, arg.data);
                    init(&arg);
                }

                i += 1;
                continue;
            }
        }

        append_string(&arg, c);
        i += 1;
    }

    if (arg.size > 0)
    {
        add_at_end(args, arg.data);
        init(&arg);
    }

    add_at_end(args, (sys_char*)nullptr);
}

void init(process_start_info *info)
{
    assert(info != nullptr);

    fill_memory(info, 0);

#if Windows
    info->detail.cb = sizeof(STARTUPINFO);
#else
    info->detail.std_in = stdin_handle();
    info->detail.std_out = stdout_handle();
    info->detail.std_err = stderr_handle();
#endif
}

void _free_process_start_info_arguments(process_start_info *info)
{
    if (info->_free_args && info->args != nullptr)
    {
#if Windows
        u64 len = string_length(info->args);
        sys_string _cmdline{.data = info->args, .size = len, .reserved_size = len};
        free(&_cmdline);
#else
        u64 arg_count = _get_argument_count(info->args);
        array<sys_char*> _args{.data = (sys_char**)info->args, .size = arg_count, .reserved_size = arg_count};

        for_array(sstr, &_args)
            free_memory(*sstr);

        free(&_args);
#endif

        info->args = nullptr;
    }
}

void free(process_start_info *info)
{
    assert(info != nullptr);

    _free_process_start_info_arguments(info);
}

void init(process *p)
{
    assert(p != nullptr);

    fill_memory(p, 0);
    init(&p->start_info);
}

void free(process *p)
{
    if (p == nullptr)
        return;

    free(&p->start_info);

#if Windows
    if (p->detail.hProcess != nullptr)
        CloseHandle(p->detail.hProcess);

    if (p->detail.hThread != nullptr)
        CloseHandle(p->detail.hThread);

    p->detail.hProcess = nullptr;
    p->detail.hThread = nullptr;
#endif
}

void set_process_executable(process *p, const sys_char *exe)
{
    assert(p != nullptr);

    p->start_info.path = exe;
}

void set_process_arguments(process *p, const sys_char *args)
{
    assert(p != nullptr);

    _free_process_start_info_arguments(&p->start_info);

#if Windows
    sys_string cmdline = copy_string(args);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
#else
    array<sys_char*> _args{};
    _cmdline_to_args(args, p->start_info.path, &_args);

    p->start_info.args = (const sys_char**)_args.data;
    p->start_info._free_args = true;
#endif
}

void set_process_arguments(process *p, const sys_char **args, bool raw)
{
    assert(p != nullptr);

#if Windows
    sys_string cmdline{};
    _args_to_cmdline(Args, &cmdline);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
#else
    if (raw)
    {
        p->start_info.args = args;
        p->start_info._free_args = false;
    }
    else
    {
        // arguments are copied and exe name is added at the start.
        // all of it is freed in free(process_start_info*).
        array<sys_char*> _args{};
        u64 arg_count = _get_argument_count(args);

        // we add the name of the executable
        const char *exe_name = _get_exe_name(p->start_info.path);

        if (exe_name != nullptr)
            add_at_end(&_args, strdup(exe_name));

        for (u64 i = 0; i < arg_count; ++i)
            add_at_end(&_args, strdup(args[i]));

        if (_args.data != nullptr)
        {
            add_at_end(&_args, (char *)nullptr);

            p->start_info.args = (const sys_char**)_args.data;
            p->start_info._free_args = true;
        }
        else
        {
            p->start_info.args = nullptr;
            p->start_info._free_args = false;
        }
    }
#endif
}

void set_process_io(process *p, io_handle in, io_handle out, io_handle err_out)
{
    assert(p != nullptr);

#if Windows
    p->start_info.detail.dwFlags |= STARTF_USESTDHANDLES;
    p->start_info.detail.hStdInput = in;
    p->start_info.detail.hStdOutput = out;
    p->start_info.detail.hStdError = err_out;
#else
    p->start_info.detail.std_in = in;
    p->start_info.detail.std_out = out;
    p->start_info.detail.std_err = err_out;
#endif
}

void get_process_io(process *p, io_handle *in, io_handle *out, io_handle *err_out)
{
    assert(p != nullptr);

#if Windows
    if (in != nullptr)      *in = p->start_info.detail.hStdInput;
    if (out != nullptr)     *out = p->start_info.detail.hStdOutput;
    if (err_out != nullptr) *err_out = p->start_info.detail.hStdError;
#else
    if (in != nullptr)      *in = p->start_info.detail.std_in;
    if (out != nullptr)     *out = p->start_info.detail.std_out;
    if (err_out != nullptr) *err_out = p->start_info.detail.std_err;
#endif
}

bool start_process(process *p, error *err)
{
    assert(p != nullptr);

#if Windows
    if (!CreateProcess(p->start_info.path,
                       p->start_info.args,
                       nullptr,
                       nullptr,
                       p->start_info.inherit_handles,
                       0,
                       p->start_info.environment,
                       nullptr,
                       (LPSTARTUPINFO)&p->start_info.detail,
                       &p->detail))
    {
        set_GetLastError_error(err);
        return false;
    }
#else
    pid_t id = fork();

    switch (id)
    {
    case -1:
    {
        set_errno_error(err);
        return false;
    }
    case 0:
    {
        // child process

        if (dup2(p->start_info.detail.std_in,  STDIN_FILENO)  == -1) _exit(errno);
        if (dup2(p->start_info.detail.std_out, STDOUT_FILENO) == -1) _exit(errno);
        if (dup2(p->start_info.detail.std_err, STDERR_FILENO) == -1) _exit(errno);

        execve(p->start_info.path, (char * const*)p->start_info.args, (char * const*)p->start_info.environment);

        // on success, execve does not return to here.
        _exit(errno);
    }
    default:
    {
        // parent process
    }
    }

#endif

    return true;
}
