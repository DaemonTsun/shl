
#include <string.h>

#include <t1/t1.hpp>
#include "shl/string.hpp"
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

define_test(parse_object_parses_number_over_identifier)
{
    SETUP(" /* comment */  deadbeef");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 24);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 25);

    assert_equal(obj.type, parsed_object_type::Integer);
    assert_equal(obj.data._integer, 0xdeadbeef);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::List);

    object_list &list = obj.data._list;

    assert_equal(list.size, 3);

    parsed_object &x1 = list[0];
    parsed_object &x2 = list[1];
    parsed_object &x3 = list[2];

    assert_equal(x1.type, parsed_object_type::Integer);
    assert_equal(x2.type, parsed_object_type::Integer);
    assert_equal(x3.type, parsed_object_type::Integer);
    assert_equal(x1.data._integer, 1);
    assert_equal(x2.data._integer, 2);
    assert_equal(x3.data._integer, 3);

    free(&obj);
}

define_test(parse_object_parses_object_list2)
{
    SETUP("  [  4, 5,6 /*123*/ ] a");

    parsed_object obj;
    parse_error<char> err;
    bool success = parse_object(&p, &obj, &err);

    assert_equal(success, true);
    assert_equal(p.it.pos, 21);
    assert_equal(p.it.line_start, 0);
    assert_equal(p.it.line, 1);
    assert_equal(p.it.line_pos, 22);

    assert_equal(obj.type, parsed_object_type::List);
    object_list &list = obj.data._list;

    assert_equal(list.size, 3);

    parsed_object &x1 = list[0];
    parsed_object &x2 = list[1];
    parsed_object &x3 = list[2];

    assert_equal(x1.type, parsed_object_type::Integer);
    assert_equal(x2.type, parsed_object_type::Integer);
    assert_equal(x3.type, parsed_object_type::Integer);
    assert_equal(x1.data._integer, 4);
    assert_equal(x2.data._integer, 5);
    assert_equal(x3.data._integer, 6);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::List);
    object_list &list = obj.data._list;

    assert_equal(list.size, 3);

    parsed_object &x1 = list[0];
    parsed_object &x2 = list[1];
    parsed_object &x3 = list[2];

    assert_equal(x1.type, parsed_object_type::Integer);
    assert_equal(x2.type, parsed_object_type::String);
    assert_equal(x3.type, parsed_object_type::Integer);
    assert_equal(x1.data._integer, 1);
    assert_equal(x2.data._string, "2"_cs);
    assert_equal(x3.data._integer, 3);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::List);
    object_list &list = obj.data._list;

    assert_equal(list.size, 3);

    parsed_object &x1 = list[0];
    parsed_object &x2 = list[1];
    parsed_object &x3 = list[2];

    assert_equal(x1.type, parsed_object_type::Integer);
    assert_equal(x2.type, parsed_object_type::String);
    assert_equal(x3.type, parsed_object_type::List);

    assert_equal(x1.data._integer, 1);
    assert_equal(x2.data._string, "2"_cs);

    object_list &x3l = x3.data._list;

    assert_equal(x3l.size, 2);

    parsed_object &y1 = x3l[0];
    parsed_object &y2 = x3l[1];

    assert_equal(y1.type, parsed_object_type::List);
    assert_equal(y2.type, parsed_object_type::Bool);

    assert_equal(y2.data._bool, true);

    object_list &y1l = y1.data._list;

    assert_equal(y1l.size, 1);

    parsed_object &z = y1l[0];

    assert_equal(z.type, parsed_object_type::Integer);
    assert_equal(z.data._integer, 3);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::List);
    object_list &list = obj.data._list;

    assert_equal(list.size, 0);

    free(&obj);
}

define_test(parse_object_yields_error_on_unterminated_list)
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

    assert_equal(obj.type, parsed_object_type::None);

    free(&obj);
}

define_test(parse_object_yields_error_on_unterminated_list2)
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

    assert_equal(obj.type, parsed_object_type::None);

    free(&obj);
}

define_test(parse_object_yields_error_on_invalid_input)
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

    assert_equal(obj.type, parsed_object_type::None);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::Table);
    object_table &tab = obj.data._table;

    assert_equal(tab.size, 0);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::Table);
    object_table &tab = obj.data._table;

    assert_equal(tab.size, 1);

    parsed_object &a = obj["a"];

    assert_equal(a.type, parsed_object_type::Integer);
    assert_equal(a.data._integer, 1);

    free(&obj);
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

    assert_equal(obj.type, parsed_object_type::Table);
    object_table &tab = obj.data._table;

    assert_equal(tab.size, 2);

    parsed_object &a = obj["a"];
    parsed_object &b = obj["b"];

    assert_equal(a.type, parsed_object_type::Integer);
    assert_equal(a.data._integer, 1);

    assert_equal(b.type, parsed_object_type::String);
    assert_equal(b.data._string, "hello"_cs);

    free(&obj);
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

    free(&obj1);
    free(&obj2);
}

define_test(parsed_object_equality_test2)
{
    SETUP(R"=(
{
    name: "John Doe",
    age: 50,
    address: {
         streetAddress: "21 2nd Street",
         city: "New York",
         state: "NY",
         postalCode: "10021-3100"
    },
    phoneNumbers: [
        "212 555-1234",
        "646 555-4567"
    ]
}
)=");

    parsed_object obj1;
    parsed_object obj2;

    parse_object(&p, &obj1, nullptr);
    init(&p.it);
    parse_object(&p, &obj2, nullptr);

    string objstr;
    init(&objstr);
    to_string(&objstr, &obj1);
    p.input = objstr.data.data;
    p.input_size = string_length(&objstr);
    init(&p.it);

    // don't forget to free before re-using same variable
    free(&obj2);
    parse_object(&p, &obj2, nullptr);

    assert_equal(obj1, obj2);

    free(&obj1);
    free(&obj2);

    printf("%s\n", objstr.data.data);
    free(&objstr);
}

define_default_test_main();
