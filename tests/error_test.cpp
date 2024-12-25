
#include <t1/t1.hpp>

#include "shl/string.hpp"
#include "shl/error.hpp"

define_test(format_error_message_formats_error_message)
{
    const char *err = format_error_message("abc %d def", 123);

    assert_equal(strcmp(err, "abc 123 def"), 0);
}

/* Deprecated as of 0.8.5. Don't throw away your errors.
define_test(throw_error_throws_error)
{
    try
    {
        throw_error("abc %d def", 567);
    }
    catch (error &err)
    {
        assert_equal(strcmp(err.what, "abc 567 def"), 0);
        assert_equal(err.line, 18);
    }
}
*/

define_test(set_error_sets_error_message)
{
    error err{};
    const char *msg = "hello world";

    set_error(&err, 123, msg);

    assert_equal(strcmp(err.what, "hello world"), 0);
    // directly sets the message to the pointer
    assert_equal(err.what, msg);
    assert_equal(err.error_code, 123);
#if Debug
    assert_equal(err.line, 34);
#endif
}

define_test(format_error_formats_error)
{
    error err{};

    format_error(&err, 123, "xyz %d uvw", 789);

    assert_equal(strcmp(err.what, "xyz 789 uvw"), 0);
    assert_equal(err.error_code, 123);
#if Debug
    assert_equal(err.line, 47);
#endif
}

define_test(format_error_does_nothing_on_nullptr)
{
    error *err = nullptr;
    format_error(err, 123, "xyz %d uvw", 789);
    format_error(err, 123, "xyz %d uvw", 789);
    format_error(err, 123, "xyz %d uvw", 789);

    assert_equal(err, nullptr);
}

define_test(set_error_sets_multiple_errors)
{
    error err1{};
    error err2{};
    set_error(&err1, 1, "hello");
    set_error(&err2, 2, "world");

    assert_equal(err1.what, "hello");
    assert_equal(err1.error_code, 1);
    assert_equal(err2.what, "world");
    assert_equal(err2.error_code, 2);
}

define_test(format_error_sets_multiple_errors)
{
    error err1{};
    error err2{};
    format_error(&err1, 1, "hello %d", 10);
    format_error(&err2, 2, "world %d", 20);

    assert_equal(to_const_string(err1.what), "hello 10"_cs);
    assert_equal(to_const_string(err2.what), "world 20"_cs);
}

#ifndef NDEBUG
define_test(set_error_keeps_trace)
{
    error err{};

    set_error(&err, 5, "a");
    set_error(&err, 6, "b");

    // since err is not cleared, err is the deepest error set
    assert_equal(to_const_string(err.what), "a"_cs);
    assert_equal(err.error_code, 5);

    assert_not_equal(err.trace, nullptr);
    
    // probably don't do this, just for test
    error *err_trace = (error*)((error*)err.trace)->trace;
    assert_equal(to_const_string(err_trace->what), "b"_cs);
    assert_equal(err_trace->error_code, 6);

    for_error_trace(frame, e, &err)
    {
        printf("[%lld][%s:%lu %s] %d: %s\n", frame, e->file, e->line, e->function, e->error_code, e->what);

        if (frame == 0)
        {
            assert_equal(e->what, "a");
            assert_equal(e->error_code, 5);
        }
        else if (frame == 1)
        {
            assert_equal(e->what, "b");
            assert_equal(e->error_code, 6);
        }
    }
}
#endif

define_default_test_main();
