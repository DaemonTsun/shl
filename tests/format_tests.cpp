
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

    format_options<char> opt = default_format_options<char>;

    assert_equal(to_string(&str, true), 1);
    assert_equal(str, "1"_cs);
    assert_equal(to_string(&str, false), 1);
    assert_equal(str, "0"_cs);
    assert_equal(to_string(&str, true, 1 /*offset*/), 1);
    assert_equal(str, "01"_cs);
    assert_equal(to_string(&str, true, 0 /*offset*/, opt, true /* as text */), 4);
    assert_equal(str, "true"_cs);
    assert_equal(to_string(&str, false, 0 /*offset*/, opt, true /* as text */), 5);
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

define_test(to_string_converts_string_to_string)
{
    string str;
    init(&str);

    assert_equal(to_string(&str, "hello"_cs), 5);
    assert_equal(str, "hello"_cs);
    assert_equal(to_string(&str, "world"_cs), 5);
    assert_equal(str, "world"_cs);
    assert_equal(to_string(&str, "hello"_cs, 5 /*offset*/), 5);
    assert_equal(str, "worldhello"_cs);

    free(&str);
}

#define assert_to_string(Str, Result, Length, ...)\
    assert_equal(to_string(&str, __VA_ARGS__), Length);\
    assert_equal(string_length(&Str), Length);\
    assert_equal(Str, Result);\
    clear(&Str);

define_test(to_string_converts_integer_to_string)
{
    string str;
    init(&str);

    format_options<char> opt = default_format_options<char>;

    // decimal
    assert_to_string(str, "5"_cs,   1, (u8)5);
    assert_to_string(str, "0"_cs,   1, (u8)0);

    // binary
    assert_to_string(str, "101"_cs, 3, (u8)5, 0 /*offset*/, opt, integer_format_options{.base = 2, .include_prefix = false});
    assert_to_string(str, "110"_cs, 3, (u8)6, 0, opt, integer_format_options{.base = 2, .include_prefix = false});
    assert_to_string(str, "0"_cs,   1, (u8)0, 0, opt, integer_format_options{.base = 2, .include_prefix = false});

    assert_to_string(str, "0b1010"_cs, 6, (u8)0b1010, 0, opt, integer_format_options{.base = 2, .include_prefix = true});

    // octal
    assert_to_string(str, "36"_cs, 2, (u8)30, 0, opt, integer_format_options{.base = 8, .include_prefix = false});
    assert_to_string(str, "77"_cs, 2, (u8)63, 0, opt, integer_format_options{.base = 8, .include_prefix = false});
    assert_to_string(str, "0"_cs,  1, (u8)0,  0, opt, integer_format_options{.base = 8, .include_prefix = false});

    assert_to_string(str, "055"_cs, 3, (u8)055, 0, opt, integer_format_options{.base = 8, .include_prefix = true});

    // hexadecimal
    assert_to_string(str, "10"_cs, 2, (u8)0x10, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, "ff"_cs, 2, (u8)0xff, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, "0"_cs,  1, (u8)0x00, 0, opt, integer_format_options{.base = 16, .include_prefix = false});

    assert_to_string(str, "0xde"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true});
    assert_to_string(str, "0xDE"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = true});
    assert_to_string(str, "0Xde"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = false, .caps_prefix = true});
    assert_to_string(str, "0XDE"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = true,  .caps_prefix = true});

    // padding
    assert_to_string(str, "00001337"_cs,    8, (u16)0x1337, 0, opt, integer_format_options{.base = 16, .include_prefix = false, .number_pad_length = 8});
    assert_to_string(str, "0x00001337"_cs, 10, (u16)0x1337, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .number_pad_length = 8});
    assert_to_string(str, "0x1337"_cs,      6, (u16)0x1337, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .number_pad_length = 2});

    // signed
    assert_to_string(str, "-5"_cs, 2, (s8)-5);
    assert_to_string(str, "5"_cs,  1, (s8)5, 0, opt, integer_format_options{.base = 10, .include_prefix = true, .force_sign = false});
    assert_to_string(str, "+5"_cs, 2, (s8)5, 0, opt, integer_format_options{.base = 10, .include_prefix = true, .force_sign = true});

    free(&str);
}

define_test(to_string_converts_float_to_string)
{
    string str;
    init(&str);

    format_options<char> opt = default_format_options<char>;

    // float
    assert_to_string(str, "0"_cs,          1, 0.f);
    assert_to_string(str, "0"_cs,          1, 0.f, 0, opt, float_format_options{.precision = 0});
    assert_to_string(str, "0.0"_cs,        3, 0.f, 0, opt, float_format_options{.precision = 1, .ignore_trailing_zeroes = false});
    assert_to_string(str, "10"_cs,         2, 10.f, 0);
    assert_to_string(str, "1337"_cs,       4, 1337.f, 0);
    // thanks precision
    assert_to_string(str, "1231231232"_cs, 10, 1231231231.f, 0);

    assert_to_string(str, "0"_cs,      1, 0.1f, 0, opt, float_format_options{.precision = 0});
    assert_to_string(str, "0.1"_cs,    3, 0.1f, 0, opt, float_format_options{.precision = 1});
    assert_to_string(str, "0.1"_cs,    3, 0.1f, 0);
    assert_to_string(str, "0.10"_cs,   4, 0.1f, 0, opt, float_format_options{.precision = 2, .ignore_trailing_zeroes = false});
    assert_to_string(str, "0.1337"_cs, 6, 0.1337f, 0);
    assert_to_string(str, "0.133791"_cs, 8, 0.13379123f, 0); // default precision is 6

    // double
    assert_to_string(str, "0"_cs,          1, 0.0);
    assert_to_string(str, "0"_cs,          1, 0.0, 0, opt, float_format_options{.precision = 0});
    assert_to_string(str, "0.0"_cs,        3, 0.0, 0, opt, float_format_options{.precision = 1, .ignore_trailing_zeroes = false});
    assert_to_string(str, "10"_cs,         2, 10.0, 0);
    assert_to_string(str, "1337"_cs,       4, 1337.0, 0);
    // thanks precision!
    assert_to_string(str, "1231231231"_cs, 10, 1231231231.0, 0);

    assert_to_string(str, "0"_cs,      1, 0.1, 0, opt, float_format_options{.precision = 0});
    assert_to_string(str, "0.1"_cs,    3, 0.1, 0, opt, float_format_options{.precision = 1});
    assert_to_string(str, "0.1"_cs,    3, 0.1, 0);
    assert_to_string(str, "0.10"_cs,   4, 0.1, 0, opt, float_format_options{.precision = 2, .ignore_trailing_zeroes = false});
    assert_to_string(str, "0.1337"_cs, 6, 0.1337, 0);
    assert_to_string(str, "0.133791"_cs, 8, 0.13379123, 0); // default precision is 6

    assert_to_string(str, "000.1337"_cs, 8, 0.1337, 0, opt, float_format_options{.precision = 6, .number_pad_length = 3, .ignore_trailing_zeroes = true});
    assert_to_string(str, "  000.1337"_cs, 10, 0.1337, 0, format_options<char>{.pad_length = 10, .pad_char = ' '}, float_format_options{.precision = 6, .number_pad_length = 3, .ignore_trailing_zeroes = true});
    assert_to_string(str, "000.1337  "_cs, 10, 0.1337, 0, format_options<char>{.pad_length = -10, .pad_char = ' '}, float_format_options{.precision = 6, .number_pad_length = 3, .ignore_trailing_zeroes = true});
    assert_to_string(str, "-000.1337 "_cs, 10, -0.1337, 0, format_options<char>{.pad_length = -10, .pad_char = ' '}, float_format_options{.precision = 6, .number_pad_length = 3, .ignore_trailing_zeroes = true});

    free(&str);
}

define_test(to_string_converts_pointer_to_string)
{
    string str;
    init(&str);

    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);
    const char *charptr = "hello";

    assert_to_string(str, "0x00000000"_cs,  10, voidptr);
    assert_to_string(str, "0x13379001"_cs,  10, intptr);
    assert_to_string(str, "hello"_cs,        5, charptr);

    free(&str);
}

#define assert_format(Str, Result, Length, Fmt, ...)\
    assert_equal(format(&str, Fmt, __VA_ARGS__), Length);\
    assert_equal(string_length(&Str), Length);\
    assert_equal(Str[string_length(&Str)], '\0');\
    assert_equal(Str, Result);\
    clear(&Str);

define_test(format_formats_string)
{
    string str;
    init(&str);

    assert_format(str, "hello! bye"_cs, 10, "% %"_cs, "hello!"_cs, "bye");
    assert_format(str, "a%b"_cs, 3, "%\\%b"_cs, 'a');

    free(&str);
}

define_default_test_main();
