
#include <string.h>
#include <t1/t1.hpp>

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

define_default_test_main();
