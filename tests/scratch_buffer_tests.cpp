
#include <t1/t1.hpp>

#include "shl/scratch_buffer.hpp"

define_test(init_initializes_scratch_buffer)
{
    constexpr u64 Size = 64;
    scratch_buffer<Size> buf;

    init(&buf);

    assert_not_equal(buf.data, nullptr);
    assert_not_equal(buf.size, 0);

    assert_equal(buf.data, buf.stack_buffer);
    assert_equal(buf.size, Size);

    free(&buf);
}

define_test(grow_grows_scratch_buffer)
{
    constexpr u64 Size = 64;
    scratch_buffer<Size> buf;

    init(&buf);

    assert_equal(buf.data, buf.stack_buffer);
    assert_equal(buf.size, Size);

    grow(&buf);

    assert_greater(buf.size, Size);
    assert_not_equal(buf.data, nullptr);
    assert_not_equal(buf.data, buf.stack_buffer);

    grow(&buf);

    assert_greater(buf.size, Size);
    assert_not_equal(buf.data, nullptr);
    assert_not_equal(buf.data, buf.stack_buffer);

    free(&buf);
}

define_test(grow_by_grows_scratch_buffer_by_factor)
{
    constexpr u64 Size = 64;
    scratch_buffer<Size> buf;

    init(&buf);

    assert_equal(buf.data, buf.stack_buffer);
    assert_equal(buf.size, Size);

    grow_by(&buf, 4);

    assert_equal(buf.size, 256);
    assert_not_equal(buf.data, nullptr);
    assert_not_equal(buf.data, buf.stack_buffer);

    grow_by(&buf, 4);

    assert_equal(buf.size, 1024);
    assert_not_equal(buf.data, nullptr);
    assert_not_equal(buf.data, buf.stack_buffer);

    free(&buf);
}

define_default_test_main()
