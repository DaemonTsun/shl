
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
    string str;
    init(&str);

    assert_equal(to_string(&str, (u8)5), 1);
    assert_equal(string_length(&str), 1);
    assert_equal(str, "5"_cs);
    clear(&str);

    assert_equal(to_string(&str, (u8)5, 0 /*offset*/, 2 /*base*/), 3);
    assert_equal(string_length(&str), 3);
    assert_equal(str, "101"_cs);
    clear(&str);

    assert_equal(to_string(&str, (u8)6, 0 /*offset*/, 2 /*base*/), 3);
    assert_equal(string_length(&str), 3);
    assert_equal(str, "110"_cs);
    clear(&str);

    assert_equal(to_string(&str, (u8)30, 0 /*offset*/, 8 /*base*/), 2);
    assert_equal(string_length(&str), 2);
    assert_equal(str, "36"_cs);
    clear(&str);

    assert_equal(to_string(&str, (u8)63, 0 /*offset*/, 8 /*base*/), 2);
    assert_equal(string_length(&str), 2);
    assert_equal(str, "77"_cs);
    clear(&str);

    assert_equal(to_string(&str, (u8)0x10, 0 /*offset*/, 16 /*base*/), 2);
    assert_equal(string_length(&str), 2);
    assert_equal(str, "10"_cs);
    clear(&str);

    assert_equal(to_string(&str, (u8)0xff, 0 /*offset*/, 16 /*base*/), 2);
    assert_equal(string_length(&str), 2);
    assert_equal(str, "ff"_cs);
    clear(&str);

    free(&str);
}

define_default_test_main();
