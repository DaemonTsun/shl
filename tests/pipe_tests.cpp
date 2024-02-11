
#include <t1/t1.hpp>

#include "shl/streams.hpp"
#include "shl/pipe.hpp"

define_test(init_initializes_new_pipe)
{
    pipe_t p{};
    error err{};

    assert_equal(init(&p, &err), true);
    assert_equal(err.error_code, 0);

#if Windows
    assert_not_equal(p.read, nullptr);
    assert_not_equal(p.write, nullptr);
#endif

    assert_equal(io_is_pipe(p.read), true);
    assert_equal(io_is_pipe(p.write), true);
    assert_equal(io_size(p.read, &err), 0);
    // should not be able to do that
    // assert_equal(io_size(p.write, &err), 0);

    assert_equal(free(&p, &err), true);
}

define_test(io_read_reads_from_pipe)
{
    pipe_t p{};
    error err{};

    assert_equal(init(&p, &err), true);
    assert_equal(err.error_code, 0);

    char buf[64] = {0};
    s64 bytes = 0;

    assert_equal(io_size(p.read, &err), 0);
    assert_equal(io_write(p.write, "hello", 5, &err), 5);
    assert_equal(io_size(p.read, &err), 5);

    assert_equal(io_poll_read(p.read), true);
    assert_equal(io_size(p.read, &err), 5);

    bytes = io_read(p.read, buf, 63, &err);

    assert_equal(bytes, 5);
    assert_equal(io_size(p.read, &err), 0);
    assert_equal(compare_strings(buf, "hello"), 0);

    assert_equal(io_poll_read(p.read), false);
    // bytes = io_read(p.read, buf, 63, &err);

    assert_equal(free(&p, &err), true);
}

define_test(read_entire_pipe_reads_everything_in_pipe)
{
    pipe_t p{};
    error err{};
    string contents{};

    init(&p);

    const char *str = "hello world! this is a long string";
    assert_equal(write(&p, str), 34);

    assert_equal(read_entire_pipe(&p, &contents, &err), true);
    assert_equal(contents.size, 34);
    assert_equal(contents.data[contents.size], '\0');
    assert_equal(compare_strings(contents, str), 0);

    // nothing left in pipe -> contents = empty
    assert_equal(read_entire_pipe(&p, &contents, &err), true);
    assert_equal(contents.size, 0);
    assert_equal(contents.data[contents.size], '\0');

    free(&p, &err);
    free(&contents);
}

define_default_test_main();
