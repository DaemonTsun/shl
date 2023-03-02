
#include <t1/t1.hpp>
#include "shl/string.hpp"

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
    assert_equal(to_int("2147483647"), std::numeric_limits<int>::max()); // may be different on some platforms
    assert_equal(to_int("-2147483648"), std::numeric_limits<int>::min()); // may be different on some platforms
    assert_equal(to_int("1234"_cs), 1234);
}

define_test(to_long_converts_to_long)
{
    assert_equal(to_long("0"), 0);
    assert_equal(to_long("1234"), 1234);
    assert_equal(to_long("9223372036854775807"), std::numeric_limits<long>::max()); // may be different on some platforms
    assert_equal(to_long("-9223372036854775808"), std::numeric_limits<long>::min()); // may be different on some platforms
    assert_equal(to_long("1234"_cs), 1234);
}

define_test(to_long_long_converts_to_long_long)
{
    assert_equal(to_long_long("0"), 0);
    assert_equal(to_long_long("1234"), 1234);
    assert_equal(to_long_long("9223372036854775807"), std::numeric_limits<long long>::max()); // may be different on some platforms
    assert_equal(to_long_long("-9223372036854775808"), std::numeric_limits<long long>::min()); // may be different on some platforms
    assert_equal(to_long_long("1234"_cs), 1234);
}

define_test(to_unsigned_int_converts_to_unsigned_int)
{
    assert_equal(to_unsigned_int("0"), 0);
    assert_equal(to_unsigned_int("1234"), 1234);
    assert_equal(to_unsigned_int("4294967295"), std::numeric_limits<unsigned int>::max()); // may be different on some platforms
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

    str.data.size = 5;

    assert_equal(compare_strings(str, "lorem"_cs), 0);

    free(&str);
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

define_default_test_main();
