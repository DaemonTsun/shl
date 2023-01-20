
#include <t1/t1.hpp>
#include "shl/string.hpp"

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
    assert_equal(is_blank(""), true);
    assert_equal(is_blank(L""), true);
}

define_test(is_blank_returns_true_if_string_is_whitespaces)
{
    assert_equal(is_blank(" "), true);
    assert_equal(is_blank("     "), true);
    assert_equal(is_blank("  \t  "), true);
    assert_equal(is_blank("  \r\n  "), true);
    assert_equal(is_blank(L" "), true);
    assert_equal(is_blank(L"     "), true);
    assert_equal(is_blank(L"  \t  "), true);
    assert_equal(is_blank(L"  \r\n  "), true);
}

define_test(is_blank_returns_false_if_string_contains_non_whitespaces)
{
    assert_equal(is_blank("a"), false);
    assert_equal(is_blank("   def  "), false);
    assert_equal(is_blank(L"a"), false);
    assert_equal(is_blank(L"   def  "), false);
}

define_test(string_length_returns_length_of_string)
{
    assert_equal(string_length(""), 0);
    assert_equal(string_length("a"), 1);
    assert_equal(string_length("abc"), 3);
    assert_equal(string_length(L"hello world"), 11);
}

define_test(compare_strings_compares_strings)
{
    assert_equal(compare_strings("", ""), 0);
    assert_equal(compare_strings("a", "a"), 0);
    assert_equal(compare_strings("hello", "hello"), 0);
    assert_less(compare_strings("a", "b"), 0);
    assert_greater(compare_strings("b", "a"), 0);
    assert_less(compare_strings("a", "aa"), 0);
    assert_greater(compare_strings("aa", "a"), 0);
    assert_less(compare_strings(L"a", L"aa"), 0);
    assert_greater(compare_strings(L"aa", L"a"), 0);
}

define_test(begins_with_returns_true_if_string_starts_with_prefix)
{
    assert_equal(begins_with("hello", "hell"), true);
    assert_equal(begins_with(L"hello", L"hell"), true);
}

define_test(begins_with_returns_false_if_prefix_is_longer_than_string)
{
    assert_equal(begins_with("hello", "hellow"), false);
}

define_test(ends_with_returns_true_if_string_ends_with_suffix)
{
    assert_equal(ends_with("hello", "ello"), true);
    assert_equal(ends_with(L"hello", L"ello"), true);
}

define_test(ends_with_returns_false_if_suffix_is_longer_than_string)
{
    assert_equal(ends_with("hello", "whello"), false);
}

define_test(to_int_converts_to_int)
{
    assert_equal(to_int("0"), 0);
    assert_equal(to_int("1234"), 1234);
    assert_equal(to_int("2147483647"), std::numeric_limits<int>::max()); // may be different on some platforms
    assert_equal(to_int("-2147483648"), std::numeric_limits<int>::min()); // may be different on some platforms
}

define_test(to_long_converts_to_long)
{
    assert_equal(to_long("0"), 0);
    assert_equal(to_long("1234"), 1234);
    assert_equal(to_long("9223372036854775807"), std::numeric_limits<long>::max()); // may be different on some platforms
    assert_equal(to_long("-9223372036854775808"), std::numeric_limits<long>::min()); // may be different on some platforms
}

define_test(to_long_long_converts_to_long_long)
{
    assert_equal(to_long_long("0"), 0);
    assert_equal(to_long_long("1234"), 1234);
    assert_equal(to_long_long("9223372036854775807"), std::numeric_limits<long long>::max()); // may be different on some platforms
    assert_equal(to_long_long("-9223372036854775808"), std::numeric_limits<long long>::min()); // may be different on some platforms
}

define_test(to_unsigned_int_converts_to_unsigned_int)
{
    assert_equal(to_unsigned_int("0"), 0);
    assert_equal(to_unsigned_int("1234"), 1234);
    assert_equal(to_unsigned_int("4294967295"), std::numeric_limits<unsigned int>::max()); // may be different on some platforms
}

define_test(to_float_converts_to_float)
{
    assert_equal(to_float("0.1"), 0.1f);
    assert_equal(to_float("0.0123123"), 0.0123123f);
    assert_equal(to_float("500"), 500.0f);
}

define_test(to_double_converts_to_double)
{
    assert_equal(to_double("0.1"), 0.1);
    assert_equal(to_double("0.0123123"), 0.0123123);
    assert_equal(to_double("500"), 500.0);
}

define_test(copy_string_copies_string)
{
    const char *str1 = "abc";
    char str2[5];

    copy_string(str1, str2, 3);

    assert_equal(compare_strings(str1, str2), 0);
}

define_default_test_main();
