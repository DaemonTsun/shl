
#include <t1/t1.hpp>
#include <stdlib.h>

#include "shl/process.hpp"
#include "shl/string.hpp"
#include "shl/array.hpp"

define_test(process_test)
{
    process p{};
    error err{};

#if Windows
    // when using args, last arg must be nullptr
    const wchar_t *args[] = {
        L"/c",
        L"echo",
        L"hello world",
        nullptr
    }; 

    bool ok = start_process(&p, L"C:\\Windows\\System32\\cmd.exe", args, nullptr, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);

    ok = start_process(&p, L"C:\\Windows\\System32\\cmd.exe", L"/c echo hello!", nullptr, &err);

    assert_equal(ok, true);
    assert_equal(err.error_code, 0);
#endif
}

define_test(process_pipe_test)
{
    process p{};
    error err{};

    pipe out_pip{};
    init(&out_pip);

    set_handle_inheritance(out_pip.read, false);

#if Windows
    const sys_char *cmd = L"C:\\Windows\\System32\\cmd.exe";
    const sys_char *args = L"/c echo world!";
#else
    const sys_char *cmd = "/usr/bin/echo";
    const sys_char *args[] = {"world!", nullptr};
#endif

    bool ok = start_process(&p, cmd, args, nullptr, nullptr, out_pip.write, out_pip.write, &err);
    assert_equal(ok, true);

    char buf[64] = {};

    s64 bytes_read = read(out_pip.read, (char*)buf, 63, &err);

    assert_equal(err.error_code, 0);
    assert_equal(bytes_read, 8);
    assert_equal(compare_strings(buf, "world!\r\n"), 0);

    free(&out_pip);
}

define_default_test_main();
