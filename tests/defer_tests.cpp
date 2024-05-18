
#define t1_use_stdlib 1
#include <t1/t1.hpp>

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

define_default_test_main();
