
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
    
define_test(skip_whitespace_does_nothing_on_nullptr)
{
    SETUP(nullptr);

    it = skip_whitespace(it, input, input_size);

    assert_equal(it.i, 0);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 1);
}
    
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

define_test(parse_comment_parses_nullptr)
{
    SETUP(nullptr);

    it = parse_comment(it, input, input_size);

    assert_equal(it.i, 0);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 1);
}

define_test(parse_comment_parses_empty_string)
{
    SETUP("");

    it = parse_comment(it, input, input_size);

    assert_equal(it.i, 0);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 1);
}

define_test(parse_comment_parses_line_comment)
{
    SETUP("// hello");

    std::string out;
    bool success;
    it = parse_comment(it, input, input_size, &out, &success);

    assert_equal(it.i, 8);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 9);

    assert_equal(success, true);
    assert_equal(out, " hello"s);
}

define_test(parse_comment_parses_line_comment2)
{
    SETUP("// hello\n world");

    std::string out;
    bool success;
    it = parse_comment(it, input, input_size, &out, &success);

    assert_equal(it.i, 9);
    assert_equal(it.line_start, 9);
    assert_equal(it.line, 2);
    assert_equal(it.line_pos, 1);

    assert_equal(success, true);
    assert_equal(out, " hello\n"s);
}

define_test(parse_comment_parses_block_comment)
{
    SETUP("/* abc def */");

    parse_range out;
    bool success;
    it = parse_comment(it, input, input_size, &out, &success);

    assert_equal(it.i, 13);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 14);

    assert_equal(success, true);

    assert_equal(out.start.i, 2);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 3);

    assert_equal(out.end.i, 11);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 12);

    auto str = slice(input, &out);
    assert_equal(str, " abc def "s);
}

define_test(parse_comment_parses_block_comment2)
{
    SETUP("/*\nhello world\n*/");

    parse_range out;
    bool success;
    it = parse_comment(it, input, input_size, &out, &success);

    assert_equal(it.i, 17);
    assert_equal(it.line_start, 15);
    assert_equal(it.line, 3);
    assert_equal(it.line_pos, 3);

    assert_equal(success, true);

    assert_equal(out.start.i, 2);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 3);

    assert_equal(out.end.i, 15);
    assert_equal(out.end.line_start, 15);
    assert_equal(out.end.line, 3);
    assert_equal(out.end.line_pos, 1);

    auto str = slice(input, &out);
    assert_equal(str, "\nhello world\n"s);
}

define_test(skip_whitespace_and_comments_skips_whitespace)
{
    SETUP("   abc def");
    //        ^

    it = skip_whitespace_and_comments(it, input, input_size);

    assert_equal(it.i, 3);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 4);
}

define_test(skip_whitespace_and_comments_skips_comment)
{
    SETUP("/*\nhello world\n*/  abc");
    //                          ^

    it = skip_whitespace_and_comments(it, input, input_size);

    assert_equal(it.i, 19);
    assert_equal(it.line_start, 15);
    assert_equal(it.line, 3);
    assert_equal(it.line_pos, 5);
}

define_test(skip_whitespace_and_comments_skips_comments)
{
    SETUP(" /*\nhello world\n*/  // this is a line comment\n /*yep*/abc");
    //                                                             ^

    it = skip_whitespace_and_comments(it, input, input_size);

    assert_equal(it.i, 54);
    assert_equal(input[it.i], 'a');
    assert_equal(it.line_start, 46);
    assert_equal(it.line, 4);
    assert_equal(it.line_pos, 9);
}

define_test(parse_string_throws_on_invalid_input)
{
    SETUP("abc");

    parse_range out;
    parse_error err;
    
    it = parse_string(it, input, input_size, &out, &err);

    assert_error(throw err, parse_error<>)
    {
        assert_equal(err.it.i, 0);
        assert_equal(err.input, "abc");
    }
}

define_test(parse_string_throws_on_unterminated_string)
{
    SETUP("\"abc");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err);

    assert_error(throw err, parse_error<>)
    {
        assert_equal(err.it.i, 4);
        assert_equal(err.input, "\"abc");
    }
}

define_test(parse_string_throws_on_unterminated_string2)
{
    SETUP("\"abc\\\"");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err);

    assert_error(throw err, parse_error<>)
    {
        assert_equal(err.it.i, 6);
        assert_equal(err.input, "\"abc\\\"");
    }
}

define_test(parse_string_parses_string)
{
    SETUP("\"\"");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err);

    assert_equal(err.success, true);

    assert_equal(it.i, 2);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 3);

    assert_equal(out.start.i, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.i, 1);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 2);

    std::string str = slice(input, &out);
    assert_equal(str, ""s);
}

define_test(parse_string_parses_string2)
{
    SETUP("\"abc\"");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err);

    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);

    assert_equal(out.start.i, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.i, 4);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 5);

    std::string str = slice(input, &out);
    assert_equal(str, "abc"s);
}

define_test(parse_string_parses_string3)
{
    SETUP("\"\nabc\n\"");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err);

    assert_equal(it.i, 7);
    assert_equal(it.line_start, 6);
    assert_equal(it.line, 3);
    assert_equal(it.line_pos, 2);

    assert_equal(out.start.i, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.i, 6);
    assert_equal(out.end.line_start, 6);
    assert_equal(out.end.line, 3);
    assert_equal(out.end.line_pos, 1);

    std::string str = slice(input, &out);
    assert_equal(str, "\nabc\n"s);
}

define_test(parse_string_parses_string4)
{
    SETUP("\"abc\"");

    std::string out;

    it = parse_string(it, input, input_size, &out);

    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, "abc"s);
}

define_test(parse_string_parses_string_with_delims)
{
    SETUP("\"hello\"   ");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err, '"', true);

    assert_equal(it.i, 7);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 8);

    assert_equal(out.start.i, 0);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 1);

    assert_equal(out.end.i, 7);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 8);

    std::string str = slice(input, &out);
    assert_equal(str, "\"hello\""s);
}

define_test(parse_string_parses_string_with_delims2)
{
    SETUP("\"hello world\\\" \"  ");

    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err, '"', true);

    assert_equal(it.i, 16);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 17);

    assert_equal(out.start.i, 0);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 1);

    assert_equal(out.end.i, 16);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 17);

    std::string str = slice(input, &out);
    assert_equal(str, "\"hello world\\\" \""s);
}

define_test(parse_string_parses_string_delims)
{
    SETUP("x abc XYZ x");
    parse_range out;
    parse_error err;

    it = parse_string(it, input, input_size, &out, &err, 'x');

    assert_equal(it.i, 11);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 12);

    assert_equal(out.start.i, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.i, 10);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 11);

    std::string str = slice(input, &out);
    assert_equal(str, " abc XYZ "s);
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

define_test(parse_integer_parses_unsigned_integer)
{
    SETUP("123123123123");

    u64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 12);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 13);
    assert_equal(out, 123123123123ull);
}

define_test(parse_integer_parses_unsigned_integer2)
{
    SETUP("-1");

    u64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 2);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 3);
    assert_equal(out, std::numeric_limits<u64>::max());
}

define_test(parse_integer_parses_binary_integer)
{
    SETUP("0b1010");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 6);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 7);
    assert_equal(out, 10);
}

define_test(parse_integer_parses_binary_integer2)
{
    SETUP("0b101014");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 7);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 8);
    assert_equal(out, 0b10101);
}

define_test(parse_integer_parses_octal_integer)
{
    SETUP("01234");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, 668);
}

define_test(parse_integer_parses_octal_integer2)
{
    SETUP("0123456789");

    s64 out;

    it = parse_integer(it, input, input_size, &out);
    assert_equal(it.i, 8);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 9);
    assert_equal(out, 01234567);
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

define_test(parse_integer_throws_on_invalid_input)
{
    SETUP("xyz");

    s64 out;

    assert_error(it = parse_integer(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 0);
        assert_equal(err.input, "xyz");
    }
}

define_test(parse_integer_throws_on_invalid_input2)
{
    SETUP("0x");

    s64 out;

    assert_error(it = parse_integer(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 2);
        assert_equal(err.input, "0x");
    }
}

define_test(parse_integer_throws_on_invalid_input3)
{
    SETUP("0b");

    s64 out;

    assert_error(it = parse_integer(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 2);
        assert_equal(err.input, "0b");
    }
}

define_test(parse_integer_throws_on_invalid_input4)
{
    SETUP("0xZ");

    s64 out;

    assert_error(it = parse_integer(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 2);
        assert_equal(err.input, "0xZ");
    }
}

define_test(parse_decimal_parses_float)
{
    SETUP("1.0");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 3);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 4);
    assert_equal(out, 1.0f);
}

define_test(parse_decimal_parses_float2)
{
    SETUP("+.06");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, .06f);
}

define_test(parse_decimal_parses_float3)
{
    SETUP("-0.06");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, -0.06f);
}

define_test(parse_decimal_parses_float4)
{
    SETUP("1.33e+10");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 8);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 9);
    assert_equal(out, 1.33e+10f);
}

define_test(parse_decimal_parses_float5)
{
    SETUP("2.5E-12");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 7);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 8);
    assert_equal(out, 2.5E-12f);
}

define_test(parse_decimal_parses_float6)
{
    SETUP("12341234");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 8);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 9);
    assert_equal(out, 12341234.f);
}

define_test(parse_decimal_parses_float7)
{
    SETUP("1234e5");

    float out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 6);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 7);
    assert_equal(out, 1234e5);
}

define_test(parse_decimal_parses_double)
{
    SETUP("1.0");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 3);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 4);
    assert_equal(out, 1.0);
}

define_test(parse_decimal_parses_double2)
{
    SETUP("+.06");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, .06);
}

define_test(parse_decimal_parses_double3)
{
    SETUP("-0.06X");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, -0.06);
}

define_test(parse_decimal_parses_double4)
{
    SETUP("1.33e+10");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 8);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 9);
    assert_equal(out, 1.33e+10);
}

define_test(parse_decimal_parses_double5)
{
    SETUP("2.5E-12X");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 7);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 8);
    assert_equal(out, 2.5E-12);
}

define_test(parse_decimal_parses_double6)
{
    SETUP("12341234.X");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 9);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 10);
    assert_equal(out, 12341234.);
}

define_test(parse_decimal_parses_double7)
{
    SETUP("1234e5X");

    double out;

    it = parse_decimal(it, input, input_size, &out);
    assert_equal(it.i, 6);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 7);
    assert_equal(out, 1234e5);
}

define_test(parse_decimal_throw_on_invalid_input)
{
    SETUP("z");

    float out;

    assert_error(it = parse_decimal(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 0);
    }
}

define_test(parse_decimal_throw_on_invalid_input2)
{
    SETUP(".");

    float out;

    assert_error(it = parse_decimal(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 1);
    }
}

define_test(parse_decimal_throw_on_invalid_input3)
{
    SETUP("e");

    float out;

    assert_error(it = parse_decimal(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 0);
    }
}

define_test(parse_identifier_throws_on_invalid_first_character)
{
    SETUP("9abc");

    std::string out;

    assert_error(it = parse_identifier(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 0);
    }
}

define_test(parse_identifier_parses_identifier)
{
    SETUP("abc");

    std::string out;

    it = parse_identifier(it, input, input_size, &out);
    assert_equal(it.i, 3);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 4);
    assert_equal(out, "abc"s);
}

define_test(parse_identifier_parses_identifier2)
{
    SETUP("def ghi");

    std::string out;

    it = parse_identifier(it, input, input_size, &out);
    assert_equal(it.i, 3);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 4);
    assert_equal(out, "def"s);
}

define_test(parse_identifier_parses_identifier3)
{
    SETUP("_hello_WORLD");

    std::string out;

    it = parse_identifier(it, input, input_size, &out);
    assert_equal(it.i, 12);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 13);
    assert_equal(out, "_hello_WORLD"s);
}

define_test(parse_bool_parses_true)
{
    SETUP("true");

    bool out;

    it = parse_bool(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, true);
}

define_test(parse_bool_parses_true2)
{
    SETUP("tRuE  abc");

    bool out;

    it = parse_bool(it, input, input_size, &out);
    assert_equal(it.i, 4);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 5);
    assert_equal(out, true);
}

define_test(parse_bool_parses_false)
{
    SETUP("false");

    bool out;

    it = parse_bool(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, false);
}

define_test(parse_bool_parses_false2)
{
    SETUP("FALse 123");

    bool out;

    it = parse_bool(it, input, input_size, &out);
    assert_equal(it.i, 5);
    assert_equal(it.line_start, 0);
    assert_equal(it.line, 1);
    assert_equal(it.line_pos, 6);
    assert_equal(out, false);
}

define_test(parse_bool_throws_on_invalid_input2)
{
    SETUP("t");

    bool out;

    assert_error(it = parse_bool(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 1);
        assert_equal(err.it.line_start, 0);
        assert_equal(err.it.line, 1);
        assert_equal(err.it.line_pos, 2);
        assert_equal(err.input, "t");
    }
}

define_test(parse_bool_throws_on_invalid_input3)
{
    SETUP("tr");

    bool out;

    assert_error(it = parse_bool(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 2);
        assert_equal(err.it.line_start, 0);
        assert_equal(err.it.line, 1);
        assert_equal(err.it.line_pos, 3);
        assert_equal(err.input, "tr");
    }
}

define_test(parse_bool_throws_on_invalid_input4)
{
    SETUP("tX");

    bool out;

    assert_error(it = parse_bool(it, input, input_size, &out), parse_error<>)
    {
        assert_equal(err.it.i, 2);
        assert_equal(err.it.line_start, 0);
        assert_equal(err.it.line, 1);
        assert_equal(err.it.line_pos, 3);
        assert_equal(err.input, "tX");
    }
}

define_default_test_main();
