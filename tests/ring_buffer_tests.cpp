
#include <t1/t1.hpp>

#include "shl/ring_buffer.hpp"

define_test(init_initializes_ring_buffer)
{
    error err{};
    ring_buffer buf;

    assert_equal(init(&buf, 1024, 3, &err), true);

    assert_not_equal(buf.data, nullptr);
    assert_not_equal(buf.size, 0);
    assert_greater_or_equal(buf.size, 1024);
    assert_equal(buf.mapping_count, 3);

    assert_equal(free(&buf, &err), true);

    assert_equal(err.error_code, 0);
}

define_test(access_within_ring_buffer_repeats)
{
    error err{};
    ring_buffer buf;

    assert_equal(init(&buf, 1024, 3, &err), true);

    buf.data[0] = 'a';

    // buf.size is not guaranteed to be 1024 here,
    // buf.size depends on the pagesize of the system,
    // so it's better to use buf.size, not 1024.
    assert_equal(buf.data[0],            'a');
    assert_equal(buf.data[buf.size],     'a');
    assert_equal(buf.data[buf.size * 2], 'a');

    assert_equal(free(&buf, &err), true);

    assert_equal(err.error_code, 0);
}

define_default_test_main()
