
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

define_test(is_space_returns_true_if_character_is_whitespace)
{
    assert_equal(is_space(' '), true);
    assert_equal(is_space('\t'), true);
    assert_equal(is_space('\n'), true);
    assert_equal(is_space('\r'), true);
    assert_equal(is_space('\v'), true);
}

define_test(is_space_returns_true_if_wide_character_is_whitespace)
{
    assert_equal(is_space(L' '), true);
    assert_equal(is_space(L'\t'), true);
    assert_equal(is_space(L'\n'), true);
}

define_test(is_space_returns_false_if_character_is_not_whitespace)
{
    assert_equal(is_space('\0'), false);
    assert_equal(is_space('a'),  false);
    assert_equal(is_space('Z'),  false);
    assert_equal(is_space('0'),  false);
    assert_equal(is_space('9'),  false);
    assert_equal(is_space('~'),  false);
}

define_test(is_space_returns_false_if_wide_character_is_not_whitespace)
{
    assert_equal(is_space(L'\0'), false);
    assert_equal(is_space(L'a'),  false);
    assert_equal(is_space(L'!'),  false);
}

define_test(is_newline_returns_true_if_character_is_newline)
{
    assert_equal(is_newline('\n'), true);
    assert_equal(is_newline('\f'), true);
    assert_equal(is_newline('\v'), true);
}

define_test(is_newline_returns_false_if_character_is_not_newline)
{
    assert_equal(is_newline(' '),  false);
    assert_equal(is_newline('\t'), false);
    assert_equal(is_newline('a'),  false);
}

define_test(is_newline_returns_true_if_wide_character_is_newline)
{
    assert_equal(is_newline(L'\n'), true);
    assert_equal(is_newline(L'\f'), true);
    assert_equal(is_newline(L'\v'), true);
}

define_test(is_newline_returns_false_if_wide_character_is_not_newline)
{
    assert_equal(is_newline(L' '),  false);
    assert_equal(is_newline(L'\t'), false);
    assert_equal(is_newline(L'a'),  false);
}

define_test(is_blank_returns_true_if_string_is_empty)
{
    assert_equal(is_blank(str("")), true);
    assert_equal(is_blank(wstr(L"")), true);
}

define_test(is_blank_returns_true_if_string_is_whitespaces)
{
    assert_equal(is_blank(str(" ")), true);
    assert_equal(is_blank(str("     ")), true);
    assert_equal(is_blank(str("  \t  ")), true);
    assert_equal(is_blank(str("  \r\n  ")), true);
    assert_equal(is_blank(wstr(L" ")), true);
    assert_equal(is_blank(wstr(L"     ")), true);
    assert_equal(is_blank(wstr(L"  \t  ")), true);
    assert_equal(is_blank(wstr(L"  \r\n  ")), true);
}

define_test(is_blank_returns_false_if_string_contains_non_whitespaces)
{
    assert_equal(is_blank(str("a")), false);
    assert_equal(is_blank(str("   def  ")), false);
    assert_equal(is_blank(wstr(L"a")), false);
    assert_equal(is_blank(wstr(L"   def  ")), false);
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

define_test(trim_trims_leftmost_and_rightmost_whitespaces_from_wstring)
{
    std::wstring a = L"  ab c  ";
    trim(a);
    assert_equal(a, L"ab c");
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

define_default_test_main();
