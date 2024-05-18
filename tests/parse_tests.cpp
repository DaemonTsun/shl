
#define t1_use_stdlib 1
#include <t1/t1.hpp>

#include "shl/number_types.hpp"
#include "shl/string.hpp"
#include "shl/parse.hpp"

#define SETUP(STR) \
    parser p;\
    init(&p, STR, STR == nullptr ? 0 : string_length(static_cast<const char*>(STR)));

template<typename C>
const_string_base<C> slice(const C *input, const parse_range *range)
{
    return const_string_base<C>{input + range->start.pos, range_length(range)};
}

define_test(skip_whitespace_does_nothing_on_nullptr)
{
    SETUP(nullptr);

    skip_whitespace(&p);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);
}

define_test(skip_whitespace_skips_whitespace)
{
    SETUP("   abc");

    skip_whitespace(&p);

    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
}

define_test(skip_whitespace_skips_whitespace2)
{
    SETUP(R"=(

 abc)=");

    skip_whitespace(&p);

    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 2);
    assert_equal(p.it.line, 3);
    assert_equal(p.it.line_pos, 2);
}

define_test(skip_whitespace_doesnt_skip_nonwhitespace)
{
    SETUP("abc    def");

    skip_whitespace(&p);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);
}

define_test(parse_comment_parses_nullptr)
{
    SETUP(nullptr);

    bool success = parse_comment(&p, nullptr);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);

    assert_equal(success, false);
}

define_test(parse_comment_parses_empty_string)
{
    SETUP("");

    bool success = parse_comment(&p, nullptr);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);

    assert_equal(success, false);
}

define_test(parse_comment_parses_line_comment)
{
    SETUP("// hello");

    parse_range out;
    bool success = parse_comment(&p, &out);

    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);

    assert_equal(success, true);
    assert_equal(slice(p.input, &out), " hello"_cs);
}

define_test(parse_comment_parses_line_comment2)
{
    SETUP("// hello\n world");

    parse_range out;
    bool success = parse_comment(&p, &out);

    assert_equal(p.it.pos, 9);
    assert_equal(p.it.line_start, 9);
    assert_equal(p.it.line, 2);
    assert_equal(p.it.line_pos, 1);

    assert_equal(success, true);
    assert_equal(slice(p.input, &out), " hello\n"_cs);
}

define_test(parse_comment_parses_block_comment)
{
    SETUP("/* abc def */");

    parse_range out;
    bool success = parse_comment(&p, &out);

    assert_equal(p.it.pos, 13);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 14);

    assert_equal(success, true);

    assert_equal(out.start.pos, 2);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 3);

    assert_equal(out.end.pos, 11);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 12);

    assert_equal(slice(p.input, &out), " abc def "_cs);
}

define_test(skip_whitespace_and_comments_skips_whitespace)
{
    SETUP("   abc def");
    //        ^

    skip_whitespace_and_comments(&p);

    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
}

define_test(skip_whitespace_and_comments_skips_comment)
{
    SETUP("/*\nhello world\n*/  abc");
    //                          ^

    skip_whitespace_and_comments(&p);

    assert_equal(p.it.pos, 19);
    assert_equal(p.it.line_start, 15);
    assert_equal(p.it.line, 3);
    assert_equal(p.it.line_pos, 5);
}

define_test(skip_whitespace_and_comments_skips_comments)
{
    SETUP(" /*\nhello world\n*/  // this is a line comment\n /*yep*/abc");
    //                                                              ^

    skip_whitespace_and_comments(&p);

    assert_equal(p.it.pos, 54);
    assert_equal(p.input[p.it.pos], 'a');
    assert_equal(parser_current_char(&p), 'a');
    assert_equal(p.it.line_start, 46);
    assert_equal(p.it.line, 4);
    assert_equal(p.it.line_pos, 9);
}

define_test(parse_string_yields_error_on_invalid_input)
{
    SETUP("abc");

    parse_range out;
    parse_error err;
    
    bool success = parse_string(&p, &out, &err);

    assert_equal(success, false);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);

    assert_equal(err.it.pos, 0);
    assert_equal(err.input, "abc");
}

define_test(parse_string_yields_error_on_unterminated_string)
{
    SETUP("\"abc");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err);

    assert_equal(success, false);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);

    assert_equal(err.it.pos, 4);
    assert_equal(err.input, "\"abc");
}

define_test(parse_string_yields_error_on_unterminated_string2)
{
    SETUP("\"abc\\\"");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err);

    assert_equal(success, false);

    assert_equal(p.it.pos, 0);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 1);

    assert_equal(err.it.pos, 6);
    assert_equal(err.input, "\"abc\\\"");
}

define_test(parse_string_parses_string)
{
    SETUP("\"\"");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 2);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 3);

    assert_equal(out.start.pos, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.pos, 1);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 2);

    assert_equal(slice(p.input, &out), ""_cs);
}

define_test(parse_string_parses_string2)
{
    SETUP("\"abc\"");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);

    assert_equal(out.start.pos, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.pos, 4);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 5);

    assert_equal(slice(p.input, &out), "abc"_cs);
}

define_test(parse_string_parses_string3)
{
    SETUP("\"\nabc\n\"");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 6);
    assert_equal(p.it.line, 3);
    assert_equal(p.it.line_pos, 2);

    assert_equal(out.start.pos, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.pos, 6);
    assert_equal(out.end.line_start, 6);
    assert_equal(out.end.line, 3);
    assert_equal(out.end.line_pos, 1);

    assert_equal(slice(p.input, &out), "\nabc\n"_cs);
}

define_test(parse_string_parses_string_with_delims)
{
    SETUP("\"hello\"   ");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err, '"', true);

    assert_equal(success, true);

    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 8);

    assert_equal(out.start.pos, 0);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 1);

    assert_equal(out.end.pos, 7);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 8);

    assert_equal(slice(p.input, &out), "\"hello\""_cs);
}

define_test(parse_string_parses_string_with_delims2)
{
    SETUP("\"hello world\\\" \"  ");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err, '"', true);

    assert_equal(success, true);

    assert_equal(p.it.pos, 16);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 17);

    assert_equal(out.start.pos, 0);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 1);

    assert_equal(out.end.pos, 16);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 17);

    assert_equal(slice(p.input, &out), "\"hello world\\\" \""_cs);
}

define_test(parse_string_parses_string_delims)
{
    SETUP("x abc XYZ x");

    parse_range out;
    parse_error err;

    bool success = parse_string(&p, &out, &err, 'x');

    assert_equal(success, true);

    assert_equal(p.it.pos, 11);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 12);

    assert_equal(out.start.pos, 1);
    assert_equal(out.start.line_start, 0);
    assert_equal(out.start.line, 1);
    assert_equal(out.start.line_pos, 2);

    assert_equal(out.end.pos, 10);
    assert_equal(out.end.line_start, 0);
    assert_equal(out.end.line, 1);
    assert_equal(out.end.line_pos, 11);

    assert_equal(slice(p.input, &out), " abc XYZ "_cs);
}

define_test(parse_integer_parses_integer)
{
    SETUP("1234");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);

    assert_equal(slice(p.input, &out), "1234"_cs);
}

define_test(parse_integer_parses_integer2)
{
    SETUP("1234xyz");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);

    assert_equal(slice(p.input, &out), "1234"_cs);
}

define_test(parse_integer_parses_integer3)
{
    SETUP("-0xabc");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 6);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 7);
}

define_test(parse_int_parses_integer)
{
    SETUP("1234");

    int out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);

    assert_equal(out, 1234);
}

define_test(parse_int_parses_integer2)
{
    SETUP("-5678");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(out, -5678);
}

define_test(parse_integer_parses_unsigned_long_int)
{
    SETUP("123123123123");

    u64 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 12);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 13);
    assert_equal(out, 123123123123ull);
}

define_test(parse_integer_parses_unsigned_integer2)
{
    SETUP("-1");

    u64 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 2);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 3);
    assert_equal(out, max_value(u64));
}

define_test(parse_integer_parses_binary_integer)
{
    SETUP("0b1010");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 6);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 7);
    assert_equal(slice(p.input, &out), "0b1010"_cs);
}

define_test(parse_int_parses_binary_integer)
{
    SETUP("0b101014");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 8);
    assert_equal(out, 21);
}

define_test(parse_int_parses_octal_integer)
{
    SETUP("01234");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(out, 668);
}

define_test(parse_int_parses_octal_integer2)
{
    SETUP("0123456789");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);
    assert_equal(out, 01234567);
}

define_test(parse_int_parses_hex_integer)
{
    SETUP("0xff");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, 255);
}

define_test(parse_int_parses_hex_integer2)
{
    SETUP("0xffghi");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, 255);
}

define_test(parse_int_parses_hex_integer3)
{
    SETUP("feef");

    s32 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, 65263);
}

define_test(parse_integer_parses_hex_integer)
{
    SETUP("0xdeAdBEeF");

    s64 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 10);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 11);
    assert_equal(out, 3735928559);
}

define_test(parse_integer_parses_hex_integer2)
{
    SETUP("-0xdeAdBEeF");

    s64 out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 11);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 12);
    assert_equal(out, -3735928559);
}

define_test(parse_integer_yields_error_on_invalid_input)
{
    SETUP("xyz");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 0);
    assert_equal(err.input, "xyz");
}

define_test(parse_integer_yields_error_on_invalid_input2)
{
    SETUP("0x");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 2);
    assert_equal(err.input, "0x");
}

define_test(parse_integer_yields_error_on_invalid_input3)
{
    SETUP("0b");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 2);
    assert_equal(err.input, "0b");
}

define_test(parse_integer_yields_error_on_invalid_input4)
{
    SETUP("0xZ");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 2);
    assert_equal(err.input, "0xZ");
}

define_test(parse_integer_yields_error_on_invalid_input5)
{
    SETUP("+");

    parse_range out;
    parse_error err;

    bool success = parse_integer(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 1);
    assert_equal(err.input, "+");
}

define_test(parse_integer_parses_from_string)
{
    int out;
    bool success = parse_integer("123"_cs, &out);

    assert_equal(success, true);
    assert_equal(out, 123);
}

define_test(parse_decimal_parses_float)
{
    SETUP("1.0");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
    assert_equal(slice(p.input, &out), "1.0"_cs);
}

define_test(parse_decimal_parses_float2)
{
    SETUP("+.06");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(slice(p.input, &out), "+.06"_cs);
}

define_test(parse_decimal_parses_float3)
{
    SETUP("-0.06");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(slice(p.input, &out), "-0.06"_cs);
}

define_test(parse_decimal_parses_float4)
{
    SETUP("1.33e+10");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);
    assert_equal(slice(p.input, &out), "1.33e+10"_cs);
}

define_test(parse_decimal_parses_float5)
{
    SETUP("2.5E-12");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 8);
    assert_equal(slice(p.input, &out), "2.5E-12"_cs);
}

define_test(parse_decimal_parses_float6)
{
    SETUP("12341234");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);
    assert_equal(slice(p.input, &out), "12341234"_cs);
}

define_test(parse_decimal_parses_float7)
{
    SETUP("1234e5");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 6);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 7);
    assert_equal(slice(p.input, &out), "1234e5"_cs);
}

define_test(parse_decimal_parses_float8)
{
    SETUP("1.0");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
    assert_equal(out, 1.0f);
}

define_test(parse_decimal_parses_float9)
{
    SETUP("+.06");

    float out;

    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, .06f);
}

define_test(parse_decimal_parses_float10)
{
    SETUP("-0.06");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(out, -0.06f);
}

define_test(parse_decimal_parses_float11)
{
    SETUP("1.33e+10");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);
    assert_equal(out, 1.33e+10f);
}

define_test(parse_decimal_parses_float12)
{
    SETUP("2.5E-12");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 8);
    assert_equal(out, 2.5E-12f);
}

define_test(parse_decimal_parses_float13)
{
    SETUP("12341234");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);
    assert_equal(out, 12341234.f);
}

define_test(parse_decimal_parses_float14)
{
    SETUP("1234e5");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 6);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 7);
    assert_equal(out, 1234e5);
}

define_test(parse_decimal_parses_double)
{
    SETUP("1.0");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
    assert_equal(out, 1.0);
}

define_test(parse_decimal_parses_double2)
{
    SETUP("+.06");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, .06);
}

define_test(parse_decimal_parses_double3)
{
    SETUP("-0.06X");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(out, -0.06);
}

define_test(parse_decimal_parses_double4)
{
    SETUP("1.33e+10");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 8);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 9);
    assert_equal(out, 1.33e+10);
}

define_test(parse_decimal_parses_double5)
{
    SETUP("2.5E-12X");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 8);
    assert_equal(out, 2.5E-12);
}

define_test(parse_decimal_parses_double6)
{
    SETUP("12341234.X");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 9);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 10);
    assert_equal(out, 12341234.);
}

define_test(parse_decimal_parses_double7)
{
    SETUP("1234e5X");

    double out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 6);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 7);
    assert_equal(out, 1234e5);
}

define_test(parse_decimal_parses_decimal)
{
    SETUP("123456789.987654321e10");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 22);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 23);
}

define_test(parse_decimal_yields_error_on_invalid_input)
{
    SETUP("z");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 0);
}

define_test(parse_decimal_yields_error_on_invalid_input2)
{
    SETUP(".");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 1);
}

define_test(parse_decimal_yields_error_on_invalid_input3)
{
    SETUP("e");

    float out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 0);
}

define_test(parse_decimal_yields_error_on_invalid_input4)
{
    SETUP("10e");

    parse_range out;
    parse_error err;

    bool success = parse_decimal(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 3);
}

define_test(parse_decimal_parses_from_string)
{
    float out;
    bool success = parse_decimal("1.23"_cs, &out);

    assert_equal(success, true);
    assert_equal(out, 1.23f);
}

define_test(parse_identifier_throws_on_invalid_first_character)
{
    SETUP("9abc");

    parse_range out;
    parse_error err;

    bool success = parse_identifier(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 0);
}

define_test(parse_identifier_parses_identifier)
{
    SETUP("abc");

    parse_range out;
    parse_error err;

    bool success = parse_identifier(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
    assert_equal(slice(p.input, &out), "abc"_cs);
}

define_test(parse_identifier_parses_identifier2)
{
    SETUP("def ghi");

    parse_range out;
    parse_error err;

    bool success = parse_identifier(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 3);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 4);
    assert_equal(slice(p.input, &out), "def"_cs);
}

define_test(parse_identifier_parses_identifier3)
{
    SETUP("_hello_WORLD");

    parse_range out;
    parse_error err;

    bool success = parse_identifier(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 12);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 13);
    assert_equal(slice(p.input, &out), "_hello_WORLD"_cs);
}

define_test(parse_bool_parses_true)
{
    SETUP("true");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, true);
}

define_test(parse_bool_parses_true2)
{
    SETUP("tRuE  abc");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);
    assert_equal(out, true);
}

define_test(parse_bool_parses_false)
{
    SETUP("false");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(out, false);
}

define_test(parse_bool_parses_false2)
{
    SETUP("FALse 123");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);
    assert_equal(out, false);
}

define_test(parse_bool_yields_error_on_invalid_input)
{
    SETUP("t");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 1);
    assert_equal(err.it.line_start, 0);
    assert_equal(err.it.line, 1);
    assert_equal(err.it.line_pos, 2);
    assert_equal(err.input, "t");
}

define_test(parse_bool_throws_on_invalid_input2)
{
    SETUP("tr");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 2);
    assert_equal(err.it.line_start, 0);
    assert_equal(err.it.line, 1);
    assert_equal(err.it.line_pos, 3);
    assert_equal(err.input, "tr");
}

define_test(parse_bool_throws_on_invalid_input3)
{
    SETUP("tX");

    bool out;
    parse_error err;

    bool success = parse_bool(&p, &out, &err);

    assert_equal(success, false);
    assert_equal(err.it.pos, 2);
    assert_equal(err.it.line_start, 0);
    assert_equal(err.it.line, 1);
    assert_equal(err.it.line_pos, 3);
    assert_equal(err.input, "tX");
}

define_test(parse_bool_parses_from_string)
{
    bool out = false;
    bool success = parse_bool("true"_cs, &out);

    assert_equal(success, true);
    assert_equal(out, true);
}

define_default_test_main();
