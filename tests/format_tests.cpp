
#include <t1/t1.hpp>
#include "shl/format.hpp"

std::ostream& operator<<(std::ostream &lhs, const string &rhs)
{
    return lhs << '"' << rhs.data.data << '"';
}

std::ostream& operator<<(std::ostream &lhs, const_string rhs)
{
    return lhs << '"' << rhs.c_str << '"';
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
    clear(&str);

    assert_equal(to_string(&str, true, 0, format_options<char>{.pad_length = 8, .pad_char = ' '}, false), 8);
    assert_equal(str, "       1"_cs);
    assert_equal(to_string(&str, true, 0, format_options<char>{.pad_length = 8, .pad_char = ' '}, true), 8);
    assert_equal(str, "    true"_cs);

    assert_equal(to_string(&str, false, 0, format_options<char>{.pad_length = -8, .pad_char = ' '}, false), 8);
    assert_equal(str, "0       "_cs);
    assert_equal(to_string(&str, false, 0, format_options<char>{.pad_length = -8, .pad_char = ' '}, true), 8);
    assert_equal(str, "false   "_cs);

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
    clear(&str);

    assert_equal(to_string(&str, 'd', 0, format_options<char>{.pad_length = 6, .pad_char = ' '}), 6);
    assert_equal(str, "     d"_cs);
    assert_equal(to_string(&str, 'e', 0, format_options<char>{.pad_length = -6, .pad_char = ' '}), 6);
    assert_equal(str, "e     "_cs);

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

    clear(&str);
    assert_equal(to_string(&str, "hello"_cs, 0), 5);
    assert_equal(str, "hello"_cs);
    clear(&str);

    assert_equal(to_string(&str, "hello"_cs, 0, format_options<char>{.pad_length = 10, .pad_char = ' '}), 10);
    assert_equal(str, "     hello"_cs);
    assert_equal(to_string(&str, "hello"_cs, 0, format_options<char>{.pad_length = -10, .pad_char = ' '}), 10);
    assert_equal(str, "hello     "_cs);

    free(&str);
}

#define assert_to_string(Str, Result, Length, ...)\
    assert_equal(to_string(&Str, __VA_ARGS__), Length);\
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

define_test(to_string_converts_float_to_string)
{
    string str;
    init(&str);

    format_options<char> opt = default_format_options<char>;

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

    format_options<wchar_t> opt = default_format_options<wchar_t>;

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

#define assert_format(Str, Result, Length, Fmt, ...)\
    assert_equal(format(&Str, Fmt, __VA_ARGS__), Length);\
    assert_equal(string_length(&Str), Length);\
    assert_equal(Str[string_length(&Str)], (C)'\0');\
    assert_equal(Str, Result);\
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

define_test(tformat_formats_to_temporary_string)
{
    assert_equal(tformat("abc"_cs), "abc"_cs)
    assert_equal(tformat("% %"_cs, "hello", "world"), "hello world"_cs)

    for (int i = 0; i < TEMP_STRING_MAX_SIZE * 2; ++i)
        tformat("% %"_cs, "hello", "world");

    assert_equal(tformat("% %"_cs, "abc", "def"), "abc def"_cs)

}

define_default_test_main();
