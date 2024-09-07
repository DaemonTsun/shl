
#include <t1/t1.hpp>
#include "shl/format.hpp"
#include "shl/compiler.hpp"

#if GNU
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

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

    opt.pad_length = 8;
    opt.pad_char = ' ';
    assert_equal(to_string(&str, true, 0, opt, false), 8);
    assert_equal_str(str, "       1"_cs);
    assert_equal(to_string(&str, true, 0, opt, true), 8);
    assert_equal_str(str, "    true"_cs);

    opt.pad_length = -8;
    assert_equal(to_string(&str, false, 0, opt, false), 8);
    assert_equal_str(str, "0       "_cs);
    assert_equal(to_string(&str, false, 0, opt, true), 8);
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

    string_copy("xxxxx", buf);

    assert_equal(to_string(buf, 3, false, 0, opt, true), 3);
    assert_equal_str(str, "falxx");
    assert_equal(to_string(buf, 5, true, 1, opt, true), 4);
    assert_equal_str(str, "ftrue");

    string_copy("xxxxx", buf);

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

    format_options<char> opt = default_format_options<char>;
    opt.pad_length = 6;
    opt.pad_char = ' ';

    assert_equal(to_string(&str, 'd', 0, opt), 6);
    assert_equal_str(str, "     d"_cs);

    opt.pad_length = -6;
    assert_equal(to_string(&str, 'e', 0, opt), 6);
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

    string_copy("     ", buf);

    format_options<char> opt = default_format_options<char>;
    opt.pad_length = 5;
    opt.pad_char = ' ';

    assert_equal(to_string(buf, 5, 'd', 0, opt), 5);
    assert_equal_str(str, "    d"_cs);

    opt.pad_length = -5;
    assert_equal(to_string(buf, 5, 'e', 0, opt), 5);
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

    format_options<char> opt = default_format_options<char>;
    opt.pad_length = 10;
    opt.pad_char = ' ';

    assert_equal(to_string(&str, "hello"_cs, 0, opt), 10);
    assert_equal_str(str, "     hello"_cs);

    opt.pad_length = -10;
    assert_equal(to_string(&str, "hello"_cs, 0, opt), 10);
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

    format_options<char> opt = default_format_options<char>;
    opt.pad_length = 10;
    opt.pad_char = ' ';

    assert_equal(to_string(buf, 10, "hello"_cs, 0, opt), 10);
    assert_equal_str(buf, "     hello");

    opt.pad_length = -10;
    assert_equal(to_string(buf, 10, "hello"_cs, 0, opt), 10);
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
    integer_format_options iopt = default_integer_options;

    // decimal
    assert_to_string(str, "5"_cs,   1, (u8)5);
    assert_to_string(str, "0"_cs,   1, (u8)0);

    // binary
    iopt.base = 2;
    iopt.include_prefix = false;
    assert_to_string(str, "101"_cs, 3, (u8)5, 0 /*offset*/, opt, iopt);
    assert_to_string(str, "110"_cs, 3, (u8)6, 0, opt, iopt);
    assert_to_string(str, "0"_cs,   1, (u8)0, 0, opt, iopt);

    iopt.include_prefix = true;
    assert_to_string(str, "0b1010"_cs, 6, (u8)0b1010, 0, opt, iopt);

    // octal
    iopt.base = 8;
    iopt.include_prefix = false;
    assert_to_string(str, "36"_cs, 2, (u8)30, 0, opt, iopt);
    assert_to_string(str, "77"_cs, 2, (u8)63, 0, opt, iopt);
    assert_to_string(str, "0"_cs,  1, (u8)0,  0, opt, iopt);

    iopt.include_prefix = true;
    assert_to_string(str, "055"_cs, 3, (u8)055, 0, opt, iopt);

    // hexadecimal
    iopt.base = 16;
    iopt.include_prefix = false;
    assert_to_string(str, "10"_cs, 2, (u8)0x10, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, "ff"_cs, 2, (u8)0xff, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, "0"_cs,  1, (u8)0x00, 0, opt, integer_format_options{.base = 16, .include_prefix = false});

    iopt.include_prefix = true;
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

define_test(to_string_converts_integer_to_u16string)
{
    u16string str{};

    format_options<c16> opt = default_format_options<c16>;

    // decimal
    assert_to_string(str, u"5"_cs,   1, (u8)5);
    assert_to_string(str, u"0"_cs,   1, (u8)0);

    // binary
    assert_to_string(str, u"101"_cs, 3, (u8)5, 0 /*offset*/, opt, integer_format_options{.base = 2, .include_prefix = false});
    assert_to_string(str, u"110"_cs, 3, (u8)6, 0, opt, integer_format_options{.base = 2, .include_prefix = false});
    assert_to_string(str, u"0"_cs,   1, (u8)0, 0, opt, integer_format_options{.base = 2, .include_prefix = false});

    assert_to_string(str, u"0b1010"_cs, 6, (u8)0b1010, 0, opt, integer_format_options{.base = 2, .include_prefix = true});

    // octal
    assert_to_string(str, u"36"_cs, 2, (u8)30, 0, opt, integer_format_options{.base = 8, .include_prefix = false});
    assert_to_string(str, u"77"_cs, 2, (u8)63, 0, opt, integer_format_options{.base = 8, .include_prefix = false});
    assert_to_string(str, u"0"_cs,  1, (u8)0,  0, opt, integer_format_options{.base = 8, .include_prefix = false});

    assert_to_string(str, u"055"_cs, 3, (u8)055, 0, opt, integer_format_options{.base = 8, .include_prefix = true});

    // hexadecimal
    assert_to_string(str, u"10"_cs, 2, (u8)0x10, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, u"ff"_cs, 2, (u8)0xff, 0, opt, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, u"0"_cs,  1, (u8)0x00, 0, opt, integer_format_options{.base = 16, .include_prefix = false});

    assert_to_string(str, u"0xde"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true});
    assert_to_string(str, u"0xDE"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = true});
    assert_to_string(str, u"0Xde"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = false, .caps_prefix = true});
    assert_to_string(str, u"0XDE"_cs, 4, (u8)0xde, 0, opt, integer_format_options{.base = 16, .include_prefix = true, .caps_letters = true,  .caps_prefix = true});

    // padding
    assert_to_string(str, u"00001337"_cs,    8, (u16)0x1337, 0, format_options<c16>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = false});
    assert_to_string(str, u"0x00001337"_cs, 10, (u16)0x1337, 0, format_options<c16>{.precision = 8}, integer_format_options{.base = 16, .include_prefix = true});
    assert_to_string(str, u"0x1337"_cs,      6, (u16)0x1337, 0, format_options<c16>{.precision = 2}, integer_format_options{.base = 16, .include_prefix = true});

    assert_to_string(str, u"  0x00001337"_cs, 12, (s16)0x1337, 0, format_options<c16>{.pad_length = 12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16,   .include_prefix = true});
    assert_to_string(str, u"0x00001337  "_cs, 12, (s16)0x1337, 0, format_options<c16>{.pad_length = -12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16,  .include_prefix = true});
    assert_to_string(str, u"-0x00001337 "_cs, 12, (s16)-0x1337, 0, format_options<c16>{.pad_length = -12, .pad_char = ' ', .precision = 8}, integer_format_options{.base = 16, .include_prefix = true});

    // signed
    assert_to_string(str, u"-5"_cs, 2, (s8)-5);
    assert_to_string(str, u"5"_cs,  1, (s8)5, 0, format_options<c16>{.sign = '\0'}, integer_format_options{.base = 10, .include_prefix = true});
    assert_to_string(str, u"+5"_cs, 2, (s8)5, 0, format_options<c16>{.sign = '+'}, integer_format_options{.base = 10, .include_prefix = true});

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

    string_copy("          ", buf);
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

define_test(to_string_converts_pointer_to_u16string)
{
    u16string str;
    init(&str);

    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);
    const c16 *charptr = u"hello";

    assert_to_string(str, u"0x00000000"_cs,  10, voidptr);
    assert_to_string(str, u"0x13379001"_cs,  10, intptr);
    assert_to_string(str, u"hello"_cs,        5, charptr);

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

define_test(to_string_converts_float_to_u16string)
{
    u16string str;
    init(&str);

    // format_options<c16> opt = default_format_options<c16>;

    // float
    assert_to_string(str, u"0"_cs,          1, 0.f);
    assert_to_string(str, u"0"_cs,          1, 0.f, 0, format_options<c16>{.precision = 0}, float_format_options{});
    assert_to_string(str, u"0.0"_cs,        3, 0.f, 0, format_options<c16>{.precision = 1}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, u"10"_cs,         2, 10.f, 0);
    assert_to_string(str, u"1337"_cs,       4, 1337.f, 0);
    // thanks precision
    assert_to_string(str, u"1231231232"_cs, 10, 1231231231.f, 0);

    assert_to_string(str, u"0"_cs,      1, 0.1f, 0, format_options<c16>{.precision = 0}, float_format_options{});
    assert_to_string(str, u"0.1"_cs,    3, 0.1f, 0, format_options<c16>{.precision = 1}, float_format_options{});
    assert_to_string(str, u"0.1"_cs,    3, 0.1f, 0);
    assert_to_string(str, u"0.10"_cs,   4, 0.1f, 0, format_options<c16>{.precision = 2}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, u"0.1337"_cs, 6, 0.1337f, 0);
    assert_to_string(str, u"0.133791"_cs, 8, 0.13379123f, 0); // default precision is 6

    // double
    assert_to_string(str, u"0"_cs,          1, 0.0);
    assert_to_string(str, u"0"_cs,          1, 0.0, 0, format_options<c16>{.precision = 0}, float_format_options{});
    assert_to_string(str, u"0.0"_cs,        3, 0.0, 0, format_options<c16>{.precision = 1}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, u"10"_cs,         2, 10.0, 0);
    assert_to_string(str, u"1337"_cs,       4, 1337.0, 0);
    // thanks precision!
    assert_to_string(str, u"1231231231"_cs, 10, 1231231231.0, 0);

    assert_to_string(str, u"0"_cs,      1, 0.1, 0, format_options<c16>{.precision = 0}, float_format_options{});
    assert_to_string(str, u"0.1"_cs,    3, 0.1, 0, format_options<c16>{.precision = 1}, float_format_options{});
    assert_to_string(str, u"0.1"_cs,    3, 0.1, 0);
    assert_to_string(str, u"0.10"_cs,   4, 0.1, 0, format_options<c16>{.precision = 2}, float_format_options{.ignore_trailing_zeroes = false});
    assert_to_string(str, u"0.1337"_cs, 6, 0.1337, 0);
    assert_to_string(str, u"0.133791"_cs, 8, 0.13379123, 0); // default precision is 6

    free(&str);
}

define_test(to_string_converts_float_to_c_string)
{
    char buf[6] = "     ";
    
    assert_equal(to_string(buf, 5, 12.34, 0), 5);
    assert_equal_str(buf, "12.34");

    string_copy("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 1), 4);
    assert_equal_str(buf, " 12.3");

    string_copy("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 2), 3);
    assert_equal_str(buf, "  12.");

    string_copy("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 3), 2);
    assert_equal_str(buf, "   12");

    string_copy("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 4), 1);
    assert_equal_str(buf, "    1");

    string_copy("     ", buf);

    assert_equal(to_string(buf, 5, 12.34, 5), 0);
    assert_equal_str(buf, "     ");

    string_copy("     ", buf);

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

define_test(format_formats_u16string)
{
    u16string str;
    init(&str);

    typedef c16 C;
    
    assert_format(str, u"hello! "_cs, 7, u"% "_cs, u"hello!"_cs);
    assert_format(str, u"abc h"_cs, 5, u"abc %"_cs, u"h"_cs);
    assert_format(str, u"hello! bye"_cs, 10, u"% %"_cs, u"hello!"_cs, u"bye");

    // escape with backslash
    assert_format(str, u"a%b"_cs, 3, u"%\\%b"_cs, u'a');

    // space padding
    assert_format(str, u"  abc"_cs, 5, u"%5"_cs, u"abc");
    assert_format(str, u"abc  "_cs, 5, u"%-5"_cs, u"abc");

    // float
    assert_format(str, u"123.456"_cs, 7, u"%"_cs, 123.456);
    assert_format(str, u"123.456"_cs, 7, u"%f"_cs, 123.456);
    assert_format(str, u"123.456"_cs, 7, u"%.3f"_cs, 123.456f);

    assert_format(str, u"123.456"_cs,   7, u"%.5f"_cs, 123.456);
    assert_format(str, u"123.45600"_cs, 9, u"%#.5f"_cs, 123.456);

    // int
    assert_format(str, u"123"_cs, 3, u"%"_cs, 123);
    assert_format(str, u"1111011"_cs, 7, u"%b"_cs, 123);
    assert_format(str, u"173"_cs, 3, u"%o"_cs, 123);
    assert_format(str, u"7b"_cs, 2, u"%x"_cs, 123);
    assert_format(str, u"7B"_cs, 2, u"%X"_cs, 123);

    assert_format(str, u"0b1111011"_cs, 9, u"%#b"_cs, 123);
    assert_format(str, u"0b01111011"_cs, 10, u"%#.8b"_cs, 123);
    assert_format(str, u"0173"_cs, 4, u"%#o"_cs, 123);
    assert_format(str, u"0x7b"_cs, 4, u"%#x"_cs, 123);
    assert_format(str, u"0x7B"_cs, 4, u"%#X"_cs, 123);

    // pointer
    void *voidptr = nullptr;
    int *intptr = reinterpret_cast<int*>(0x13379001);

    assert_format(str, u"0x00000000"_cs, 10, u"%"_cs, voidptr);
    assert_format(str, u"0x13379001"_cs, 10, u"%"_cs, intptr);
    assert_format(str, u"0x13379001"_cs, 10, u"%p"_cs, intptr);

    free(&str);
}

define_test(format_formats_c_string)
{
    char buf[11] = "          ";

    assert_equal(format(buf, 10, "abc"), 3);
    assert_equal_str(buf, "abc       ");

    assert_equal(format(buf, 10, 7, "abc"), 3);
    assert_equal_str(buf, "abc    abc");

    string_copy("          ", buf);

    assert_equal(format(buf, 10, 0, "% %", 123, 45.6), 8);
    assert_equal_str(buf, "123 45.6  ");
    assert_equal(format(buf, 10, 500000, "% %", 123, 45.6), 0);
    assert_equal_str(buf, "123 45.6  ");
}

define_test(tformat_formats_to_temporary_string)
{
    s64 bufsize = get_tformat_buffer_size();

    assert_greater_or_equal(bufsize, 4096);
    assert_equal(tformat("abc"_cs), "abc"_cs);

    const_string prev = tformat("% %"_cs, "hello", "world");
    assert_equal_str(prev, "hello world"_cs);
    assert_equal(prev.c_str[prev.size], '\0');

    for (s64 i = 0; i < bufsize * 10; ++i)
    {
        const_string next = tformat("% %"_cs, "hello", "world");
        bool failed = false;

        // we do these ifs to not flood the test count
        if (string_compare(prev, "hello world"_cs) != 0)
        {
            assert_equal_str(prev, "hello world"_cs);
            failed = true;
        }

        if (prev.c_str[prev.size] != '\0') 
        {
            assert_equal(prev.c_str[prev.size], '\0');
            failed = true;
        }

        if (string_compare(next, "hello world"_cs) != 0)
        {
            assert_equal_str(next, "hello world"_cs);
            failed = true;
        }

        if (next.c_str[next.size] != '\0')
        {
            assert_equal(next.c_str[next.size], '\0');
            failed = true;
        }

        if (failed)
            break;

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
    string str = new_format("%s %d %1.f %u", "abc", 5, 16.5f, 32u);

    assert_equal_str(str, "abc 5 16.5 32"_cs);
    free(&str);
}

define_test(format_only_escapes_percent_backslashes)
{
    // no escape
    assert_equal_str(tformat(R"(abc\def)"),    R"(abc\def)");
    assert_equal_str(tformat(R"(abc\\def)"),   R"(abc\\def)");
    assert_equal_str(tformat(R"(abc\\\def)"),  R"(abc\\\def)");
    assert_equal_str(tformat(R"(abc\\\\def)"), R"(abc\\\\def)");

    // escape
    assert_equal_str(tformat(R"(abc\%def)"),    R"(abc%def)");
    assert_equal_str(tformat(R"(abc\\\%def)"),  R"(abc\\%def)");
    assert_equal_str(tformat(R"(abc\%0def)"),   R"(abc%0def)");
    assert_equal_str(tformat(R"(/* \% */)"),    R"(/* % */)");

    char buf[32] = {0};
    format(buf, 32, R"(/* \%0% \%08x \%08x */)", 5);
    assert_equal_str((const c8*)buf, R"(/* %05 %08x %08x */)");

    // no escape
    assert_equal_str(tformat(R"(abc\\% def)", 123),   R"(abc\\123 def)");
    assert_equal_str(tformat(R"(abc\\\\% def)", 123), R"(abc\\\\123 def)");

}

define_test(format_padding_pads_string)
{
    assert_equal_str(tformat("%6s", "hello"),  " hello");
    assert_equal_str(tformat("%-6s", "hello"), "hello ");
    assert_equal_str(tformat("%7s", "hello"),  "  hello");
    assert_equal_str(tformat("%-7s", "hello"), "hello  ");
    assert_equal_str(tformat("%8s", "hello"),  "   hello");
    assert_equal_str(tformat("%-8s", "hello"), "hello   ");
    assert_equal_str(tformat("%9s", "hello"),  "    hello");
    assert_equal_str(tformat("%-9s", "hello"), "hello    ");
    assert_equal_str(tformat("%10s", "hello"),  "     hello");
    assert_equal_str(tformat("%-10s", "hello"), "hello     ");
    assert_equal_str(tformat("%11s", "hello"),  "      hello");
    assert_equal_str(tformat("%-11s", "hello"), "hello      ");
}

define_test(format_alt_formats_hex)
{
    assert_equal_str(tformat("%#x", 0),     "0");
    assert_equal_str(tformat("%#x", 1),     "0x1");
    assert_equal_str(tformat("%#x", 255),   "0xff");
    assert_equal_str(tformat("%#x", 65535), "0xffff");
    assert_equal_str(tformat("%#x", 65535), "0xffff");
    assert_equal_str(tformat("%#x", U32_MAX), "0xffffffff");
}

define_default_test_main();
