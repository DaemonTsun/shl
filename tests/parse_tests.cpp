
#include <iostream>
#include <string.h>

#include <t1/t1.hpp>
#include "../src/parse.hpp"

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
    

define_test(skip_whitespace_skips_whitespace)
{
    SETUP("   abc");

    it = skip_whitespace(it, input, input_size);

    assert_equal(it.i, 3);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 4);
}

define_test(skip_whitespace_skips_whitespace2)
{
    SETUP(R"=(

 abc)=");

    it = skip_whitespace(it, input, input_size);

    assert_equal(it.i, 3);
    assert_equal(it.line_start, 2);
    assert_equal(it.line, 3);
    assert_equal(it.line_pos, 2);
}

define_test(skip_whitespace_doesnt_skip_nonwhitespace)
{
    SETUP("abc    def");

    it = skip_whitespace(it, input, input_size);

    assert_equal(it.i, 0);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 1);
}

define_test(parse_string_throws_on_nullptr)
{
    SETUP(nullptr);

    std::string *out = nullptr;

    assert_error(it = parse_string(it, input, input_size, out), parse_error<>)
    {
        assert_equal(err.it.i, 0);
        assert_equal(err.input, nullptr);
    }
}

define_test(parse_string_throws_on_invalid_input)
{
    SETUP("abc");

    std::string out;

    assert_error(it = parse_string(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 0);
        assert_equal(err.input, "abc");
    }
}

define_test(parse_string_throws_on_unterminated_string)
{
    SETUP("\"abc");

    std::string out;

    assert_error(it = parse_string(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 4);
        assert_equal(err.input, "\"abc");
    }
}

define_test(parse_string_throws_on_unterminated_string2)
{
    SETUP("\"abc\\\"");

    std::string out;

    assert_error(it = parse_string(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 6);
        assert_equal(err.input, "\"abc\\\"");
    }
}

define_test(parse_string_parses_string)
{
    SETUP("\"\"");

    std::string out;

    it = parse_string(it, input, input_size, &out);
    assert_equal(it.i, 2);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 3);
    assert_equal(out, "\"\""s);
}

define_test(parse_string_parses_string2)
{
    SETUP("\"abc\"");

    std::string out;

    it = parse_string(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, str(input));
}

define_test(parse_string_parses_string3)
{
    SETUP("\"\n\nabc\n  \"");

    std::string out;

    it = parse_string(it, input, input_size, &out);
    assert_equal(it.i, 10);
    assert_equal(it.line_start, 7);
    assert_equal(it.line, 4);
    assert_equal(it.line_pos, 4);
    assert_equal(out, str(input));
}

define_test(parse_integer_parses_integer)
{
    SETUP("1234");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, 1234);
}

define_test(parse_integer_parses_integer2)
{
    SETUP("1234abcde");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, 1234);
}

define_test(parse_integer_parses_integer3)
{
    SETUP("-5678");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, -5678);
}

define_test(parse_integer_parses_hex_integer)
{
    SETUP("0xff");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, 255);
}

define_test(parse_integer_parses_hex_integer2)
{
    SETUP("0xffghi");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, 255);
}

define_test(parse_integer_parses_hex_integer3)
{
    SETUP("0xdeAdBEeF");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 10);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 11);
    assert_equal(out, 3735928559);
}

define_test(parse_integer_parses_hex_integer4)
{
    SETUP("-0xdeAdBEeF");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 11);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 12);
    assert_equal(out, -3735928559);
}

define_test(parse_integer_parses_hex_integer5)
{
    SETUP("feef");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, 65263);
}

// TODO: failing parse_integer tests

define_default_test_main();
