
/* parse_object.hpp
v1.0

depends on parse.hpp

parses strings into objects, similar to json.
an object is either a boolean (true, false), a number (integer,
floating point), a string ("..."), an identifier, a list of objects ([x, y, z])
or a table of objects ({a: obj, b: obj}) (where the keys are identifiers).

example:
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

comments can be inserted anywhere between values with C line or multiline comments.

objects can be cast to their value to obtain a reference or const reference,
the value can also be obtained via the get<T> method or directly via obj.data.
to check whether an object holds a given type, use the .has_value method.

*/

#pragma once

#include "shl/string.hpp"
#include "shl/linked_list.hpp"
#include "shl/hash_table.hpp"
#include "shl/format.hpp"
#include "shl/parse.hpp"

struct parsed_identifier
{
    const_string value;
};

bool operator==(const parsed_identifier &lhs, const parsed_identifier &rhs);

enum class parsed_object_type : u8
{
    None,
    Bool,
    Integer,
    Decimal,
    String,
    Identifier,
    List,
    Table
};

struct parsed_object
{
    typedef bool bool_type;
    typedef s64 integer_type;
    typedef double decimal_type;
    typedef const_string string_type;
    typedef parsed_identifier identifier_type;
    typedef linked_list<parsed_object> list_type;
    typedef hash_table<string_type, parsed_object> table_type;

    parsed_object_type type;

    union parsed_object_data_type
    {
        bool_type       _bool;
        integer_type    _integer;
        decimal_type    _decimal;
        string_type     _string;
        identifier_type _identifier;
        list_type       _list;
        table_type      _table;
    } data;

    parsed_object &operator[](s64 n) { return data._list[n]; }

    parsed_object &operator[](const char *i) { return this->operator[](to_const_string(i)); }
    parsed_object &operator[](string_type i) { return *search_by_hash(&data._table, hash(&i)); }
};

bool operator==(parsed_object &lhs, parsed_object &rhs);
bool operator==(parsed_object &lhs, parsed_object &rhs);

typedef typename parsed_object::list_type  object_list;
typedef typename parsed_object::table_type object_table;

void init(parsed_object  *obj);
void free(parsed_object  *obj);

bool parse_number_object(parser  *p, parsed_object  *obj, parse_error *err = nullptr);
bool parse_number_object(const_string  input, parsed_object *obj, parse_error *err = nullptr);
bool parse_number_object(const string *input, parsed_object *obj, parse_error *err = nullptr);

bool parse_object_list(parser  *p, object_list  *out, parse_error *err = nullptr);
bool parse_object_list(const_string  input, object_list *obj, parse_error *err = nullptr);
bool parse_object_list(const string *input, object_list *obj, parse_error *err = nullptr);

bool parse_object_table(parser  *p, object_table  *out, parse_error *err = nullptr);
bool parse_object_table(const_string  input, object_table *obj, parse_error *err = nullptr);
bool parse_object_table(const string *input, object_table *obj, parse_error *err = nullptr);

bool parse_object(parser  *p, parsed_object  *out, parse_error *err = nullptr);
bool parse_object(const_string  input, parsed_object *obj, parse_error *err = nullptr);
bool parse_object(const string *input, parsed_object *obj, parse_error *err = nullptr);

s64 to_string(string *s, const parsed_object *x);
s64 to_string(string *s, const parsed_object *x, s64 offset);
s64 to_string(string *s, const parsed_object *x, s64 offset, format_options<char> opt);
