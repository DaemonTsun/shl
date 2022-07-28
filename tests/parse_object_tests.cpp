
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

    assert_equal(obj.type, parsed_object_type::Bool);
    assert_equal(obj.data.boolean, true);
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

    assert_equal(obj.type, parsed_object_type::String);
    assert_equal(obj.data.string, "hello world"s);
}

define_default_test_main();
