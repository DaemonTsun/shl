
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

define_default_test_main();
