
#include <iostream>

#include <t1/t1.hpp>
#include "../src/string.hpp"

using namespace std::literals;

define_test(str_constructs_std_string)
{
    assert_equal(str(""), std::string(""));
    assert_equal(str("abc"), std::string("abc"));
}

define_test(wstr_constructs_std_wstring)
{
    assert_equal(wstr(L""), std::wstring(L""));
    assert_equal(wstr(L"abc"), std::wstring(L"abc"));
}

define_test(str_concatenates_arguments_to_form_single_std_string)
{
    assert_equal(str("hello", "world"), std::string("helloworld"));
    assert_equal(str("hello", ""), std::string("hello"));
    assert_equal(str("", "world"), std::string("world"));
    assert_equal(str("", "abc", "", "", "def", ""), std::string("abcdef"));
}

define_test(wstr_concatenates_arguments_to_form_single_std_wstring)
{
    assert_equal(wstr("hello", L"world"), std::wstring(L"helloworld"));
    assert_equal(wstr(L"hello", L""), std::wstring(L"hello"));
    assert_equal(wstr("", "world"), std::wstring(L"world"));
    assert_equal(wstr("", L"abc", L"", "", "def", ""), std::wstring(L"abcdef"));
}

define_test(ltrim_trims_leftmost_whitespaces_from_string)
{
    std::string a = "  ab c  ";
    ltrim(a);
    assert_equal(a, "ab c  ");
}

define_test(rtrim_trims_rightmost_whitespaces_from_string)
{
    std::string a = "  ab c  ";
    rtrim(a);
    assert_equal(a, "  ab c");
}

define_test(trim_trims_leftmost_and_rightmost_whitespaces_from_string)
{
    std::string a = "  ab c  ";
    trim(a);
    assert_equal(a, "ab c");
}

define_test(begins_with_returns_true_if_string_starts_with_prefix)
{
    assert_equal(begins_with("hello"s, "hell"s), true);
}

define_test(begins_with_returns_false_if_prefix_is_longer_than_string)
{
    assert_equal(begins_with("hello"s, "hellow"s), false);
}

define_test(ends_with_returns_true_if_string_ends_with_suffix)
{
    assert_equal(ends_with("hello"s, "ello"s), true);
}

define_test(ends_with_returns_false_if_suffix_is_longer_than_string)
{
    assert_equal(ends_with("hello"s, "whello"s), false);
}

define_test(repl_replaces_characters_in_std_string_and_returns_it)
{
    std::string str = "h3llo world";
    assert_equal((repl<'e', '3'>(str)), "hello world"s);
    assert_equal(str, "hello world"s);
}

define_default_test_main();
