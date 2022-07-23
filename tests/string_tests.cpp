
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

define_test(to_integer_converts_to_int)
{
    assert_equal(to_integer<int>("0"s), 0);
    assert_equal(to_integer<int>("1234"s), 1234);
    assert_equal(to_integer<int>("2147483647"s), std::numeric_limits<int>::max()); // may be different on some platforms
    assert_equal(to_integer<int>("-2147483648"s), std::numeric_limits<int>::min()); // may be different on some platforms
}

define_test(to_integer_throws_on_int_overflow)
{
    assert_throws(to_integer<int>("2147483648"s), std::exception); // may be different on some platforms
    assert_throws(to_integer<int>("-2147483649"s), std::exception); // may be different on some platforms
}

define_test(to_integer_converts_to_long)
{
    assert_equal(to_integer<long>("0"s), 0);
    assert_equal(to_integer<long>("1234"s), 1234);
    assert_equal(to_integer<long>("9223372036854775807"s), std::numeric_limits<long>::max()); // may be different on some platforms
    assert_equal(to_integer<long>("-9223372036854775808"s), std::numeric_limits<long>::min()); // may be different on some platforms
}

define_test(to_integer_throws_on_long_overflow)
{
    assert_throws(to_integer<long>("9223372036854775808"s), std::exception); // may be different on some platforms
    assert_throws(to_integer<long>("-9223372036854775809"s), std::exception); // may be different on some platforms
}

define_test(to_integer_converts_to_long_long)
{
    assert_equal(to_integer<long long>("0"s), 0);
    assert_equal(to_integer<long long>("1234"s), 1234);
    assert_equal(to_integer<long long>("9223372036854775807"s), std::numeric_limits<long long>::max()); // may be different on some platforms
    assert_equal(to_integer<long long>("-9223372036854775808"s), std::numeric_limits<long long>::min()); // may be different on some platforms
}

define_test(to_integer_converts_to_unsigned_int)
{
    assert_equal(to_integer<unsigned int>("0"s), 0);
    assert_equal(to_integer<unsigned int>("1234"s), 1234);
    assert_equal(to_integer<unsigned int>("4294967295"s), std::numeric_limits<unsigned int>::max()); // may be different on some platforms
}

define_test(to_integer_overflows_on_unsigned_int)
{
    assert_equal(to_integer<unsigned int>("-1"s), std::numeric_limits<unsigned int>::max()); // may be different on some platforms
    assert_equal(to_integer<unsigned int>("4294967296"s), 0);
}

define_test(to_decimal_converts_to_float)
{
    assert_equal(to_decimal<float>("0.1"s), 0.1f);
    assert_equal(to_decimal<float>("0.0123123"s), 0.0123123f);
    assert_equal(to_decimal<float>("500"s), 500.0f);
}

define_test(to_decimal_throws_on_float_overflow)
{
    assert_throws(to_decimal<float>("3.5e+38"s), std::exception); // may be different on some platforms
}

define_test(to_decimal_converts_to_double)
{
    assert_equal(to_decimal<double>("0.1"s), 0.1);
    assert_equal(to_decimal<double>("0.0123123"s), 0.0123123);
    assert_equal(to_decimal<double>("500"s), 500.0);
}

define_test(to_decimal_throws_on_double_overflow)
{
    assert_throws(to_decimal<double>("1.8e+308"s), std::exception); // may be different on some platforms
}

define_default_test_main();
