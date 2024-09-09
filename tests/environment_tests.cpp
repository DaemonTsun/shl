
#include "t1/t1.hpp"

#include "shl/string.hpp"
#include "shl/environment.hpp"

using sys_utf_char = if_type(is_same(sys_char, wchar_t), wc_utf_type, sys_char);
#define SYS_UTF_CHAR(x) ((const sys_utf_char*)SYS_CHAR(x))

define_test(set_environment_variable_adds_new_environment_variable)
{
    const sys_char *var = get_environment_variable(SYS_CHAR("SETVAR_SETS_ME"));
    assert_equal(var, nullptr);

    assert_equal(set_environment_variable(SYS_CHAR("SETVAR_SETS_ME"), SYS_CHAR("abc")), true);

    var = get_environment_variable(SYS_CHAR("SETVAR_SETS_ME"));
    assert_not_equal(var, nullptr);
    assert_equal(string_compare((const sys_utf_char*)var, SYS_UTF_CHAR("abc")), 0);
}

define_test(get_environment_variable_returns_nullptr_when_variable_does_not_exist)
{
    const sys_char *var = get_environment_variable(SYS_CHAR("HOMEABC"));
    assert_equal(var, nullptr);
}

define_default_test_main();

