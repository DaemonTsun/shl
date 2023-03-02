
#include <iostream>

#include <t1/t1.hpp>
#include "shl/string_manip.hpp"

define_test(to_string_constructs_std_string)
{
    assert_equal(to_string(""), std::string(""));
    assert_equal(to_string("abc"), std::string("abc"));
}

define_test(to_wstring_constructs_std_wstring)
{
    assert_equal(to_wstring(L""), std::wstring(L""));
    assert_equal(to_wstring(L"abc"), std::wstring(L"abc"));
}

define_test(to_string_concatenates_arguments_to_form_single_std_string)
{
    assert_equal(to_string("hello", "world"), std::string("helloworld"));
    assert_equal(to_string("hello", ""), std::string("hello"));
    assert_equal(to_string("", "world"), std::string("world"));
    assert_equal(to_string("", "abc", "", "", "def", ""), std::string("abcdef"));
}

define_test(to_wstring_concatenates_arguments_to_form_single_std_wstring)
{
    assert_equal(to_wstring("hello", L"world"), std::wstring(L"helloworld"));
    assert_equal(to_wstring(L"hello", L""), std::wstring(L"hello"));
    assert_equal(to_wstring("", "world"), std::wstring(L"world"));
    assert_equal(to_wstring("", L"abc", L"", "", "def", ""), std::wstring(L"abcdef"));
}

define_default_test_main();
