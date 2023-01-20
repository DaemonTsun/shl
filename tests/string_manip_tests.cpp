
#include <iostream>

#include <t1/t1.hpp>
#include "shl/string_manip.hpp"

using namespace std::literals;

define_test(to_upper_converts_to_upper)
{
    assert_equal(to_upper('a'), 'A');
    assert_equal(to_upper('z'), 'Z');
    assert_equal(to_upper(' '), ' ');
    assert_equal(to_upper(L'a'), L'A');
    assert_equal(to_upper(L'z'), L'Z');
    assert_equal(to_upper(L'!'), L'!');

    std::string s = "hello world"s;
    to_upper(s.data());
    assert_equal(s, "HELLO WORLD"s);

    std::wstring ws = L"hello world"s;
    to_upper(ws.data());
    assert_equal(ws, L"HELLO WORLD"s);
}

define_test(to_lower_converts_to_lower)
{
    assert_equal(to_lower('A'), 'a');
    assert_equal(to_lower('Z'), 'z');
    assert_equal(to_lower(' '), ' ');
    assert_equal(to_lower(L'A'), L'a');
    assert_equal(to_lower(L'Z'), L'z');
    assert_equal(to_lower(L'!'), L'!');

    std::string s = "HELLO WORLD"s;
    to_lower(s.data());
    assert_equal(s, "hello world"s);

    std::wstring ws = L"HELLO WORLD"s;
    to_lower(ws.data());
    assert_equal(ws, L"hello world"s);
}

define_test(trim_left_trims_leftmost_whitespaces_from_string)
{
    std::string a = "  ab c  ";
    trim_left(a);
    assert_equal(a, "ab c  "s);
    trim_left(a);
    assert_equal(a, "ab c  "s);
}

define_test(trim_right_trims_rightmost_whitespaces_from_string)
{
    std::string a = "  ab c  ";
    trim_right(a);
    assert_equal(a, "  ab c"s);
    trim_right(a);
    assert_equal(a, "  ab c"s);
}

define_test(trim_trims_leftmost_and_rightmost_whitespaces_from_string)
{
    std::string a = "  ab c  ";
    trim(a);
    assert_equal(a, "ab c"s);
    trim(a);
    assert_equal(a, "ab c"s);
}

define_test(trim_trims_leftmost_and_rightmost_whitespaces_from_wstring)
{
    std::wstring a = L"  ab c  ";
    trim(a);
    assert_equal(a, L"ab c"s);
}

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
