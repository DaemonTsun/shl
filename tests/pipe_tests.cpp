
#include <t1/t1.hpp>
#include <stdlib.h>

#include "shl/string.hpp"
#include "shl/pipe.hpp"

define_test(init_initializes_new_pipe)
{
    pipe p{};
    error err{};

    assert_equal(init(&p, 0, true, &err), true);
    assert_equal(err.error_code, 0);

#if Windows
    assert_not_equal(p.read, nullptr);
    assert_not_equal(p.write, nullptr);
#endif

    assert_equal(free(&p, &err), true);
}

define_test(read_reads_from_pipe)
{
    pipe p{};
    error err{};

    assert_equal(init(&p, 0, true, &err), true);
    assert_equal(err.error_code, 0);

    char buf[64] = {0};
    s64 bytes = 0;

    assert_equal(write(&p, "hello", 5, &err), 5);

    bytes = read(&p, buf, 5, &err);

    assert_equal(bytes, 5);
    assert_equal(compare_strings(buf, "hello"), 0);

    assert_equal(free(&p, &err), true);
}


define_default_test_main();
