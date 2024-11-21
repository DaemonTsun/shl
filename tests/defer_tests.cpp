
#include <t1/t1.hpp>

#undef defer

#include "shl/memory.hpp"
#include "shl/print.hpp"
#include "shl/defer.hpp"

define_test(file_stream_init_initializes)
{
    int x = 0;
    assert_equal(x, 0);

    defer { x = 1; };

    assert_equal(x, 0);

    {
        defer { x = 2; };
    }

    assert_equal(x, 2);

    defer { x = 3; };

    assert_equal(x, 2);

    // x would be 3 after this block
}

void some_func()
{
    static int *x = nullptr;

    // x gets allocated and assigned exactly once, no matter how many times the
    // function is called.
    static_exec {
        tprint("some_func: static_exec called\n");
        x = alloc<int>();
        *x = 13;
    };

    // x gets freed exactly once.
    static_defer {
        tprint("some_func: static_defer called\n");
        dealloc(x);
        x = nullptr;
    };

    assert_not_equal(x, nullptr);
    assert_equal(*x, 13);
}

define_test(static_defer_and_static_exec_test)
{
    some_func();
    some_func();
    some_func();
}

define_default_test_main();
