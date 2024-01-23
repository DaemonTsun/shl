
#include <t1/t1.hpp>
#include <stdlib.h>

#include "shl/process.hpp"
#include "shl/string.hpp"
#include "shl/array.hpp"

#define WIN_CMD  L"C:\\Windows\\System32\\cmd.exe"
#define UNIX_ECHO "/usr/bin/echo"

define_test(process_arguments_test)
{
    process p;
    init(&p);

    set_process_arguments(&p, R"=(echo "hello world"   a\\b\"c\")=");

#if Windows
    assert_equal(compare_strings(p.start_info.args, LR"=(echo "hello world"   a\\b\"c\")="), 0);
#else
    assert_equal(compare_strings(p.start_info.args[0], "echo"), 0);
    assert_equal(compare_strings(p.start_info.args[1], R"(hello world)"), 0);
    assert_equal(compare_strings(p.start_info.args[2], R"(a\b"c")"), 0);
    assert_equal(p.start_info.args[3], nullptr);
#endif

    free(&p.start_info);

    // ON LINUX: if the executable path is set, the name of the executable
    // is preprended to be the first argument of the argument list.
    // Does not apply to Windows.
    set_process_executable(&p, "/usr/bin/echo");
    set_process_arguments(&p, R"=("hello world"   a\\b\"c\")=");

#if Windows
    assert_equal(compare_strings(p.start_info.args, LR"=("hello world"   a\\b\"c\")="), 0);
#else
    assert_equal(compare_strings(p.start_info.args[0], "echo"), 0);
    assert_equal(compare_strings(p.start_info.args[1], R"(hello world)"), 0);
    assert_equal(compare_strings(p.start_info.args[2], R"(a\b"c")"), 0);
    assert_equal(p.start_info.args[3], nullptr);
#endif

    free(&p.start_info);
}

define_test(process_test)
{
    process p;
    init(&p);
    error err{};

    bool ok;

#if Windows
    set_process_executable(&p, WIN_CMD);
    set_process_arguments(&p, L"/c echo hello world");
#else
    set_process_executable(&p, UNIX_ECHO);
    set_process_arguments(&p, "hello world");
#endif

    ok = start_process(&p, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);

    free(&p);
    init(&p);

#if Windows
    set_process_executable(&p, WIN_CMD);

    // when using args, last arg must be nullptr
    const sys_char *args[] = {
        L"/c",
        L"echo",
        L"hello world",
        nullptr
    }; 

    set_process_arguments(&p, args);
#else
    set_process_executable(&p, UNIX_ECHO);

    // exe name is prepended automatically, unless "raw" argument in
    // set_process_arguments is set to true.
    const sys_char *args[] = {
        "hello world",
        nullptr
    }; 

    set_process_arguments(&p, args);
#endif

    ok = start_process(&p, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);

    free(&p);
}

define_test(process_pipe_test)
{
    process p{};
    error err{};

    init(&p);

    pipe out_pipe{};
    init(&out_pipe);

    set_handle_inheritance(out_pipe.read, false);

#if Windows
    const sys_char *cmd = WIN_CMD;
    const sys_char *args = L"/c echo world!";
#else
    const sys_char *cmd = UNIX_ECHO;
    const sys_char *args[] = {"world!", nullptr};
#endif

    set_process_executable(&p, cmd);
    set_process_arguments(&p, args);

    set_process_io(&p, stdin_handle(), out_pipe.write, out_pipe.write);

    bool ok = start_process(&p, &err);
    assert_equal(ok, true);
    assert_not_equal(get_pid(&p), 0);
    assert_not_equal(get_pid(&p), get_pid());

    char buf[64] = {};

    s64 bytes_read = io_read(out_pipe.read, (char*)buf, 63, &err);

    assert_equal(err.error_code, 0);

#if Windows
    assert_equal(bytes_read, 8);
    assert_equal(compare_strings(buf, "world!\r\n"), 0);
#else
    assert_equal(bytes_read, 7);
    assert_equal(compare_strings(buf, "world!\n"), 0);
#endif

    free(&out_pipe);
    free(&p);
}

define_test(stop_process_stops_process)
{
    process p{};
    error err{};
    init(&p);

#if Windows
    const sys_char *cmd = WIN_CMD;
    const sys_char *args = nullptr;
#else
    const sys_char *cmd = "/usr/bin/cat";
    const sys_char *args[] = {"-", nullptr};
#endif

    pipe pip{};
    init(&pip);
    set_process_io(&p, pip.read, stdout_handle(), stderr_handle());

    set_process_executable(&p, cmd);
    set_process_arguments(&p, args);

    assert_equal(start_process(&p, &err), true);
    assert_equal(stop_process(&p, &err), true);
    assert_equal(err.error_code, 0);
    assert_equal(stop_process(&p, &err), false);
    assert_not_equal(err.error_code, 0);

    free(&pip);
    free(&p);
}

define_default_test_main();
