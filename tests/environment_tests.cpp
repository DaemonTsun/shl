
#include "t1/t1.hpp"

#include "shl/string.hpp"
#include "shl/environment.hpp"

define_test(set_environment_variable_adds_new_environment_variable)
{
    const sys_native_char *var = (const sys_native_char*)get_environment_variable(SYS_CHAR("SETVAR_SETS_ME"));
    assert_equal(var, nullptr);

    assert_equal(set_environment_variable(SYS_CHAR("SETVAR_SETS_ME"), SYS_CHAR("abc")), true);

    var = (const sys_native_char*)get_environment_variable(SYS_CHAR("SETVAR_SETS_ME"));
    assert_not_equal(var, nullptr);
    assert_equal(string_compare((const sys_char*)var, SYS_CHAR("abc")), 0);
}

define_test(get_environment_variable_returns_nullptr_when_variable_does_not_exist)
{
    const sys_native_char *var = (const sys_native_char*)get_environment_variable(SYS_CHAR("HOMEABC"));
    assert_equal(var, nullptr);
}

define_default_test_main();

