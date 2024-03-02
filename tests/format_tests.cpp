
#include <t1/t1.hpp>
#include "shl/format.hpp"

define_t1_to_string(const string &s, "%s", s.data);
define_t1_to_string(const_string s, "%s", s.c_str);

#define assert_equal_str(Str1, Str2) assert_equal(to_const_string(Str1), to_const_string(Str2))

define_test(pad_string_pads_string)
{
    string str = "abc"_s;

    // inserts a repeated character 'd' 5 times at index 0
    pad_string(&str, 'd', 5, 0);

    assert_equal_str(str, "ddddd"_cs);
    assert_equal(str.size, 5);
    assert_equal(str[5], '\0');

    pad_string(&str, 'e', 0);

    assert_equal_str(str, "ddddd"_cs);
    assert_equal(str.size, 5);
    assert_equal(str[5], '\0');

    pad_string(&str, 'e', 1);

    assert_equal_str(str, "edddd"_cs);
    assert_equal(str.size, 5);
    assert_equal(str[5], '\0');
    
    pad_string(&str, 'f', 1, 5);

    assert_equal_str(str, "eddddf"_cs);
    assert_equal(str.size, 6);
    assert_equal(str[6], '\0');

    free(&str);
}

define_test(pad_string_pads_c_string)
{
    // 6 (index 5) is null character
    // actual length is 5.
    char buf[6] = "xxxxx";

    assert_equal(pad_string(buf, 5, 'a', 0, 0), 0);
    assert_equal_str(buf, "xxxxx");

    assert_equal(pad_string(buf, 5, 'a', 1, 0), 1);
    assert_equal_str(buf, "axxxx");

    assert_equal(pad_string(buf, 5, 'a', 3, 0), 3);
    assert_equal_str(buf, "aaaxx");

    assert_equal(pad_string(buf, 5, 'b', 5, 0), 5);
    assert_equal_str(buf, "bbbbb");

    assert_equal(pad_string(buf, 5, 'c', 10, 0), 5);
    assert_equal_str(buf, "ccccc");

    assert_equal(pad_string(buf, 5, 'd', 100, 3), 2);
    assert_equal_str(buf, "cccdd");

    assert_equal(pad_string(buf, 5, 'd', 3, 5), 0);
    assert_equal_str(buf, "cccdd");

    assert_equal(pad_string(buf, 5, 'd', 3, 100), 0);
    assert_equal_str(buf, "cccdd");
}

define_test(to_string_converts_bool_to_string)
{
    string str;
    init(&str);

    format_options<char> opt = default_format_options<char>;

    assert_equal(to_string(&str, true), 1);
    assert_equal_str(str, "1"_cs);
    assert_equal(to_string(&str, false), 1);
    assert_equal_str(str, "0"_cs);
    assert_equal(to_string(&str, true, 1 /*offset*/), 1);
    assert_equal_str(str, "01"_cs);
    assert_equal(to_string(&str, true, 0 /*offset*/, opt, true /* as text */), 4);
    assert_equal_str(str, "true"_cs);
    assert_equal(to_string(&str, false, 0 /*offset*/, opt, true /* as text */), 5);
    assert_equal_str(str, "false"_cs);
    clear(&str);

    assert_equal(to_string(&str, true, 0, format_options<char>{.pad_length = 8, .pad_char = ' '}, false), 8);
    assert_equal_str(str, "       1"_cs);
    assert_equal(to_string(&str, true, 0, format_options<char>{.pad_length = 8, .pad_char = ' '}, true), 8);
    assert_equal_str(str, "    true"_cs);

    assert_equal(to_string(&str, false, 0, format_options<char>{.pad_length = -8, .pad_char = ' '}, false), 8);
    assert_equal_str(str, "0       "_cs);
    assert_equal(to_string(&str, false, 0, format_options<char>{.pad_length = -8, .pad_char = ' '}, true), 8);
    assert_equal_str(str, "false   "_cs);

    free(&str);
}

define_test(to_string_converts_bool_to_c_string)
{
    char buf[6] = "xxxxx";
    const_string str{buf, 5};

    format_options<char> opt = default_format_options<char>;

    assert_equal(to_string(buf, 5, true),  1);
    assert_equal_str(buf, "1xxxx");
    assert_equal(to_string(buf, 5, false), 1);
    assert_equal_str(buf, "0xxxx");
    assert_equal(to_string(buf, 5, true, 1 /*offset*/), 1);
    assert_equal_str(buf, "01xxx");
    assert_equal(to_string(buf, 5, true, 0 /*offset*/, opt, true /* as text */), 4);
    assert_equal_str(buf, "truex");
    assert_equal(to_string(buf, 5, false, 0 /*offset*/, opt, true /* as text */), 5);
    assert_equal_str(buf, "false");

    copy_string("xxxxx", buf);

    assert_equal(to_string(buf, 3, false, 0, opt, true), 3);
    assert_equal_str(str, "falxx");
    assert_equal(to_string(buf, 5, true, 1, opt, true), 4);
    assert_equal_str(str, "ftrue");

    copy_string("xxxxx", buf);

    assert_equal(to_string(buf, 5, true, 3, opt, true), 2);
    assert_equal_str(str, "xxxtr");

    assert_equal(to_string(buf, 5, true, 50000, opt, false), 0);
    assert_equal_str(str, "xxxtr");
}

define_test(to_string_converts_char_to_string)
{
    string str;
    init(&str);

    assert_equal(to_string(&str, 'a'), 1);
    assert_equal_str(str, "a"_cs);
    assert_equal(to_string(&str, 'b'), 1);
    assert_equal_str(str, "b"_cs);
    assert_equal(to_string(&str, 'c', 1 /*offset*/), 1);
    assert_equal_str(str, "bc"_cs);
    clear(&str);

    assert_equal(to_string(&str, 'd', 0, format_options<char>{.pad_length = 6, .pad_char = ' '}), 6);
    assert_equal_str(str, "     d"_cs);
    assert_equal(to_string(&str, 'e', 0, format_options<char>{.pad_length = -6, .pad_char = ' '}), 6);
    assert_equal_str(str, "e     "_cs);

    free(&str);
}

define_test(to_string_converts_char_to_c_string)
{
    char buf[6] = "xxxxx";
    const_string str{buf, 5};

    assert_equal(to_string(buf, 5, 'a'), 1);
    assert_equal_str(str, "axxxx"_cs);
    assert_equal(to_string(buf, 5, 'b'), 1);
    assert_equal_str(str, "bxxxx"_cs);
    assert_equal(to_string(buf, 5, 'c', 1 /*offset*/), 1);
    assert_equal_str(str, "bcxxx"_cs);

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 'd', 0, format_options<char>{.pad_length =  5, .pad_char = ' '}), 5);
    assert_equal_str(str, "    d"_cs);
    assert_equal(to_string(buf, 5, 'e', 0, format_options<char>{.pad_length = -5, .pad_char = ' '}), 5);
    assert_equal_str(str, "e    "_cs);
}

define_test(to_string_converts_string_to_string)
{
    string str;
    init(&str);

    assert_equal(to_string(&str, "hello"_cs), 5);
    assert_equal_str(str, "hello"_cs);
    assert_equal(to_string(&str, "world"_cs), 5);
    assert_equal_str(str, "world"_cs);
    assert_equal(to_string(&str, "hello"_cs, 5 /*offset*/), 5);
    assert_equal_str(str, "worldhello"_cs);

    clear(&str);
    assert_equal(to_string(&str, "hello"_cs, 0), 5);
    assert_equal_str(str, "hello"_cs);
    clear(&str);

    assert_equal(to_string(&str, "hello"_cs, 0, format_options<char>{.pad_length = 10, .pad_char = ' '}), 10);
    assert_equal_str(str, "     hello"_cs);
    assert_equal(to_string(&str, "hello"_cs, 0, format_options<char>{.pad_length = -10, .pad_char = ' '}), 10);
    assert_equal_str(str, "hello     "_cs);

    free(&str);
}

define_test(to_string_converts_string_to_c_string)
{
    char buf[11] = "xxxxxxxxxx";

    assert_equal(to_string(buf, 10, "hello"_cs), 5);
    assert_equal_str(buf, "helloxxxxx");
    assert_equal(to_string(buf, 10, "world"_cs), 5);
    assert_equal_str(buf, "worldxxxxx"_cs);
    assert_equal(to_string(buf, 10, "hello"_cs, 5 /*offset*/), 5);
    assert_equal_str(buf, "worldhello"_cs);

    assert_equal(to_string(buf, 10, "hello"_cs, 0), 5);
    assert_equal_str(buf, "hellohello");

    assert_equal(to_string(buf, 10, "world"_cs, 7), 3);
    assert_equal_str(buf, "hellohewor");

    assert_equal(to_string(buf, 10, "hello"_cs, 0, format_options<char>{.pad_length = 10, .pad_char = ' '}), 10);
    assert_equal_str(buf, "     hello");
    assert_equal(to_string(buf, 10, "hello"_cs, 0, format_options<char>{.pad_length = -10, .pad_char = ' '}), 10);
    assert_equal_str(buf, "hello     ");
}

#define assert_to_string(Str, Result, Length, ...)\
    assert_equal(to_string(&Str, __VA_ARGS__), Length);\
    assert_equal(string_length(&Str), Length);\
    assert_equal_str(Str, Result);\
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
    assert_to_string(str, "00001337"_cs,    8, (u16)0x1337, 0, format_options<char>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, "0x00001337"_cs, 10, (u16)0x1337, 0, format_options<char>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = true});
    assert_to_string(str, "0x1337"_cs,      6, (u16)0x1337, 0, format_options<char>{.precision = 2}, integer_format_options{.base = 16, .include_prefix = true});

    assert_to_string(str, "  0x00001337"_cs, 12, (s16)0x1337, 0, format_options<char>{.pad_length = 12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16,   .include_prefix = true});
    assert_to_string(str, "0x00001337  "_cs, 12, (s16)0x1337, 0, format_options<char>{.pad_length = -12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16,  .include_prefix = true});
    assert_to_string(str, "-0x00001337 "_cs, 12, (s16)-0x1337, 0, format_options<char>{.pad_length = -12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16, .include_prefix = true});

    // signed
    assert_to_string(str, "-5"_cs, 2, (s8)-5);
    assert_to_string(str, "5"_cs,  1, (s8)5, 0, format_options<char>{.sign = '\0'}, integer_format_options{.base = 10, .include_prefix = true});
    assert_to_string(str, "+5"_cs, 2, (s8)5, 0, format_options<char>{.sign = '+'}, integer_format_options{.base = 10, .include_prefix = true});

    free(&str);
}

define_test(to_string_converts_integer_to_wstring)
{
    wstring str;
    init(&str);

    format_options<wchar_t> opt = default_format_options<wchar_t>;

    // decimal
    assert_to_string(str, L"5"_cs,   1, (u8)5);
    assert_to_string(str, L"0"_cs,   1, (u8)0);

    // binary
    assert_to_string(str, L"101"_cs, 3, (u8)5, 0 /*offset*/, opt, integer_format_options{.base = 2, .include_prefix = false});
    assert_to_string(str, L"110"_cs, 3, (u8)6, 0, opt, integer_format_options{.base = 2, .include_prefix = false});
    assert_to_string(str, L"0"_cs,   1, (u8)0, 0, opt, integer_format_options{.base = 2, .include_prefix = false});

    assert_to_string(str, L"0b1010"_cs, 6, (u8)0b1010, 0, opt, integer_format_options{.base = 2, .include_prefix = true});

    // octal
    assert_to_string(str, L"36"_cs, 2, (u8)30, 0, opt, integer_format_options{.base = 8, .include_prefix = false});
    assert_to_string(str, L"77"_cs, 2, (u8)63, 0, opt, integer_format_options{.base = 8, .include_prefix = false});
    assert_to_string(str, L"0"_cs,  1, (u8)0,  0, opt, integer_format_options{.base = 8, .include_prefix = false});

    assert_to_string(str, L"055"_cs, 3, (u8)055, 0, opt, integer_format_options{.base = 8, .include_prefix = true});

    // hexadecimal
    assert_to_string(str, L"10"_cs, 2, (u8)0x10, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, L"ff"_cs, 2, (u8)0xff, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, L"0"_cs,  1, (u8)0x00, 0, opt, integer_format_options{.base = 16, .include_prefix = false});

    assert_to_string(str, L"0xde"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true});
    assert_to_string(str, L"0xDE"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = true});
    assert_to_string(str, L"0Xde"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = false, .caps_prefix = true});
    assert_to_string(str, L"0XDE"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = true,  .caps_prefix = true});

    // padding
    assert_to_string(str, L"00001337"_cs,    8, (u16)0x1337, 0, format_options<wchar_t>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, L"0x00001337"_cs, 10, (u16)0x1337, 0, format_options<wchar_t>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = true});
    assert_to_string(str, L"0x1337"_cs,      6, (u16)0x1337, 0, format_options<wchar_t>{.precision = 2}, integer_format_options{.base = 16, .include_prefix = true});

    assert_to_string(str, L"  0x00001337"_cs, 12, (s16)0x1337, 0, format_options<wchar_t>{.pad_length = 12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16,   .include_prefix = true});
    assert_to_string(str, L"0x00001337  "_cs, 12, (s16)0x1337, 0, format_options<wchar_t>{.pad_length = -12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16,  .include_prefix = true});
    assert_to_string(str, L"-0x00001337 "_cs, 12, (s16)-0x1337, 0, format_options<wchar_t>{.pad_length = -12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16, .include_prefix = true});

    // signed
    assert_to_string(str, L"-5"_cs, 2, (s8)-5);
    assert_to_string(str, L"5"_cs,  1, (s8)5, 0, format_options<wchar_t>{.sign = '\0'}, integer_format_options{.base = 10, .include_prefix = true});
    assert_to_string(str, L"+5"_cs, 2, (s8)5, 0, format_options<wchar_t>{.sign = '+'}, integer_format_options{.base = 10, .include_prefix = true});

    free(&str);
}

define_test(to_string_converts_integer_to_c_string)
{
    char buf[11] = "xxxxxxxxxx";

    assert_equal(to_string(buf, 10, 5, 0), 1);
    assert_equal_str(buf, "5xxxxxxxxx");

    assert_equal(to_string(buf, 10, 5, 9), 1);
    assert_equal_str(buf, "5xxxxxxxx5");

    assert_equal(to_string(buf, 10, 123, 0), 3);
    assert_equal_str(buf, "123xxxxxx5");

    assert_equal(to_string(buf, 10, 456, 8), 2);
    assert_equal_str(buf, "123xxxxx45");

    assert_equal(to_string(buf, 10, 789, 500), 0);
    assert_equal_str(buf, "123xxxxx45");

    assert_equal(to_string(buf, 10, 0x1337, 0, format_options<char>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = true}), 10);
    assert_equal_str(buf, "0x00001337");

    copy_string("          ", buf);
    assert_equal(to_string(buf, 10, 0x1337, 5, format_options<char>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = true}), 5);
    assert_equal_str(buf, "     0x000");
    assert_equal(to_string(buf, 10, 0x1337, 9999, format_options<char>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = true}), 0);
    assert_equal_str(buf, "     0x000");
}

define_test(to_string_converts_pointer_to_string)
{
    string str;
    init(&str);

    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);
    const char *charptr = "hello";
    const char *charptr2 = nullptr;

    assert_to_string(str, "0x00000000"_cs,  10, voidptr);
    assert_to_string(str, "0x13379001"_cs,  10, intptr);
    assert_to_string(str, "hello"_cs,        5, charptr);
    assert_to_string(str, "<null>"_cs,       6, charptr2);

    free(&str);
}

define_test(to_string_converts_pointer_to_wstring)
{
    wstring str;
    init(&str);

    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);
    const wchar_t *charptr = L"hello";

    assert_to_string(str, L"0x00000000"_cs,  10, voidptr);
    assert_to_string(str, L"0x13379001"_cs,  10, intptr);
    assert_to_string(str, L"hello"_cs,        5, charptr);

    free(&str);
}

define_test(to_string_converts_pointer_to_c_string)
{
    char buf[11] = "xxxxxxxxxx";

    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);

    assert_equal(to_string(buf, 10, voidptr, 0), 10);
    assert_equal_str(buf, "0x00000000");

    assert_equal(to_string(buf, 10, intptr, 0), 10);
    assert_equal_str(buf, "0x13379001");

    assert_equal(to_string(buf, 10, intptr, 5), 5);
    assert_equal_str(buf, "0x1330x133");
}

define_test(to_string_converts_float_to_string)
{
    string str;
    init(&str);

    // format_options<char> opt = default_format_options<char>;

    // float
    assert_to_string(str, "0"_cs,          1, 0.f);
    assert_to_string(str, "0"_cs,          1, 0.f, 0, format_options<char>{.precision = 0}, float_format_options{});
    assert_to_string(str, "0.0"_cs,        3, 0.f, 0, format_options<char>{.precision = 1}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, "10"_cs,         2, 10.f, 0);
    assert_to_string(str, "1337"_cs,       4, 1337.f, 0);
    // thanks precision
    assert_to_string(str, "1231231232"_cs, 10, 1231231231.f, 0);

    assert_to_string(str, "0"_cs,      1, 0.1f, 0, format_options<char>{.precision = 0}, float_format_options{});
    assert_to_string(str, "0.1"_cs,    3, 0.1f, 0, format_options<char>{.precision = 1}, float_format_options{});
    assert_to_string(str, "0.1"_cs,    3, 0.1f, 0);
    assert_to_string(str, "0.10"_cs,   4, 0.1f, 0, format_options<char>{.precision = 2}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, "0.1337"_cs, 6, 0.1337f, 0);
    assert_to_string(str, "0.133791"_cs, 8, 0.13379123f, 0); // default precision is 6

    // double
    assert_to_string(str, "0"_cs,          1, 0.0);
    assert_to_string(str, "0"_cs,          1, 0.0, 0, format_options<char>{.precision = 0}, float_format_options{});
    assert_to_string(str, "0.0"_cs,        3, 0.0, 0, format_options<char>{.precision = 1}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, "10"_cs,         2, 10.0, 0);
    assert_to_string(str, "1337"_cs,       4, 1337.0, 0);
    // thanks precision!
    assert_to_string(str, "1231231231"_cs, 10, 1231231231.0, 0);

    assert_to_string(str, "0"_cs,      1, 0.1, 0, format_options<char>{.precision = 0}, float_format_options{});
    assert_to_string(str, "0.1"_cs,    3, 0.1, 0, format_options<char>{.precision = 1}, float_format_options{});
    assert_to_string(str, "0.1"_cs,    3, 0.1, 0);
    assert_to_string(str, "0.10"_cs,   4, 0.1, 0, format_options<char>{.precision = 2}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, "0.1337"_cs, 6, 0.1337, 0);
    assert_to_string(str, "0.133791"_cs, 8, 0.13379123, 0); // default precision is 6

    free(&str);
}

define_test(to_string_converts_float_to_wstring)
{
    wstring str;
    init(&str);

    // format_options<wchar_t> opt = default_format_options<wchar_t>;

    // float
    assert_to_string(str, L"0"_cs,          1, 0.f);
    assert_to_string(str, L"0"_cs,          1, 0.f, 0, format_options<wchar_t>{.precision = 0}, float_format_options{});
    assert_to_string(str, L"0.0"_cs,        3, 0.f, 0, format_options<wchar_t>{.precision = 1}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, L"10"_cs,         2, 10.f, 0);
    assert_to_string(str, L"1337"_cs,       4, 1337.f, 0);
    // thanks precision
    assert_to_string(str, L"1231231232"_cs, 10, 1231231231.f, 0);

    assert_to_string(str, L"0"_cs,      1, 0.1f, 0, format_options<wchar_t>{.precision = 0}, float_format_options{});
    assert_to_string(str, L"0.1"_cs,    3, 0.1f, 0, format_options<wchar_t>{.precision = 1}, float_format_options{});
    assert_to_string(str, L"0.1"_cs,    3, 0.1f, 0);
    assert_to_string(str, L"0.10"_cs,   4, 0.1f, 0, format_options<wchar_t>{.precision = 2}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, L"0.1337"_cs, 6, 0.1337f, 0);
    assert_to_string(str, L"0.133791"_cs, 8, 0.13379123f, 0); // default precision is 6

    // double
    assert_to_string(str, L"0"_cs,          1, 0.0);
    assert_to_string(str, L"0"_cs,          1, 0.0, 0, format_options<wchar_t>{.precision = 0}, float_format_options{});
    assert_to_string(str, L"0.0"_cs,        3, 0.0, 0, format_options<wchar_t>{.precision = 1}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, L"10"_cs,         2, 10.0, 0);
    assert_to_string(str, L"1337"_cs,       4, 1337.0, 0);
    // thanks precision!
    assert_to_string(str, L"1231231231"_cs, 10, 1231231231.0, 0);

    assert_to_string(str, L"0"_cs,      1, 0.1, 0, format_options<wchar_t>{.precision = 0}, float_format_options{});
    assert_to_string(str, L"0.1"_cs,    3, 0.1, 0, format_options<wchar_t>{.precision = 1}, float_format_options{});
    assert_to_string(str, L"0.1"_cs,    3, 0.1, 0);
    assert_to_string(str, L"0.10"_cs,   4, 0.1, 0, format_options<wchar_t>{.precision = 2}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, L"0.1337"_cs, 6, 0.1337, 0);
    assert_to_string(str, L"0.133791"_cs, 8, 0.13379123, 0); // default precision is 6

    free(&str);
}

define_test(to_string_converts_float_to_c_string)
{
    char buf[6] = "     ";
    
    assert_equal(to_string(buf, 5, 12.34, 0), 5);
    assert_equal_str(buf, "12.34");

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 1), 4);
    assert_equal_str(buf, " 12.3");

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 2), 3);
    assert_equal_str(buf, "  12.");

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 3), 2);
    assert_equal_str(buf, "   12");

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 4), 1);
    assert_equal_str(buf, "    1");

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 5), 0);
    assert_equal_str(buf, "     ");

    copy_string("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 5000), 0);
    assert_equal_str(buf, "     ");
}

#define assert_format(Str, Result, Length, Fmt, ...)\
    assert_equal(format(&Str, Fmt, __VA_ARGS__), Length);\
    assert_equal(string_length(&Str), Length);\
    assert_equal(Str[string_length(&Str)], (C)'\0');\
    assert_equal_str(Str, Result);\
    clear(&Str);

define_test(format_formats_string)
{
    string str;
    init(&str);

    typedef char C;
    
    assert_format(str, "hello! "_cs, 7, "% "_cs, "hello!"_cs);
    assert_format(str, "abc h"_cs, 5, "abc %"_cs, "h"_cs);
    assert_format(str, "hello! bye"_cs, 10, "% %"_cs, "hello!"_cs, "bye");

    // escape with backslash
    assert_format(str, "a%b"_cs, 3, "%\\%b"_cs, 'a');

    // space padding
    assert_format(str, "  abc"_cs, 5, "%5"_cs, "abc");
    assert_format(str, "abc  "_cs, 5, "%-5"_cs, "abc");

    // float
    assert_format(str, "123.456"_cs, 7, "%"_cs, 123.456);
    assert_format(str, "123.456"_cs, 7, "%f"_cs, 123.456);
    assert_format(str, "123.456"_cs, 7, "%.3f"_cs, 123.456f);

    assert_format(str, "123.456"_cs,   7, "%.5f"_cs, 123.456);
    assert_format(str, "123.45600"_cs, 9, "%#.5f"_cs, 123.456);

    // int
    assert_format(str, "123"_cs, 3, "%"_cs, 123);
    assert_format(str, "1111011"_cs, 7, "%b"_cs, 123);
    assert_format(str, "173"_cs, 3, "%o"_cs, 123);
    assert_format(str, "7b"_cs, 2, "%x"_cs, 123);
    assert_format(str, "7B"_cs, 2, "%X"_cs, 123);

    assert_format(str, "0b1111011"_cs, 9, "%#b"_cs, 123);
    assert_format(str, "0b01111011"_cs, 10, "%#.8b"_cs, 123);
    assert_format(str, "0173"_cs, 4, "%#o"_cs, 123);
    assert_format(str, "0x7b"_cs, 4, "%#x"_cs, 123);
    assert_format(str, "0x7B"_cs, 4, "%#X"_cs, 123);

    // pointer
    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);

    assert_format(str, "0x00000000"_cs, 10, "%"_cs, voidptr);
    assert_format(str, "0x13379001"_cs, 10, "%"_cs, intptr);
    assert_format(str, "0x13379001"_cs, 10, "%p"_cs, intptr);

    free(&str);
}

define_test(format_formats_wstring)
{
    wstring str;
    init(&str);

    typedef wchar_t C;
    
    assert_format(str, L"hello! "_cs, 7, L"% "_cs, L"hello!"_cs);
    assert_format(str, L"abc h"_cs, 5, L"abc %"_cs, L"h"_cs);
    assert_format(str, L"hello! bye"_cs, 10, L"% %"_cs, L"hello!"_cs, L"bye");

    // escape with backslash
    assert_format(str, L"a%b"_cs, 3, L"%\\%b"_cs, L'a');

    // space padding
    assert_format(str, L"  abc"_cs, 5, L"%5"_cs, L"abc");
    assert_format(str, L"abc  "_cs, 5, L"%-5"_cs, L"abc");

    // float
    assert_format(str, L"123.456"_cs, 7, L"%"_cs, 123.456);
    assert_format(str, L"123.456"_cs, 7, L"%f"_cs, 123.456);
    assert_format(str, L"123.456"_cs, 7, L"%.3f"_cs, 123.456f);

    assert_format(str, L"123.456"_cs,   7, L"%.5f"_cs, 123.456);
    assert_format(str, L"123.45600"_cs, 9, L"%#.5f"_cs, 123.456);

    // int
    assert_format(str, L"123"_cs, 3, L"%"_cs, 123);
    assert_format(str, L"1111011"_cs, 7, L"%b"_cs, 123);
    assert_format(str, L"173"_cs, 3, L"%o"_cs, 123);
    assert_format(str, L"7b"_cs, 2, L"%x"_cs, 123);
    assert_format(str, L"7B"_cs, 2, L"%X"_cs, 123);

    assert_format(str, L"0b1111011"_cs, 9, L"%#b"_cs, 123);
    assert_format(str, L"0b01111011"_cs, 10, L"%#.8b"_cs, 123);
    assert_format(str, L"0173"_cs, 4, L"%#o"_cs, 123);
    assert_format(str, L"0x7b"_cs, 4, L"%#x"_cs, 123);
    assert_format(str, L"0x7B"_cs, 4, L"%#X"_cs, 123);

    // pointer
    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);

    assert_format(str, L"0x00000000"_cs, 10, L"%"_cs, voidptr);
    assert_format(str, L"0x13379001"_cs, 10, L"%"_cs, intptr);
    assert_format(str, L"0x13379001"_cs, 10, L"%p"_cs, intptr);

    free(&str);
}

define_test(format_formats_c_string)
{
    char buf[11] = "          ";

    assert_equal(format(buf, 10, "abc"), 3);
    assert_equal_str(buf, "abc       ");

    assert_equal(format(buf, 10, 7, "abc"), 3);
    assert_equal_str(buf, "abc    abc");

    copy_string("          ", buf);

    assert_equal(format(buf, 10, 0, "% %", 123, 45.6), 8);
    assert_equal_str(buf, "123 45.6  ");
    assert_equal(format(buf, 10, 500000, "% %", 123, 45.6), 0);
    assert_equal_str(buf, "123 45.6  ");
}

define_test(tformat_formats_to_temporary_string)
{
    u64 bufsize = get_tformat_buffer_size();

    assert_greater_or_equal(bufsize, 4096);
    assert_equal(tformat("abc"_cs), "abc"_cs);

    const_string prev = tformat("% %"_cs, "hello", "world");
    assert_equal_str(prev, "hello world"_cs);
    assert_equal(prev.c_str[prev.size], '\0');

    for (int i = 0; i < bufsize * 10; ++i)
    {
        const_string next = tformat("% %"_cs, "hello", "world");

        // we do these ifs to not flood the test count
        if (compare_strings(prev, "hello world"_cs) != 0) assert_equal_str(prev, "hello world"_cs);
        if (prev.c_str[prev.size] != '\0')                assert_equal(prev.c_str[prev.size], '\0');
        if (compare_strings(next, "hello world"_cs) != 0) assert_equal_str(next, "hello world"_cs);
        if (next.c_str[next.size] != '\0')                assert_equal(next.c_str[next.size], '\0');
        prev = next;
    }

    assert_equal(tformat("% %"_cs, "abc", "def"), "abc def"_cs)
}

define_test(format_formats_int_padding)
{
    string str;
    init(&str);

    typedef char C;
    
    assert_format(str, "ff"_cs, 2, "%x"_cs, 0xff);
    assert_format(str, "0xff"_cs, 4, "%#x"_cs, 0xff);

    assert_format(str, "ff 0x11"_cs, 7, "%x %#x"_cs, 0xff, 0x11);

    assert_format(str, "      ff 0x11"_cs, 13, "%8x %#x"_cs, 0xff, 0x11);
    assert_format(str, "000000ff 0x11"_cs, 13, "%08x %#x"_cs, 0xff, 0x11);
    assert_format(str, "000000ff 0x000011 0x00001337"_cs, 28, "%08x %#08x 0x%08x"_cs, 0xff, 0x11, 0x1337);

    free(&str);
}

define_test(new_format_creates_string_from_format)
{
    string str = new_format("%s %d %1.f", "abc", 5, 16.5f);

    assert_equal_str(str, "abc 5 16.5"_cs);
    free(&str);
}

define_default_test_main();
