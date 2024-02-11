
#include <t1/t1.hpp>
#include "shl/number_types.hpp"
#include "shl/string.hpp"

define_t1_to_string(const string &s, "%s", s.data);
define_t1_to_string(const_string s, "%s", s.c_str);

define_test(cs_suffix_constructs_const_string_from_literal)
{
    const_string str = "abc"_cs;

    assert_equal(str.size, 3);
    assert_equal(compare_strings(str.c_str, "abc"), 0);
}

define_test(s_suffix_constructs_string_from_literal)
{
    string str = "abc"_s;

    assert_equal(string_length(&str), 3);
    assert_equal(compare_strings(str, "abc"_cs), 0);

    free(&str);
}

define_test(init_initializes_empty_string)
{
    string str;
    init(&str);

    assert_equal(string_length(&str), 0);

    free(&str);
}

define_test(init_initializes_string_with_uninitialized_data)
{
    string str;
    init(&str, 5);

    assert_equal(string_length(&str), 5);
    assert_equal(str[5], '\0');

    free(&str);
}

define_test(init_initializes_string_from_c_string)
{
    string str;
    init(&str, "hello");

    assert_equal(string_length(&str), 5);
    assert_equal(compare_strings(str, "hello"_cs), 0);
    assert_equal(str[5], '\0');

    free(&str);
}

define_test(init_initializes_string_from_c_string_with_length)
{
    string str;
    init(&str, "hello", 4);

    assert_equal(string_length(&str), 4);
    assert_equal(compare_strings(str, "hell"_cs), 0);
    assert_equal(str[4], '\0');

    free(&str);
}

define_test(init_initializes_string_from_const_string)
{
    string str;
    init(&str, "hello"_cs);

    assert_equal(string_length(&str), 5);
    assert_equal(compare_strings(str, "hello"_cs), 0);
    assert_equal(str[5], '\0');

    free(&str);
}

define_test(init_initializes_string_with_nulls)
{
    string str;
    init(&str, "w\0rld"_cs);

    // length is 5 !!
    assert_equal(string_length(&str), 5);
    assert_equal(compare_strings(str, "w\0rld"_cs), 0);
    assert_equal(str[5], '\0');

    free(&str);
}

define_test(clear_clears_string)
{
    string str = "hello world"_s;

    assert_equal(string_length(&str), 11);
    
    clear(&str);
    assert_equal(string_length(&str), 0);
    assert_equal(str[0], '\0');

    free(&str);
}

define_test(equals_operator_returns_if_strings_are_equal)
{
    string str = "hello world"_s;
    const_string cstr = "hello world"_cs;

    assert_equal(str == "hello world"_cs, true);
    assert_equal(cstr == "hello world"_cs, true);
    assert_equal(cstr == "world hello"_cs, false);

    // when == operator is defined, assert_equal works
    assert_equal(str, "hello world"_cs);

    free(&str);
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

define_test(is_empty_returns_true_if_string_is_empty)
{
    assert_equal(is_empty(""), true);
    assert_equal(is_empty(L""), true);
    assert_equal(is_empty(""_cs), true);
}

define_test(is_empty_returns_false_if_string_is_nullptr)
{
    string s;
    init(&s);

    const char *s2 = nullptr;

    assert_equal(is_empty(&s), false);
    assert_equal(is_empty(s2), false);

    free(&s);
}

define_test(is_null_or_empty_returns_true_if_string_is_empty)
{
    assert_equal(is_null_or_empty(""), true);
    assert_equal(is_null_or_empty(L""), true);
    assert_equal(is_null_or_empty(""_cs), true);
}

define_test(is_null_or_empty_returns_true_if_string_is_nullptr)
{
    string s;
    init(&s);

    const char *s2 = nullptr;

    assert_equal(is_null_or_empty(&s), true);
    assert_equal(is_null_or_empty(s2), true);

    free(&s);
}

define_test(is_blank_returns_true_if_string_is_empty)
{
    assert_equal(is_blank(""), true);
    assert_equal(is_blank(L""), true);
    assert_equal(is_blank(""_cs), true);
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
    assert_equal(is_blank("  \r\n\0  "_cs), true);
}

define_test(is_blank_returns_false_if_string_contains_non_whitespaces)
{
    assert_equal(is_blank("a"), false);
    assert_equal(is_blank("   def  "), false);
    assert_equal(is_blank(L"a"), false);
    assert_equal(is_blank(L"   def  "), false);
    assert_equal(is_blank("   \0def  "_cs), false);
}

define_test(string_length_returns_length_of_string)
{
    assert_equal(string_length(""), 0);
    assert_equal(string_length("a"), 1);
    assert_equal(string_length("abc"), 3);
    assert_equal(string_length(L"hello world"), 11);
    assert_equal(string_length("hell\0 w\0rld!!!"_cs), 14);
    
    string str = "hades"_s;
    assert_equal(string_length(&str), 5);
    free(&str);
}

define_test(compare_strings_compares_strings)
{
    assert_equal(compare_strings("", ""), 0);
    assert_equal(compare_strings("a", "a"), 0);
    assert_equal(compare_strings("hello", "hello"), 0);

    assert_less   (compare_strings("a", "b"), 0);
    assert_greater(compare_strings("b", "a"), 0);
    assert_less   (compare_strings("a", "aa"), 0);
    assert_greater(compare_strings("aa", "a"), 0);
    assert_less   (compare_strings(L"a", L"aa"), 0);
    assert_greater(compare_strings(L"aa", L"a"), 0);

    assert_equal(compare_strings("abc"_cs, "abc"_cs), 0);

    string a = "w\0rld"_s;
    string b = "w\0rld"_s;

    assert_equal(compare_strings(&a, &b), 0);
    assert_equal(compare_strings(a, "w\0rld"_cs), 0);
    assert_equal(compare_strings(a, "abc"_cs),     1);
    assert_equal(compare_strings(a, "alfred"_cs), -1);
    assert_equal(compare_strings(a, "zeta"_cs),   1);
    assert_equal(compare_strings(a, "zebra"_cs), -1);

    free(&a);
    free(&b);
}

define_test(begins_with_returns_true_if_string_starts_with_prefix)
{
    assert_equal(begins_with("hello", "hell"), true);
    assert_equal(begins_with(L"hello", L"hell"), true);

    string s = "hell\0 w\0rld"_s;
    assert_equal(begins_with(s, "hell\0 w"_cs), true);

    free(&s);
}

define_test(begins_with_returns_false_if_prefix_is_longer_than_string)
{
    assert_equal(begins_with("hello", "hellow"), false);
}

define_test(ends_with_returns_true_if_string_ends_with_suffix)
{
    assert_equal(ends_with("hello", "ello"), true);
    assert_equal(ends_with(L"hello", L"ello"), true);
    assert_equal(ends_with("world"_cs, "orld"_cs), true);

    string s = "hello w\0rld"_s;

    assert_equal(ends_with(s, "\0rld"_cs), true);

    free(&s);
}

define_test(ends_with_returns_false_if_suffix_is_longer_than_string)
{
    assert_equal(ends_with("hello", "whello"), false);
}

define_test(to_int_converts_to_int)
{
    assert_equal(to_int("0"), 0);
    assert_equal(to_int("1234"), 1234);
    assert_equal(to_int("2147483647"), max_value(s32)); // may be different on some platforms
    assert_equal(to_int("-2147483648"), min_value(s32)); // may be different on some platforms
    assert_equal(to_int("1234"_cs), 1234);
}

define_test(to_long_converts_to_long)
{
    assert_equal(to_long("0"), 0);
    assert_equal(to_long("1234"), 1234);
    assert_equal(to_long("9223372036854775807"), max_value(s64)); // may be different on some platforms
    assert_equal(to_long("-9223372036854775808"), min_value(s64)); // may be different on some platforms
    assert_equal(to_long("1234"_cs), 1234);
}

define_test(to_long_long_converts_to_long_long)
{
    assert_equal(to_long_long("0"), 0);
    assert_equal(to_long_long("1234"), 1234);
    assert_equal(to_long_long("9223372036854775807"), max_value(s64)); // may be different on some platforms
    assert_equal(to_long_long("-9223372036854775808"), min_value(s64)); // may be different on some platforms
    assert_equal(to_long_long("1234"_cs), 1234);
}

define_test(to_unsigned_int_converts_to_unsigned_int)
{
    assert_equal(to_unsigned_int("0"), 0);
    assert_equal(to_unsigned_int("1234"), 1234);
    assert_equal(to_unsigned_int("4294967295"), max_value(u32)); // may be different on some platforms
    assert_equal(to_unsigned_int("1234"_cs), 1234);
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

define_test(set_string_empty_string)
{
    string dst{};

    assert_equal(dst.data, nullptr);
    assert_equal(dst.size, 0);

    set_string(&dst, "");

    assert_equal(compare_strings(dst.data, ""), 0);
    assert_equal(dst.size, 0);

    free(&dst);
}

define_test(set_string_overwrites_string)
{
    string dst = "hello"_s;

    assert_equal(dst.size, 5);

    set_string(&dst, "bye"_cs);

    assert_equal(compare_strings(dst.data, "bye"), 0);
    assert_equal(dst.size, 3);

    free(&dst);
}

define_test(set_string_overwrites_string2)
{
    string dst = "abc"_s;

    assert_equal(dst.size, 3);

    set_string(&dst, "lorem ipsum abc def"_cs);

    assert_equal(compare_strings(dst.data, "lorem ipsum abc def"), 0);
    assert_equal(dst.size, 19);

    free(&dst);
}

define_test(copy_string_copies_string)
{
    const char *str1 = "abc";
    char str2[5] = {0};

    copy_string(str1, str2, 3);

    assert_equal(compare_strings(str1, str2), 0);
}

define_test(copy_string_copies_to_empty_string_object)
{
    string str;
    init(&str);

    assert_equal(string_length(str), 0);

    copy_string("abc", &str);

    assert_equal(string_length(str), 3);
    assert_equal(str[3], '\0');

    assert_equal(compare_strings(str, "abc"_cs), 0);

    free(&str);
}

define_test(copy_string_copies_up_to_n_characters)
{
    string str = "hello world"_s;

    assert_equal(string_length(str), 11);

    // does not truncate past copying
    copy_string("lorem ipsum", &str, 5);

    assert_equal(string_length(str), 11);
    assert_equal(str[string_length(str)], '\0');

    assert_equal(compare_strings(str, "lorem world"_cs), 0);

    str.size = 5;

    assert_equal(compare_strings(str, "lorem"_cs), 0);

    free(&str);
}

define_test(copy_string_copies_at_destination_offset)
{
    string str = "hello world"_s;

    assert_equal(string_length(str), 11);

    copy_string("lorem ipsum", &str, 5, 6);

    assert_equal(string_length(str), 11);
    assert_equal(str[string_length(str)], '\0');

    assert_equal(compare_strings(str, "hello lorem"_cs), 0);

    free(&str);
}

define_test(copy_string_copies_at_destination_offset_and_allocates_if_offset_is_outside_destination_size)
{
    string str = "hello world"_s;

    assert_equal(string_length(str), 11);

    copy_string("lorem ipsum", &str, 5, 11);

    assert_equal(string_length(str), 16);
    assert_equal(str[string_length(str)], '\0');

    assert_equal(compare_strings(str, "hello worldlorem"_cs), 0);

    free(&str);
}

define_test(copy_string_copies_string3)
{
    string a = ""_s;

    copy_string("hello", &a);

    assert_equal(string_length(&a), 5);
    assert_equal(a, "hello"_cs);

    copy_string("hello_", &a);

    assert_equal(string_length(&a), 6);
    assert_equal(a, "hello_"_cs);

    free(&a);
}

define_test(copy_string_copies_to_new_string)
{
    const char *str1 = "abc";

    string ret = copy_string(str1);

    assert_equal(ret, "abc"_cs);

    free(&ret);
}

define_test(append_string_appends_to_empty_string_object)
{
    string str;
    init(&str);

    assert_equal(string_length(str), 0);

    append_string(&str, "abc");

    assert_equal(string_length(str), 3);
    assert_equal(str[3], '\0');

    assert_equal(compare_strings(str, "abc"_cs), 0);

    free(&str);
}

define_test(append_string_appends_to_string_object)
{
    string str = "hello"_s;

    assert_equal(string_length(str), 5);

    append_string(&str, " world");

    assert_equal(string_length(str), 11);
    assert_equal(str[11], '\0');

    assert_equal(compare_strings(str, "hello world"_cs), 0);

    free(&str);
}

define_test(append_string_appends_to_string_object2)
{
    string str = "hello"_s;
    string ipsum = " ipsum"_s;

    assert_equal(string_length(str), 5);

    append_string(&str, " world");
    append_string(&str, " lorem"_cs);
    append_string(&str, ipsum);

    assert_equal(string_length(str), 23);
    assert_equal(str[23], '\0');

    assert_equal(compare_strings(str, "hello world lorem ipsum"_cs), 0);

    free(&ipsum);
    free(&str);
}

define_test(prepend_string_prepends_to_empty_string_object)
{
    string str;
    init(&str);

    assert_equal(string_length(str), 0);

    prepend_string(&str, "abc");

    assert_equal(string_length(str), 3);
    assert_equal(str[3], '\0');

    assert_equal(compare_strings(str, "abc"_cs), 0);

    free(&str);
}

define_test(prepend_string_prepends_to_string_object)
{
    string str = "world"_s;

    assert_equal(string_length(str), 5);

    prepend_string(&str, "hello ");

    assert_equal(string_length(str), 11);
    assert_equal(str[11], '\0');

    assert_equal(compare_strings(str, "hello world"_cs), 0);

    free(&str);
}

define_test(prepend_string_prepends_to_string_object2)
{
    string str = "ipsum"_s;
    string hello = "hello "_s;

    assert_equal(string_length(str), 5);

    prepend_string(&str, "lorem ");
    prepend_string(&str, "world "_cs);
    prepend_string(&str, hello);

    assert_equal(string_length(str), 23);
    assert_equal(str[23], '\0');

    assert_equal(compare_strings(str, "hello world lorem ipsum"_cs), 0);

    free(&hello);
    free(&str);
}

define_test(index_of_returns_negative_one_on_negative_offset)
{
    assert_equal(index_of("hello"_cs, "hell"_cs, -1), -1);
    assert_equal(index_of("hello"_cs, "hell"_cs, -500), -1);
}

define_test(index_of_returns_offset_on_empty_needle)
{
    assert_equal(index_of("hello"_cs, ""_cs, 0), 0);
    assert_equal(index_of("hello"_cs, ""_cs, 1), 1);
    assert_equal(index_of("hello"_cs, ""_cs, 2), 2);
    assert_equal(index_of("hello"_cs, ""_cs, 3), 3);
    assert_equal(index_of("hello"_cs, ""_cs, 4), 4);
}

define_test(index_of_returns_negative_one_if_offset_is_outside_haystack)
{
    assert_equal(index_of("hello"_cs, "hell"_cs, 5), -1);
    assert_equal(index_of("hello"_cs, "hell"_cs, 100), -1);
}

define_test(index_of_returns_index_of_first_needle_occurence_in_haystack_starting_at_offset)
{
    assert_equal(index_of("hello"_cs, "hell"_cs, 0), 0);
    assert_equal(index_of("hello"_cs, "hell"_cs, 1), -1);

    assert_equal(index_of("hello hello hello hello"_cs, "hell"_cs, 1), 6);
    assert_equal(index_of("hello hello hello hello"_cs, "hell"_cs, 6), 6);
    assert_equal(index_of("hello hello hello hello"_cs, "hell"_cs, 7), 12);
    assert_equal(index_of("hello hello hello hello"_cs, "hell"_cs, 13), 18);

    assert_equal(index_of("w\0rld w\0rld w\0rld"_cs, "w\0rld"_cs, 1), 6);

    assert_equal(index_of(L"hello hello hello hello"_cs, L"hell"_cs, 1), 6);
    assert_equal(index_of(L"hello hello hello hello"_cs, L"hell"_cs, 6), 6);
    assert_equal(index_of(L"hello hello hello hello"_cs, L"hell"_cs, 7), 12);
    assert_equal(index_of(L"hello hello hello hello"_cs, L"hell"_cs, 13), 18);

    assert_equal(index_of("hello"_cs, 'e'), 1);
    assert_equal(index_of("hello"_cs, 'l'), 2);
    assert_equal(index_of("hello"_cs, 'l', 3), 3);
    assert_equal(index_of("hello"_cs, 'l', 4), -1);
}

define_test(last_index_of_returns_negative_one_on_negative_offset)
{
    assert_equal(last_index_of("hello"_cs, "hell"_cs, -1), -1);
    assert_equal(last_index_of("hello"_cs, "hell"_cs, -500), -1);
}

define_test(last_index_of_returns_offset_on_empty_needle)
{
    assert_equal(last_index_of("hello", "", 0), 0);
    assert_equal(last_index_of("hello", "", 1), 1);
    assert_equal(last_index_of("hello", "", 2), 2);
    assert_equal(last_index_of("hello", "", 3), 3);
    assert_equal(last_index_of("hello", "", 4), 4);
}

define_test(last_index_of_returns_index_of_last_needle_occurence_in_haystack_starting_in_reverse_at_offset)
{
    assert_equal(last_index_of("hello", "hell"), 0);
    assert_equal(last_index_of("hello", "hell", 0), 0);
    assert_equal(last_index_of("hello", "hell", 1), 0);

    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs), 18);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 500), 18);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 19), 18);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 18), 18);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 17), 12);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 13), 12);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 12), 12);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 11), 6);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 6), 6);
    assert_equal(last_index_of("hello hello hello hello"_cs, "hell"_cs, 1), 0);

    assert_equal(last_index_of("w\0rld w\0rld w\0rld"_cs, "w\0rld"_cs), 12);
    assert_equal(last_index_of("w\0rld w\0rld w\0rld"_cs, "w\0rld"_cs, 1), 0);

    assert_equal(last_index_of(L"hello hello hello hello", L"hell"), 18);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 500), 18);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 19), 18);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 18), 18);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 17), 12);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 13), 12);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 12), 12);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 11), 6);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 6), 6);
    assert_equal(last_index_of(L"hello hello hello hello", L"hell", 1), 0);

    assert_equal(last_index_of("hello", 'e'), 1);
    assert_equal(last_index_of("hello", 'l'), 3);
    assert_equal(last_index_of("hello", 'l', 2), 2);
    assert_equal(last_index_of("hello", 'l', 1), -1);
    assert_equal(last_index_of("hello", 'o'), 4);
    assert_equal(last_index_of("hello", 'h'), 0);
    assert_equal(last_index_of("o", 'o'), 0);
}

define_test(contains_returns_true_when_string_contains_other_string)
{
    assert_equal(contains("hello"_cs, "hell"_cs), true);
    assert_equal(contains("hello"_cs, "ello"_cs), true);
    assert_equal(contains("hello"_cs, "hello"_cs), true);
    assert_equal(contains("hello"_cs, "l"_cs), true);
    assert_equal(contains("hello"_cs, 'l'), true);
    assert_equal(contains("hello"_cs, ""_cs), true);
}

define_test(contains_returns_false_when_string_doesnt_contain_other_string)
{
    assert_equal(contains("hello"_cs, "world"_cs), false);
    assert_equal(contains("hello"_cs, "hello123"_cs), false);
    assert_equal(contains("hello"_cs, "123hello"_cs), false);
    assert_equal(contains("hello"_cs, "a"_cs), false);
    assert_equal(contains("hello"_cs, 'a'), false);
}

define_test(to_upper_converts_to_upper)
{
    assert_equal(to_upper('a'), 'A');
    assert_equal(to_upper('z'), 'Z');
    assert_equal(to_upper(' '), ' ');
    assert_equal(to_upper(L'a'), L'A');
    assert_equal(to_upper(L'z'), L'Z');
    assert_equal(to_upper(L'!'), L'!');

    string s = "hello world"_s;
    to_upper(&s);
    assert_equal(compare_strings(s, "HELLO WORLD"_cs), 0);

    free(&s);
}

define_test(to_lower_converts_to_lower)
{
    assert_equal(to_lower('A'), 'a');
    assert_equal(to_lower('Z'), 'z');
    assert_equal(to_lower(' '), ' ');
    assert_equal(to_lower(L'A'), L'a');
    assert_equal(to_lower(L'Z'), L'z');
    assert_equal(to_lower(L'!'), L'!');

    string s = "HELLO WORLD"_s;
    to_lower(&s);
    assert_equal(compare_strings(s, "hello world"_cs), 0);

    free(&s);
}

define_test(trim_left_trims_leftmost_whitespaces_from_string)
{
    string s = "  ab c  "_s;
    assert_equal(string_length(&s), 8);

    trim_left(&s);
    assert_equal(compare_strings(s, "ab c  "_cs), 0);
    assert_equal(string_length(&s), 6);
    assert_equal(s[string_length(&s)], '\0');
    trim_left(&s);
    assert_equal(compare_strings(s, "ab c  "_cs), 0);
    assert_equal(string_length(&s), 6);
    assert_equal(s[string_length(&s)], '\0');

    free(&s);
}

define_test(trim_left_trims_whitespace_string)
{
    string s = "  \0\t\v\n  "_s;
    assert_equal(string_length(&s), 8);

    trim_left(&s);
    assert_equal(compare_strings(s, ""_cs), 0);
    assert_equal(string_length(&s), 0);
    assert_equal(s[0], '\0');
    trim_left(&s);
    assert_equal(compare_strings(s, ""_cs), 0);
    assert_equal(string_length(&s), 0);
    assert_equal(s[0], '\0');

    free(&s);
}

define_test(trim_right_trims_rightmost_whitespaces_from_string)
{
    string s = "  ab c  "_s;
    assert_equal(string_length(&s), 8);

    trim_right(&s);
    assert_equal(compare_strings(s, "  ab c"_cs), 0);
    assert_equal(string_length(&s), 6);
    assert_equal(s[string_length(&s)], '\0');
    trim_right(&s);
    assert_equal(compare_strings(s, "  ab c"_cs), 0);
    assert_equal(string_length(&s), 6);
    assert_equal(s[string_length(&s)], '\0');

    free(&s);
}

define_test(trim_right_trims_whitespace_string)
{
    string s = "  \0\t\v\n  "_s;
    assert_equal(string_length(&s), 8);

    trim_right(&s);
    assert_equal(compare_strings(s, ""_cs), 0);
    assert_equal(string_length(&s), 0);
    assert_equal(s[0], '\0');
    trim_right(&s);
    assert_equal(compare_strings(s, ""_cs), 0);
    assert_equal(string_length(&s), 0);
    assert_equal(s[0], '\0');

    free(&s);
}

define_test(trim_trims_leftmost_and_rightmost_whitespaces_from_string)
{
    string s = "  ab c  "_s;

    assert_equal(string_length(&s), 8);

    trim(&s);
    assert_equal(compare_strings(s, "ab c"_cs), 0);
    assert_equal(string_length(&s), 4);
    assert_equal(s[string_length(&s)], '\0');
    trim(&s);
    assert_equal(compare_strings(s, "ab c"_cs), 0);
    assert_equal(string_length(&s), 4);
    assert_equal(s[string_length(&s)], '\0');

    free(&s);
}

define_test(trim_trims_whitespace_string)
{
    string s = "  \0\t\v\n  "_s;
    assert_equal(string_length(&s), 8);

    trim(&s);
    assert_equal(compare_strings(s, ""_cs), 0);
    assert_equal(string_length(&s), 0);
    assert_equal(s[0], '\0');
    trim(&s);
    assert_equal(compare_strings(s, ""_cs), 0);
    assert_equal(string_length(&s), 0);
    assert_equal(s[0], '\0');

    free(&s);
}

define_test(substring_of_zero_length_does_nothing)
{
    string out;
    init(&out);

    substring("hello"_cs, 0, 0, &out);

    assert_equal(string_length(&out), 0);

    free(&out);
}

define_test(substring_starting_outside_source_string_does_nothing)
{
    string out;
    init(&out);

    substring("hello"_cs, 5, 0, &out);

    assert_equal(string_length(&out), 0);

    substring("hello"_cs, 500, 0, &out);

    assert_equal(string_length(&out), 0);

    free(&out);
}

define_test(substring_copies_substring_to_empty_string)
{
    string out;
    init(&out);

    substring("hello"_cs, 0, 5, &out);

    assert_equal(string_length(&out), 5);
    assert_equal(compare_strings(out, "hello"_cs), 0);
    assert_equal(out[5], '\0');

    free(&out);
}

define_test(substring_overwrites_string_with_substring)
{
    string out = "yoink world"_s;

    assert_equal(string_length(&out), 11);

    substring("hello"_cs, 0, 5, &out);

    assert_equal(string_length(&out), 11);
    assert_equal(compare_strings(out, "hello world"_cs), 0);
    assert_equal(out[11], '\0');

    free(&out);
}

define_test(substring_overwrites_string_with_substring_at_given_offset)
{
    string out = "hello yoink"_s;

    assert_equal(string_length(&out), 11);

    substring("world"_cs, 0, 5, &out, 6);

    assert_equal(string_length(&out), 11);
    assert_equal(compare_strings(out, "hello world"_cs), 0);
    assert_equal(out[11], '\0');

    free(&out);
}

define_test(substring_allocates_memory_in_target_when_target_is_not_large_enough)
{
    string out = "hello wo"_s;

    assert_equal(string_length(&out), 8);

    substring("world"_cs, 0, 5, &out, 6);

    assert_equal(string_length(&out), 11);
    assert_equal(compare_strings(out, "hello world"_cs), 0);
    assert_equal(out[11], '\0');

    free(&out);
}

define_test(substring_with_no_out_parameter_returns_slice_to_data)
{
    const_string input = "hello"_cs;
    const_string output;

    output = substring(input, 0);
    assert_equal(compare_strings(input, output), 0);
    assert_equal(input.c_str, output.c_str);

    output = substring(input, 0, 5);
    assert_equal(compare_strings(input, output), 0);
    assert_equal(input.c_str, output.c_str);

    output = substring(input, 0, 5000);
    assert_equal(compare_strings(input, output), 0);
    assert_equal(input.c_str, output.c_str);

    output = substring(input, 0, max_value(u64));
    assert_equal(compare_strings(input, output), 0);
    assert_equal(input.c_str, output.c_str);

    output = substring(input, 1);
    assert_equal(compare_strings(output, "ello"), 0);
    assert_equal(output.c_str, input.c_str + 1);
    assert_equal(output.size, 4);

    output = substring(input, 1, 3);
    assert_equal(compare_strings(output, "ell"), 0);
    assert_equal(output.c_str, input.c_str + 1);
    assert_equal(output.size, 3);

    output = substring(input, 5, 3);
    assert_equal(compare_strings(output, ""), 0);
    assert_equal(output.c_str, input.c_str + input.size);
    assert_equal(output.size, 0);

    output = substring(input, 20, 3);
    assert_equal(compare_strings(output, ""), 0);
    assert_equal(output.c_str, input.c_str + input.size);
    assert_equal(output.size, 0);
}


define_test(hash_hashes_string)
{
    string str = "hello world"_s;

    assert_equal(hash(str), hash("hello world"_cs));
    assert_equal(hash(str), hash("hello world"));

    free(&str);
}

define_test(hash_hashes_string_with_null_characters)
{
    string str = "hell\0 w\0rld"_s;

    assert_equal(hash(str), hash("hell\0 w\0rld"_cs));

    // these are not equal because plain const char* string hashes
    // stop before the first null character.
    assert_not_equal(hash(str), hash("hell\0 w\0rld"));

    free(&str);
}

define_test(replace_does_nothing_on_empty_needle)
{
    string str = "hello world"_s;

    replace(&str, "", ""_cs);
    replace(&str, "", "abc"_cs);

    assert_equal(str, "hello world"_cs);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_characters)
{
    string str = "hello world"_s;

    replace(&str, 'o', ' ');

    assert_equal(str, "hell  world"_cs);
    assert_equal(string_length(&str), 11);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_equal_length_strings)
{
    string str = "hello world"_s;

    replace(&str, "hell"_cs, "beef"_cs);

    assert_equal(str, "beefo world"_cs);
    assert_equal(string_length(&str), 11);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_needle_with_larger_replacement)
{
    string str = "hello world"_s;

    replace(&str, "hello"_cs, "good morning"_cs);

    assert_equal(str, "good morning world"_cs);
    assert_equal(string_length(&str), 18);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_needle_with_shorter_replacement)
{
    string str = "hello world"_s;

    replace(&str, "hello"_cs, "hell"_cs);

    assert_equal(str, "hell world"_cs);
    assert_equal(string_length(&str), 10);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_needle_with_shorter_replacement2)
{
    string str = "hello world"_s;

    replace(&str, "hello"_cs, ""_cs);

    assert_equal(str, " world"_cs);
    assert_equal(string_length(&str), 6);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_needle_with_shorter_replacement3)
{
    string str = "hello world"_s;

    replace(&str, "hello world"_cs, ""_cs);

    assert_equal(str, ""_cs);
    assert_equal(string_length(&str), 0);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_replaces_needle_with_replacement)
{
    string str = "hello hello"_s;

    replace(&str, "hello"_cs, "world"_cs);

    assert_equal(str, "world hello"_cs);

    free(&str);
}

define_test(replace_replaces_needle_with_replacement_at_offset)
{
    string str = "hello hello"_s;

    replace(&str, "hello"_cs, "world"_cs, 1);

    assert_equal(str, "hello world"_cs);

    free(&str);
}

define_test(replace_does_nothing_if_offset_is_outside_string)
{
    string str = "hello world"_s;

    replace(&str, "hello"_cs, "world"_cs, -500);

    assert_equal(str, "hello world"_cs);

    replace(&str, "hello"_cs, "world"_cs, 200);

    assert_equal(str, "hello world"_cs);

    free(&str);
}

define_test(replace_all_replaces_all_occurrences_of_character)
{
    string str = "hello world"_s;

    replace_all(&str, 'l', ' ');

    assert_equal(str, "he  o wor d"_cs);

    free(&str);
}

define_test(replace_all_replaces_all_occurrences_of_character_starting_at_offset)
{
    string str = "hello world"_s;

    replace_all(&str, 'l', ' ', 3);

    assert_equal(str, "hel o wor d"_cs);

    free(&str);
}

define_test(replace_all_replaces_all_occurrences_of_string)
{
    string str = "hello hello hello world"_s;

    replace_all(&str, "hello"_cs, "bye"_cs);

    assert_equal(str, "bye bye bye world"_cs);
    assert_equal(string_length(&str), 17);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_all_replaces_all_occurrences_of_string_starting_at_offset)
{
    string str = "hello hello hello world"_s;

    replace_all(&str, "hello"_cs, "bye"_cs, 1);

    assert_equal(str, "hello bye bye world"_cs);
    assert_equal(string_length(&str), 19);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(replace_all_replaces_all_occurrences_of_string_starting_at_offset2)
{
    string str = "hello hello hello world"_s;

    replace_all(&str, "hello"_cs, "bye"_cs, 5);

    assert_equal(str, "hello bye bye world"_cs);
    assert_equal(string_length(&str), 19);
    assert_equal(str[string_length(&str)], '\0');

    free(&str);
}

define_test(split_splits_by_char)
{
    const_string str = "hello;world"_cs;
    array<const_string> splits;
    init(&splits);

    assert_equal(split(str, ';', &splits), 1);

    assert_equal(splits.size, 2);
    assert_equal(splits[0], "hello"_cs);
    assert_equal(splits[0].size, 5);
    assert_equal(splits[1], "world"_cs);
    assert_equal(splits[1].size, 5);

    free(&splits);
}

define_test(split_splits_by_char2)
{
    const_string str = "hello world, this is a test"_cs;
    array<const_string> splits;
    init(&splits);

    assert_equal(split(str, ' ', &splits), 5);

    assert_equal(splits.size, 6);
    assert_equal(splits[0], "hello"_cs);
    assert_equal(splits[0].size, 5);
    assert_equal(splits[1], "world,"_cs);
    assert_equal(splits[1].size, 6);
    assert_equal(splits[2], "this"_cs);
    assert_equal(splits[2].size, 4);
    assert_equal(splits[3], "is"_cs);
    assert_equal(splits[3].size, 2);
    assert_equal(splits[4], "a"_cs);
    assert_equal(splits[4].size, 1);
    assert_equal(splits[5], "test"_cs);
    assert_equal(splits[5].size, 4);

    free(&splits);
}

define_test(split_splits_by_char3)
{
    const_string str = "hello world, this is a test"_cs;
    array<const_string> splits;
    init(&splits);

    assert_equal(split(str, '!', &splits), 0);

    assert_equal(splits.size, 1);
    assert_equal(splits[0], str);
    assert_equal(splits[0].size, str.size);

    free(&splits);
}

define_test(split_splits_by_char4)
{
    const_string str = "!hello world, this is a test!"_cs;
    array<const_string> splits;
    init(&splits);

    assert_equal(split(str, '!', &splits), 2);

    assert_equal(splits.size, 3);
    assert_equal(splits[0], ""_cs);
    assert_equal(splits[0].size, 0);
    assert_equal(splits[1], "hello world, this is a test"_cs);
    assert_equal(splits[1].size, 27);
    assert_equal(splits[2], ""_cs);
    assert_equal(splits[2].size, 0);

    free(&splits);
}

define_test(split_splits_by_string)
{
    const_string str = "hello ABC world"_cs;
    array<const_string> splits;
    init(&splits);

    assert_equal(split(str, " ABC ", &splits), 1);

    assert_equal(splits.size, 2);
    assert_equal(splits[0], "hello"_cs);
    assert_equal(splits[0].size, 5);
    assert_equal(splits[1], "world"_cs);
    assert_equal(splits[1].size, 5);

    str = "hello ABC world, ABC this ABC is ABC a ABC test"_cs;
    assert_equal(split(str, " ABC "_cs, &splits), 5);

    assert_equal(splits.size, 6);
    assert_equal(splits[0], "hello"_cs);
    assert_equal(splits[0].size, 5);
    assert_equal(splits[1], "world,"_cs);
    assert_equal(splits[1].size, 6);
    assert_equal(splits[2], "this"_cs);
    assert_equal(splits[2].size, 4);
    assert_equal(splits[3], "is"_cs);
    assert_equal(splits[3].size, 2);
    assert_equal(splits[4], "a"_cs);
    assert_equal(splits[4].size, 1);
    assert_equal(splits[5], "test"_cs);
    assert_equal(splits[5].size, 4);

    free(&splits);
}

define_test(split_splits_by_string2)
{
    const_string str = "!!!hello world, this is a test!!!"_cs;
    array<const_string> splits;
    init(&splits);

    assert_equal(split(str, "!!!", &splits), 2);

    assert_equal(splits.size, 3);
    assert_equal(splits[0], ""_cs);
    assert_equal(splits[0].size, 0);
    assert_equal(splits[1], "hello world, this is a test"_cs);
    assert_equal(splits[1].size, 27);
    assert_equal(splits[2], ""_cs);
    assert_equal(splits[2].size, 0);

    free(&splits);
}

define_test(join_joins_strings_by_char)
{
    const_string str = "a,b,c,d"_cs;
    array<const_string> splits;
    init(&splits);

    split(str, ',', &splits);

    string out = ""_s;
    join(&splits, ';', &out);

    assert_equal(string_length(&out), 7);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, "a;b;c;d"_cs);

    free(&out);
    free(&splits);
}

define_test(join_joins_strings_by_char2)
{
    const_string str = ",a,b,,,c,d,"_cs;
    array<const_string> splits;
    init(&splits);

    split(str, ',', &splits);

    string out = ""_s;
    join(&splits, ';', &out);

    assert_equal(string_length(&out), 11);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, ";a;b;;;c;d;"_cs);

    free(&out);
    free(&splits);
}

define_test(join_joins_strings_by_char3)
{
    const_string str = "a,b,c,d"_cs;

    string out = ""_s;
    join(&str, 1, ' ', &out);

    assert_equal(string_length(&out), 7);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, str);

    join(&str, 0, ' ', &out);

    assert_equal(string_length(&out), 7);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, str);

    free(&out);
}

define_test(join_joins_strings_by_string_delim)
{
    const_string str = "a,b,c,d"_cs;
    array<const_string> splits;
    init(&splits);

    split(str, ',', &splits);

    string out = ""_s;
    join(&splits, "hello", &out);

    assert_equal(string_length(&out), 19);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, "ahellobhellochellod"_cs);

    free(&out);
    free(&splits);
}

define_test(join_joins_strings_by_string_delim2)
{
    const_string str = ",a,,,b,"_cs;
    array<const_string> splits;
    init(&splits);

    split(str, ',', &splits);

    string out = ""_s;
    join(&splits, "HEY"_cs, &out);

    assert_equal(string_length(&out), 17);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, "HEYaHEYHEYHEYbHEY"_cs);

    free(&out);
    free(&splits);
}

define_test(join_joins_strings_by_string_delim3)
{
    const_string str = "a,b,c"_cs;
    array<const_string> splits;
    init(&splits);

    split(str, ',', &splits);

    string out = ""_s;
    join(&splits, "", &out);

    assert_equal(string_length(&out), 3);
    assert_equal(out[string_length(&out)], '\0');
    assert_equal(out, "abc"_cs);

    free(&out);
    free(&splits);
}


define_default_test_main();
