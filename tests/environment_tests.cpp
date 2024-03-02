
#include <t1/t1.hpp>

#include "shl/string.hpp"
#include "shl/environment.hpp"

define_test(set_environment_variable_adds_new_environment_variable)
{
    const sys_char *var = get_environment_variable("SETVAR_SETS_ME");
    assert_equal(var, nullptr);

    assert_equal(set_environment_variable("SETVAR_SETS_ME", "abc"), true);

    var = get_environment_variable("SETVAR_SETS_ME");
    assert_not_equal(var, nullptr);
    assert_equal(compare_strings(var, "abc"), 0);
}

define_test(get_environment_variable_returns_nullptr_when_variable_does_not_exist)
{
    const sys_char *var = get_environment_variable("HOMEABC");
    assert_equal(var, nullptr);
}

define_default_test_main();

