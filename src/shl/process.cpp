
#include "shl/string.hpp"
#include "shl/memory.hpp"
#include "shl/type_functions.hpp"
#include "shl/process.hpp"

#define LIT(C, Literal)\
    inline_const_if(is_same(C, char), Literal, L##Literal)

typedef string_base<sys_char> sys_string;

#if Windows
#include <tlhelp32.h> // CreateToolhelp32Snapshot
#else
#include <string.h>
#include <errno.h>
#include "shl/impl/linux/exit.hpp"
#include "shl/impl/linux/process.hpp"
#include "shl/impl/linux/signal.hpp"
#include "shl/impl/linux/io.hpp"


const char *_get_exe_name(const char *path)
{
    if (path == nullptr)
        return nullptr;

    const char *lastsep = strrchr(path, '/');

    if (lastsep == nullptr)
    {
        return nullptr;
    }
    else
    {
        if (lastsep[1] == '\0')
            return nullptr;

        return lastsep + 1;
    }
}

void _cmdline_to_args(const sys_char *cmd, const sys_char *exe, array<sys_char*> *args, array<s64> *sizes)
{
    // TODO: empty arguments, more tests...
    sys_string arg{};
    sys_char c;

    const char *exe_name = _get_exe_name(exe);
    if (exe_name != nullptr)
    {
        string_set(&arg, exe_name);
        add_at_end(args, arg.data);
        add_at_end(sizes, arg.reserved_size);
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

            string_append(&arg, char_to_const_string(cmd[i+1]));
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
                    add_at_end(sizes, arg.reserved_size);
                    init(&arg);
                }

                i += 1;
                continue;
            }
        }

        string_append(&arg, char_to_const_string(c));
        i += 1;
    }

    if (arg.size > 0)
    {
        add_at_end(args, arg.data);
        add_at_end(sizes, arg.reserved_size);
        init(&arg);
    }

    add_at_end(args, (sys_char*)nullptr);
    *add_at_end(sizes) = 0;
}
#endif

template<typename C>
s64 _get_argument_count(const C **args)
{
    if (args == nullptr)
        return 0;

    s64 arg_count = 0;

    while (*args != nullptr)
    {
        args++;
        arg_count += 1;
    }

    return arg_count;
}

template<typename C>
void _args_to_cmdline(const C **args, string_base<C> *cmdline)
{
    s64 arg_count = _get_argument_count(args);

    if (arg_count > 0)
    {
        string_append(cmdline, string_literal(C, "\""));
        join(args, arg_count, string_literal(C, "\" \""), cmdline);
        string_append(cmdline, string_literal(C, "\""));
    }
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

void _free_process_start_info_path(process_start_info *info)
{
    if (info->_free_exe_path && info->path != nullptr)
    {
        dealloc((void*)info->path, info->_free_exe_path_size);
        info->path = nullptr;
    }
}

void _free_process_start_info_arguments(process_start_info *info)
{
    if (info->_free_args && info->args != nullptr)
    {
#if Windows
        assert(info->_free_args_sizes != nullptr);
        dealloc((void*)info->args, *info->_free_args_sizes);
        dealloc(info->_free_args_sizes, sizeof(s64));
#else
        s64 arg_count = _get_argument_count(info->args);
        array<sys_char*> _args{.data = (sys_char**)info->args, .size = arg_count, .reserved_size = arg_count, .allocator = {}};
        array<s64> _sizes{.data = (s64*)info->_free_args_sizes, .size = arg_count, .reserved_size = arg_count, .allocator = {}};

        for_array(i, sstr, &_args)
            if (*sstr != nullptr)
                dealloc(*sstr, info->_free_args_sizes[i]);

        free(&_args);
        free(&_sizes);
#endif

        info->args = nullptr;
    }
}

void free(process_start_info *info)
{
    assert(info != nullptr);

    _free_process_start_info_path(info);
    _free_process_start_info_arguments(info);
}

void process_create(process *p)
{
    assert(p != nullptr);

    fill_memory(p, 0);
    init(&p->start_info);
}

void process_destroy(process *p)
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

void set_process_executable(process *p, const c8 *exe)
{
    assert(p != nullptr);

    _free_process_start_info_path(&p->start_info);

#if Windows
    sys_string path{};
    string_set(&path, exe);
    p->start_info.path = path.data;
    p->start_info._free_exe_path = true;
    p->start_info._free_exe_path_size = path.reserved_size * sizeof(sys_char);
#else
    p->start_info.path = exe;
    p->start_info._free_exe_path = false;
#endif
}

void set_process_executable(process *p, const c16 *exe)
{
    assert(p != nullptr);

    _free_process_start_info_path(&p->start_info);

#if Windows
    p->start_info.path = (const wchar_t*)exe;
    p->start_info._free_exe_path = false;
#else
    sys_string path{};
    string_set(&path, exe);
    p->start_info.path = path.data;
    p->start_info._free_exe_path = true;
    p->start_info._free_exe_path_size = path.reserved_size * sizeof(sys_char);
#endif
}

void set_process_executable(process *p, const c32 *exe)
{
    assert(p != nullptr);

    _free_process_start_info_path(&p->start_info);

    sys_string path{};
    string_set(&path, exe);
    p->start_info.path = path.data;
    p->start_info._free_exe_path = true;
    p->start_info._free_exe_path_size = path.reserved_size * sizeof(sys_char);
}

void set_process_arguments(process *p, const c8 *args)
{
    assert(p != nullptr);

    _free_process_start_info_arguments(&p->start_info);

    if (args == nullptr)
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        return;
    }

#if Windows
    sys_string cmdline{};
    string_set(&cmdline, args);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = alloc<s64>();
    *p->start_info._free_args_sizes = cmdline.reserved_size * sizeof(sys_char);
#else
    array<sys_char*> _args{};
    array<s64> _sizes{};
    _cmdline_to_args(args, p->start_info.path, &_args, &_sizes);

    p->start_info.args = (const sys_char**)_args.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = _sizes.data;
#endif
}

void set_process_arguments(process *p, const c16 *args)
{
    assert(p != nullptr);

    _free_process_start_info_arguments(&p->start_info);

    if (args == nullptr)
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        return;
    }

#if Windows
    // we need to copy this because args must be modifiable. thanks windows.
    sys_string cmdline = copy_string(args);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = alloc<s64>();
    *p->start_info._free_args_sizes = cmdline.reserved_size * sizeof(sys_char);
#else
    sys_string sargs{};
    string_set(&sargs, args);
    array<sys_char*> _args{};
    array<s64> _sizes{};
    _cmdline_to_args(sargs.data, p->start_info.path, &_args, &_sizes);

    p->start_info.args = (const sys_char**)_args.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = _sizes.data;
    dealloc(sargs.data, sargs.reserved_size);
#endif
}

void set_process_arguments(process *p, const c32 *args)
{
    assert(p != nullptr);

    _free_process_start_info_arguments(&p->start_info);

    if (args == nullptr)
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        return;
    }

#if Windows
    sys_string cmdline{};
    string_set(&cmdline, args);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = alloc<s64>();
    *p->start_info._free_args_sizes = cmdline.reserved_size * sizeof(sys_char);
#else
    sys_string sargs{};
    string_set(&sargs, args);
    array<sys_char*> _args{};
    array<s64> _sizes{};
    _cmdline_to_args(sargs.data, p->start_info.path, &_args, &_sizes);

    p->start_info.args = (const sys_char**)_args.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = _sizes.data;
    dealloc(sargs.data, sargs.reserved_size);
#endif
}

void set_process_arguments(process *p, const c8 **args, [[maybe_unused]] bool raw)
{
    assert(p != nullptr);

#if Windows
    if (args == nullptr)
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        return;
    }

    string _cmdline{};
    _args_to_cmdline(args, &_cmdline);

    sys_string cmdline{};
    string_set(&cmdline, &_cmdline);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = alloc<s64>();
    *p->start_info._free_args_sizes = cmdline.reserved_size * sizeof(sys_char);

    free(&_cmdline);
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
        array<s64> _sizes{};
        u64 arg_count = _get_argument_count(args);

        // we add the name of the executable
        const c8 *exe_name = _get_exe_name(p->start_info.path);

        if (exe_name != nullptr)
        {
            string e = string_copy(exe_name);
            add_at_end(&_args, e.data);
            add_at_end(&_sizes, e.reserved_size);
        }

        for (u64 i = 0; i < arg_count; ++i)
        {
            string e = string_copy(args[i]);
            add_at_end(&_args, e.data);
            add_at_end(&_sizes, e.reserved_size);
        }

        if (_args.data != nullptr)
        {
            add_at_end(&_args, (c8 *)nullptr);
            *add_at_end(&_sizes) = 0;

            p->start_info.args = (const sys_char**)_args.data;
            p->start_info._free_args = true;
            p->start_info._free_args_sizes = _sizes.data;
        }
        else
        {
            p->start_info.args = nullptr;
            p->start_info._free_args = false;
        }
    }
#endif
}

void set_process_arguments(process *p, const c16 **args)
{
    assert(p != nullptr);

#if Windows
    if (args == nullptr)
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        return;
    }

    sys_string cmdline{};
    _args_to_cmdline(args, &cmdline);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = alloc<s64>();
    *p->start_info._free_args_sizes = cmdline.reserved_size * sizeof(sys_char);
#else
    array<sys_char*> _args{};
    array<s64> _args_sizes{};
    u64 arg_count = _get_argument_count(args);

    const char *exe_name = _get_exe_name(p->start_info.path);

    if (exe_name != nullptr)
        add_at_end(&_args, strdup(exe_name));

    for (u64 i = 0; i < arg_count; ++i)
    {
        sys_string tmp{};
        string_set(&tmp, args[i]);
        add_at_end(&_args, tmp.data);
        add_at_end(&_args_sizes, tmp.reserved_size);
    }

    if (_args.data != nullptr)
    {
        add_at_end(&_args, (char *)nullptr);

        p->start_info.args = (const sys_char**)_args.data;
        p->start_info._free_args = true;
        p->start_info._free_args_sizes = _args_sizes.data;
    }
    else
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        p->start_info._free_args_sizes = nullptr;
    }
#endif
}

void set_process_arguments(process *p, const c32 **args)
{
    assert(p != nullptr);

#if Windows
    if (args == nullptr)
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        return;
    }

    u32string _cmdline{};
    _args_to_cmdline(args, &_cmdline);

    sys_string cmdline{};
    string_set(&cmdline, &_cmdline);
    p->start_info.args = cmdline.data;
    p->start_info._free_args = true;
    p->start_info._free_args_sizes = alloc<s64>();
    *p->start_info._free_args_sizes = cmdline.reserved_size * sizeof(sys_char);

    free(&_cmdline);
#else
    array<sys_char*> _args{};
    array<s64> _args_sizes{};
    u64 arg_count = _get_argument_count(args);

    const char *exe_name = _get_exe_name(p->start_info.path);

    if (exe_name != nullptr)
        add_at_end(&_args, strdup(exe_name));

    for (u64 i = 0; i < arg_count; ++i)
    {
        sys_string tmp{};
        string_set(&tmp, args[i]);
        add_at_end(&_args, tmp.data);
        add_at_end(&_args_sizes, tmp.reserved_size);
    }

    if (_args.data != nullptr)
    {
        add_at_end(&_args, (char *)nullptr);

        p->start_info.args = (const sys_char**)_args.data;
        p->start_info._free_args = true;
        p->start_info._free_args_sizes = _args_sizes.data;
    }
    else
    {
        p->start_info.args = nullptr;
        p->start_info._free_args = false;
        p->start_info._free_args_sizes = nullptr;
    }
#endif
}

void set_process_io(process *p, io_handle in, io_handle out, io_handle err_out)
{
    assert(p != nullptr);

#if Windows
    p->start_info.inherit_handles = true;
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

bool process_start(process *p, error *err)
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
    int id = fork();

    if (id < 0)
    {
        set_error_by_code(err, -id);
        return false;
    }

    switch (id)
    {
    case 0:
    {
        // child process
        if (int nfd = dup2(p->start_info.detail.std_in,  0); nfd < 0) exit_group(-nfd);
        if (int nfd = dup2(p->start_info.detail.std_out, 1); nfd < 0) exit_group(-nfd);
        if (int nfd = dup2(p->start_info.detail.std_err, 2); nfd < 0) exit_group(-nfd);

        int ret = execve((char*)p->start_info.path, (char**)p->start_info.args, (char**)p->start_info.environment);

        // on success, execve does not return to here.
        exit_group(-ret);
    }
    default:
    {
        // parent process
        p->detail.pid = id;
        break;
    }
    }

#endif

    return true;
}

bool process_stop(process *p, error *err)
{
    assert(p != nullptr);

#if Windows
    if (!TerminateProcess(p->detail.hProcess, 0))
    {
        set_GetLastError_error(err);
        return false;
    }
#else
    if (!process_stop(p->detail.pid, err))
        return false;
#endif

    p->detail = {};

    return true;
}

bool process_stop(int pid, error *err)
{
#if Windows
    HANDLE h = OpenProcess(PROCESS_TERMINATE, true, pid);

    if (h == nullptr || h == INVALID_HANDLE_VALUE)
    {
        set_GetLastError_error(err);
        return false;
    }

    if (!TerminateProcess(h, 0))
    {
        set_GetLastError_error(err);
        CloseHandle(h);
        return false;
    }

    CloseHandle(h);

#else
    // let's not kill ourselves
    if (pid == 0)
    {
        if (err) err->error_code = EINVAL;
        return false;
    }

    if (s64 ret = kill(pid, SIGKILL); ret < 0)
    {
        set_error_by_code(err, -ret);
        return false;
    }
#endif

    return true;
}

int get_process_id()
{
#if Windows
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

// given process
int get_process_id(const process *p)
{
#if Windows
    return GetProcessId((HANDLE)p->detail.hProcess);
#else
    return p->detail.pid;
#endif
}

int get_parent_process_id()
{
#if Windows
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry{};

    int current_pid = get_process_id();
    
    if (!Process32First(snapshot, &entry))
    {
        CloseHandle(snapshot);
        return -1;
    }

    int tries = 0;

    while (entry.th32ProcessID != current_pid && tries < 100)
    {
        if (!Process32Next(snapshot, &entry))
        {
            CloseHandle(snapshot);
            return -1;
        }

        tries++;
    }

    CloseHandle(snapshot);

    if (entry.th32ProcessID != current_pid)
        return -1;

    return entry.th32ParentProcessID;
#else
    return getppid();
#endif
}
