
#include <t1/t1.hpp>
#include "shl/format.hpp"

std::ostream& operator<<(std::ostream &lhs, const string &rhs)
{
    return lhs << rhs.data.data;
}

std::ostream& operator<<(std::ostream &lhs, const_string rhs)
{
    return lhs << rhs.c_str;
}

define_test(to_string_converts_bool_to_string)
{
    string str;
    init(&str);

    assert_equal(to_string(&str, true), 1);
    assert_equal(str, "1"_cs);
    assert_equal(to_string(&str, false), 1);
    assert_equal(str, "0"_cs);
    assert_equal(to_string(&str, true, 1 /*offset*/), 1);
    assert_equal(str, "01"_cs);
    assert_equal(to_string(&str, true, 0 /*offset*/, true /* as text */), 4);
    assert_equal(str, "true"_cs);
    assert_equal(to_string(&str, false, 0 /*offset*/, true /* as text */), 5);
    assert_equal(str, "false"_cs);

    free(&str);
}

define_test(to_string_converts_char_to_string)
{
    string str;
    init(&str);

    assert_equal(to_string(&str, 'a'), 1);
    assert_equal(str, "a"_cs);
    assert_equal(to_string(&str, 'b'), 1);
    assert_equal(str, "b"_cs);
    assert_equal(to_string(&str, 'c', 1 /*offset*/), 1);
    assert_equal(str, "bc"_cs);

    free(&str);
}

define_test(to_string_converts_number_to_string)
{
#define assert_to_string_number_format(Str, Result, Length, ...)\
    assert_equal(to_string(&str, __VA_ARGS__), Length);\
    assert_equal(string_length(&Str), Length);\
    assert_equal(Str, Result);\
    clear(&Str);

    string str;
    init(&str);

    // decimal
    assert_to_string_number_format(str, "5"_cs,   1, (u8)5);
    assert_to_string_number_format(str, "0"_cs,   1, (u8)0);

    // binary
    assert_to_string_number_format(str, "101"_cs, 3, (u8)5, 0 /*offset*/, number_format_options{.base = 2, .include_prefix = false, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "110"_cs, 3, (u8)6, 0, number_format_options{.base = 2, .include_prefix = false, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "0"_cs,   1, (u8)0, 0, number_format_options{.base = 2, .include_prefix = false, .pad_length = 0, .pad_char = '0'});

    assert_to_string_number_format(str, "0b1010"_cs, 6, (u8)0b1010, 0, number_format_options{.base = 2, .include_prefix = true, .pad_length = 0, .pad_char = '0'});

    // octal
    assert_to_string_number_format(str, "36"_cs, 2, (u8)30, 0, number_format_options{.base = 8, .include_prefix = false, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "77"_cs, 2, (u8)63, 0, number_format_options{.base = 8, .include_prefix = false, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "0"_cs,  1, (u8)0,  0, number_format_options{.base = 8, .include_prefix = false, .pad_length = 0, .pad_char = '0'});

    assert_to_string_number_format(str, "055"_cs, 3, (u8)055, 0, number_format_options{.base = 8, .include_prefix = true, .pad_length = 0, .pad_char = '0'});

    // hexadecimal
    assert_to_string_number_format(str, "10"_cs, 2, (u8)0x10, 0, number_format_options{.base = 16, .include_prefix = false, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "ff"_cs, 2, (u8)0xff, 0, number_format_options{.base = 16, .include_prefix = false, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "0"_cs,  1, (u8)0x00, 0, number_format_options{.base = 16, .include_prefix = false, .pad_length = 0, .pad_char = '0'});

    assert_to_string_number_format(str, "0xde"_cs, 4, (u8)0xde, 0, number_format_options{.base = 16, .include_prefix = true, .pad_length = 0, .pad_char = '0'});
    assert_to_string_number_format(str, "0xDE"_cs, 4, (u8)0xde, 0, number_format_options{.base = 16, .include_prefix = true, .pad_length = 0, .pad_char = '0', .caps_letters = true});
    assert_to_string_number_format(str, "0Xde"_cs, 4, (u8)0xde, 0, number_format_options{.base = 16, .include_prefix = true, .pad_length = 0, .pad_char = '0', .caps_letters = false, .caps_prefix = true});
    assert_to_string_number_format(str, "0XDE"_cs, 4, (u8)0xde, 0, number_format_options{.base = 16, .include_prefix = true, .pad_length = 0, .pad_char = '0', .caps_letters = true,  .caps_prefix = true});

    // padding
    assert_to_string_number_format(str, "00001337"_cs,    8, (u16)0x1337, 0, number_format_options{.base = 16, .include_prefix = false, .pad_length = 8, .pad_char = '0'});
    assert_to_string_number_format(str, "0x00001337"_cs, 10, (u16)0x1337, 0, number_format_options{.base = 16, .include_prefix = true, .pad_length = 8, .pad_char = '0'});
    assert_to_string_number_format(str, "0x1337"_cs,      6, (u16)0x1337, 0, number_format_options{.base = 16, .include_prefix = true, .pad_length = 2, .pad_char = '0'});

    // signed
    assert_to_string_number_format(str, "-5"_cs, 2, (s8)-5);
    assert_to_string_number_format(str, "5"_cs,  1, (s8)5, 0, number_format_options{.base = 10, .include_prefix = true, .pad_length = 0, .pad_char = '0', .force_sign = false});
    assert_to_string_number_format(str, "+5"_cs, 2, (s8)5, 0, number_format_options{.base = 10, .include_prefix = true, .pad_length = 0, .pad_char = '0', .force_sign = true});

    free(&str);
}

define_default_test_main();
