
#include <iostream>
#include <string.h>

#include <t1/t1.hpp>
#include "../src/parse_object.hpp"

using namespace std::literals;

#define SETUP(STR) \
    const char *input = STR;\
    size_t input_size = input == nullptr ? 0 : strlen(input);\
    parse_iterator it;

#define assert_error(EXPR, ERR) \
    assert_throws(EXPR, ERR);\
    \
    try\
    {\
        EXPR;\
    }\
    catch (ERR &err)

define_test(parse_object_parses_bool)
{
    SETUP("true");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);

    assert_equal(obj.has_value<bool>(), true);
    assert_equal(obj.get<bool>(), true);
}

define_test(parse_object_parses_identifier)
{
    SETUP("truee");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);

    assert_equal(obj.has_value<parsed_identifier<char>>(), true);
    assert_equal(obj.get<parsed_identifier<char>>().value, "truee"s);
}

define_test(parse_object_parses_string)
{
    SETUP("   \"hello world\"");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 16);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 17);

    assert_equal(obj.has_value<std::string>(), true);
    assert_equal(obj.get<std::string>(), "hello world"s);
}

define_test(parse_object_parses_number_over_identifier)
{
    SETUP(" /* comment */  deadbeef");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 24);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 25);

    assert_equal(obj.has_value<s64>(), true);
    assert_equal(obj.get<s64>(), 0xdeadbeef);

    s64 x = (s64)obj;
    assert_equal(x, 0xdeadbeef);
}

define_test(parse_object_parses_object_list)
{
    SETUP("[1,2,3]");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 7);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 8);

    assert_equal(obj.has_value<object_list>(), true);

    auto &list = obj.get<object_list>();

    assert_equal(list.size(), 3);

    auto &x1 = list[0];
    auto &x2 = list[1];
    auto &x3 = list[2];

    assert_equal(x1.has_value<s64>(), true);
    assert_equal(x2.has_value<s64>(), true);
    assert_equal(x3.has_value<s64>(), true);
    assert_equal(x1.get<s64>(), 1);
    assert_equal(x2.get<s64>(), 2);
    assert_equal(x3.get<s64>(), 3);
}

define_test(parse_object_parses_object_list2)
{
    SETUP("  [  1, 2,3 /*123*/ ] a");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 21);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 22);

    assert_equal(obj.has_value<object_list>(), true);

    auto &list = obj.get<object_list>();

    assert_equal(list.size(), 3);

    auto &x1 = obj[0];
    auto &x2 = obj[1];
    auto &x3 = obj[2];

    assert_equal(x1.has_value<s64>(), true);
    assert_equal(x2.has_value<s64>(), true);
    assert_equal(x3.has_value<s64>(), true);
    assert_equal(x1.get<s64>(), 1);
    assert_equal(x2.get<s64>(), 2);
    assert_equal(x3.get<s64>(), 3);
}

define_test(parse_object_parses_object_list3)
{
    SETUP("[1, \"2\", 3]");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 11);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 12);

    assert_equal(obj.has_value<object_list>(), true);

    auto &list = obj.get<object_list>();

    assert_equal(list.size(), 3);

    auto &x1 = obj[0];
    auto &x2 = obj[1];
    auto &x3 = obj[2];

    assert_equal(x1.has_value<s64>(), true);
    assert_equal(x2.has_value<std::string>(), true);
    assert_equal(x3.has_value<s64>(), true);
    assert_equal(x1.get<s64>(), 1);
    assert_equal(x2.get<std::string>(), "2"s);
    assert_equal(x3.get<s64>(), 3);
}

define_test(parse_object_parses_object_list4)
{
    SETUP("[1, \"2\", [[3], true]]");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 21);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 22);

    assert_equal(obj.has_value<object_list>(), true);

    auto &list = obj.get<object_list>();

    assert_equal(list.size(), 3);

    auto &x1 = list[0];
    auto &x2 = list[1];
    auto &x3 = list[2];

    assert_equal(x1.has_value<s64>(), true);
    assert_equal(x2.has_value<std::string>(), true);
    assert_equal(x3.has_value<object_list>(), true);
    assert_equal(x1.get<s64>(), 1);
    assert_equal(x2.get<std::string>(), "2"s);

    auto &x3l = x3.get<object_list>();

    assert_equal(x3l.size(), 2);

    auto &y1 = x3l[0];
    auto &y2 = x3l[1];

    assert_equal(y1.has_value<object_list>(), true);
    assert_equal(y2.has_value<bool>(), true);
    assert_equal((bool)y2, true);

    auto &y1l = y1.get<object_list>();

    assert_equal(y1l.size(), 1);

    auto &z = y1l[0];

    assert_equal(z.has_value<s64>(), true);
    assert_equal((s64)z, 3);
}

define_test(parse_object_parses_object_list5)
{
    SETUP("[]");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 2);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 3);

    assert_equal(obj.has_value<object_list>(), true);

    auto &list = obj.get<object_list>();

    assert_equal(list.size(), 0);
}

define_test(parse_object_throws_on_unterminated_list)
{
    SETUP("[1,2");

    parsed_object obj;

    assert_error(it = parse_object(it, input, input_size, &obj), parse_error<>)
    {
        assert_equal(err.it.i, 4);
        assert_equal(err.it.line_start, 0);
        assert_equal(err.it.line, 1);
        assert_equal(err.it.line_pos, 5);
    }
}

define_test(parse_object_throws_on_unterminated_list2)
{
    SETUP("[1,");

    parsed_object obj;

    assert_error(it = parse_object(it, input, input_size, &obj), parse_error<>)
    {
        assert_equal(err.it.i, 3);
        assert_equal(err.it.line_start, 0);
        assert_equal(err.it.line, 1);
        assert_equal(err.it.line_pos, 4);
    }
}

define_test(parse_object_throws_on_invalid_input)
{
    SETUP(" ]");

    parsed_object obj;

    assert_error(it = parse_object(it, input, input_size, &obj), parse_error<>)
    {
        assert_equal(err.it.i, 1);
        assert_equal(err.it.line_start, 0);
        assert_equal(err.it.line, 1);
        assert_equal(err.it.line_pos, 2);
    }
}

define_test(parse_object_parses_object_table)
{
    SETUP("{}");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 2);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 3);

    assert_equal(obj.has_value<object_table>(), true);

    auto &tab = obj.get<object_table>();

    assert_equal(tab.size(), 0);
}

define_test(parse_object_parses_object_table2)
{
    SETUP("{a:1}");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);

    assert_equal(obj.has_value<object_table>(), true);

    auto &tab = obj.get<object_table>();

    assert_equal(tab.size(), 1);

    auto a = obj["a"];

    assert_equal(a.has_value<s64>(), true);
    assert_equal((s64)a, 1);
}

define_test(parse_object_parses_object_table3)
{
    SETUP("{a:1; b : \"hello\"}");

    parsed_object obj;

    it = parse_object(it, input, input_size, &obj);

    assert_equal(it.i, 18);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 19);

    assert_equal(obj.has_value<object_table>(), true);

    auto &tab = obj.get<object_table>();

    assert_equal(tab.size(), 2);

    auto a = obj["a"];
    auto b = obj["b"];

    assert_equal(a.has_value<s64>(), true);
    assert_equal((s64)a, 1);

    assert_equal(b.has_value<std::string>(), true);
    assert_equal((std::string)b, "hello"s);
}

define_default_test_main();
