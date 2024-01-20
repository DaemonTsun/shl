
#include <t1/t1.hpp>
#include <stdlib.h>

#include "shl/process.hpp"
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

define_default_test_main();
