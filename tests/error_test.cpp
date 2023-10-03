
#include <string.h>
#include <t1/t1.hpp>

#include "shl/error.hpp"

define_test(format_error_formats_error)
{
    const char *err = format_error("abc %d def", 123);

    assert_equal(strcmp(err, "abc 123 def"), 0);
}

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

define_test(get_error_gets_error)
{
    error err;

    get_error(&err, "xyz %d uvw", 789);

    assert_equal(strcmp(err.what, "xyz 789 uvw"), 0);
    assert_equal(err.line, 31);
}

define_test(get_error_does_nothing_on_nullptr)
{
    error *err = nullptr;
    get_error(err, "xyz %d uvw", 789);
    get_error(err, "xyz %d uvw", 789);
    get_error(err, "xyz %d uvw", 789);

    assert_equal(err, nullptr);
}

define_default_test_main();
