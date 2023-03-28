
#include <string.h>

#include <t1/t1.hpp>
#include "shl/string.hpp"
#include "shl/string_manip.hpp"
#include "shl/parse_object.hpp"

using namespace std::literals;

#define SETUP(STR) \
    parser<char> p;\
    init(&p, STR, STR == nullptr ? 0 : string_length(static_cast<const char*>(STR)));

#define WSETUP(STR) \
    parser<wchar_t> p;\
    init(&p, STR, STR == nullptr ? 0 : string_length(static_cast<const wchar_t*>(STR)));

define_test(parse_object_parses_bool)
{
    SETUP("true");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 4);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 5);

    assert_equal(obj.type, parsed_object_type::Bool);
    assert_equal(obj.data._bool, true);

    free(&obj);
}

define_test(parse_object_parses_identifier)
{
    SETUP("truee");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);

    assert_equal(obj.type, parsed_object_type::Identifier);
    assert_equal(obj.data._identifier.value, "truee"_cs);

    free(&obj);
}

define_test(parse_object_parses_string)
{
    SETUP("   \"hello world\"");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 16);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 17);

    assert_equal(obj.type, parsed_object_type::String);
    assert_equal(obj.data._string, "hello world"_cs);

    free(&obj);
}

/*
define_test(parse_object_parses_number_over_identifier)
{
*/
//    SETUP(" /* comment */  deadbeef");
/*

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 24);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 25);

    assert_equal(obj.has_value<s64>(), true);
    assert_equal(obj.get<s64>(), 0xdeadbeef);

    s64 x = (s64)obj;
    assert_equal(x, 0xdeadbeef);
}

define_test(parse_object_parses_object_list)
{
    SETUP("[1,2,3]");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 7);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 8);

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
*/
    // SETUP("  [  1, 2,3 /*123*/ ] a");
/*

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 21);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 22);

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
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 11);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 12);

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
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 21);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 22);

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
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 2);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 3);

    assert_equal(obj.has_value<object_list>(), true);

    auto &list = obj.get<object_list>();

    assert_equal(list.size(), 0);
}

define_test(parse_object_throws_on_unterminated_list)
{
    SETUP("[1,2");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, false);

    assert_equal(err.it.pos, 4);
    assert_equal(err.it.line_start, 0);
    assert_equal(err.it.line, 1);
    assert_equal(err.it.line_pos, 5);
}

define_test(parse_object_throws_on_unterminated_list2)
{
    SETUP("[1,");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, false);

    assert_equal(err.it.pos, 3);
    assert_equal(err.it.line_start, 0);
    assert_equal(err.it.line, 1);
    assert_equal(err.it.line_pos, 4);
}

define_test(parse_object_throws_on_invalid_input)
{
    SETUP(" ]");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, false);

    assert_equal(err.it.pos, 1);
    assert_equal(err.it.line_start, 0);
    assert_equal(err.it.line, 1);
    assert_equal(err.it.line_pos, 2);
}

define_test(parse_object_parses_object_table)
{
    SETUP("{}");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 2);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 3);

    assert_equal(obj.has_value<object_table>(), true);

    auto &tab = obj.get<object_table>();

    assert_equal(tab.size(), 0);
}

define_test(parse_object_parses_object_table2)
{
    SETUP("{a:1}");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 5);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 6);

    assert_equal(obj.has_value<object_table>(), true);

    auto &tab = obj.get<object_table>();

    assert_equal(tab.size(), 1);

    auto a = obj["a"];

    assert_equal(a.has_value<s64>(), true);
    assert_equal((s64)a, 1);
}

define_test(parse_object_parses_object_table3)
{
    SETUP("{a:1, b : \"hello\"}");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);

    assert_equal(p.it.pos, 18);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 19);

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

define_test(parsed_object_equality_test)
{
    SETUP("{a:1, b : \"hello\"}");

    parsed_object obj1;
    parsed_object obj2;

    parse_object(&p, &obj1, nullptr);
    init(&p.it);
    parse_object(&p, &obj2, nullptr);

    assert_equal(obj1, obj2);
}

define_test(parsed_object_equality_test2)
{
    SETUP("{a:1, b : \"hello\"}");

    parsed_object obj1;
    parsed_object obj2;

    parse_object(&p, &obj1, nullptr);
    init(&p.it);
    parse_object(&p, &obj2, nullptr);

    auto objstr = to_string(obj1);
    p.input = objstr.c_str();
    p.input_size = objstr.size();
    init(&p.it);
    parse_object(&p, &obj2, nullptr);

    assert_equal(obj1, obj2);
}
*/

define_default_test_main();
