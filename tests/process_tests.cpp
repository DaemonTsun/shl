
#include <t1/t1.hpp>
#include <stdlib.h>

#include "shl/process.hpp"
#include "shl/pipe.hpp"
#include "shl/string.hpp"
#include "shl/array.hpp"

#define WIN_CMD  SYS_UTF_CHAR("C:\\Windows\\System32\\cmd.exe")
#define UNIX_ECHO "/usr/bin/echo"

define_test(process_arguments_test)
{
    process p{};
    process_create(&p);

    set_process_arguments(&p, SYS_UTF_CHAR(R"=(echo "hello world"   a\\b\"c\")="));

#if Windows
    assert_equal(string_compare((const sys_utf_char*)p.start_info.args, SYS_UTF_CHAR(R"=(echo "hello world"   a\\b\"c\")=")), 0);
#else
    assert_equal(string_compare(p.start_info.args[0], "echo"), 0);
    assert_equal(string_compare(p.start_info.args[1], R"(hello world)"), 0);
    assert_equal(string_compare(p.start_info.args[2], R"(a\b"c")"), 0);
    assert_equal(p.start_info.args[3], nullptr);
#endif

    free(&p.start_info);

    // ON LINUX: if the executable path is set, the name of the executable
    // is preprended to be the first argument of the argument list.
    // Does not apply to Windows.
    set_process_executable(&p, SYS_UTF_CHAR("/usr/bin/echo"));
    set_process_arguments(&p, SYS_UTF_CHAR(R"=("hello world"   a\\b\"c\")="));

#if Windows
    int res = string_compare((const sys_utf_char*)p.start_info.args, SYS_UTF_CHAR(R"=("hello world"   a\\b\"c\")="));
    assert_equal(res, 0);
#else
    assert_equal(string_compare(p.start_info.args[0], "echo"), 0);
    assert_equal(string_compare(p.start_info.args[1], R"(hello world)"), 0);
    assert_equal(string_compare(p.start_info.args[2], R"(a\b"c")"), 0);
    assert_equal(p.start_info.args[3], nullptr);
#endif

    free(&p.start_info);
}

define_test(process_test)
{
    process p;
    process_create(&p);
    error err{};

    bool ok;

#if Windows
    set_process_executable(&p, WIN_CMD);
    set_process_arguments(&p, SYS_UTF_CHAR("/c echo hello world"));
#else
    set_process_executable(&p, UNIX_ECHO);
    set_process_arguments(&p, "hello world");
#endif

    ok = process_start(&p, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);

    process_destroy(&p);
    process_create(&p);

#if Windows
    set_process_executable(&p, WIN_CMD);

    // when using args, last arg must be nullptr
    const c16 *args[] = {
        u"/c",
        u"echo",
        u"hello world",
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

    ok = process_start(&p, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);

    process_destroy(&p);
}

define_test(process_pipe_test)
{
    process p{};
    process_create(&p);

    error err{};

    pipe_t out_pipe{};
    init(&out_pipe);

    set_handle_inheritance(out_pipe.read, false);

#if Windows
    const sys_utf_char *cmd = WIN_CMD;
    const sys_utf_char *args = SYS_UTF_CHAR("/c echo world!");
#else
    const sys_utf_char *cmd = UNIX_ECHO;
    const sys_utf_char *args[] = {"world!", nullptr};
#endif

    set_process_executable(&p, cmd);
    set_process_arguments(&p, args);

    set_process_io(&p, stdin_handle(), out_pipe.write, out_pipe.write);

    bool ok = process_start(&p, &err);
    assert_equal(ok, true);
    assert_not_equal(get_process_id(&p), 0);
    assert_not_equal(get_process_id(&p), get_process_id());

    char buf[64] = {};
    s64 bytes_read = io_read(out_pipe.read, (char*)buf, 63, &err);

    assert_equal(err.error_code, 0);

#if Windows
    assert_equal(bytes_read, 8);
    assert_equal(string_compare(buf, "world!\r\n"), 0);
#else
    assert_equal(bytes_read, 7);
    assert_equal(string_compare(buf, "world!\n"), 0);
#endif

    free(&out_pipe);
    process_destroy(&p);
}

define_test(process_stop_stops_process)
{
    process p{};
    process_create(&p);

    error err{};

#if Windows
    const sys_utf_char *cmd = (const sys_utf_char*)WIN_CMD;
    const sys_utf_char *args = nullptr;
#else
    const sys_utf_char *cmd = "/usr/bin/cat";
    const sys_utf_char *args[] = {"-", nullptr};
#endif

    pipe_t pip{};
    init(&pip);
    set_process_io(&p, pip.read, stdout_handle(), stderr_handle());

    set_process_executable(&p, cmd);
    set_process_arguments(&p, args);

    assert_equal(process_start(&p, &err), true);
    assert_equal(process_stop(&p, &err), true);
    assert_equal(err.error_code, 0);
    assert_equal(process_stop(&p, &err), false);
    assert_not_equal(err.error_code, 0);

    free(&pip);
    process_destroy(&p);
}

define_default_test_main();
