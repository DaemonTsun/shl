
/* parse_object.hpp
 * v1.0
 *
 * depends on parse.hpp
 *
 * parses strings into objects, similar to json.
 * an object is either a boolean (true, false), a number (integer,
 * floating point), a string ("..."), an identifier, a list of objects ([x, y, z])
 * or a table of objects ({a: obj, b: obj}) (where the keys are identifiers).
 *
 * example:
 * {
 *     name: "John Doe",
 *     age: 50,
 *     address: {
 *          streetAddress: "21 2nd Street",
 *          city: "New York",
 *          state: "NY",
 *          postalCode: "10021-3100"
 *     },
 *     phoneNumbers: [
 *         "212 555-1234",
 *         "646 555-4567"
 *     ]
 * }
 *
 * comments can be inserted anywhere between values with C line or multiline comments.
 *
 * objects can be cast to their value to obtain a reference or const reference,
 * the value can also be obtained via the get<T> method or directly via obj.data.
 * to check whether an object holds a given type, use the .has_value method.
 */

#pragma once

#include "shl/string.hpp"
#include "shl/linked_list.hpp"
#include "shl/hash_table.hpp"
#include "shl/parse.hpp"

template<typename C = char>
struct parsed_identifier
{
    string_base<C> value;
};

bool operator==(const parsed_identifier<char> &lhs, const parsed_identifier<char> &rhs);
bool operator==(const parsed_identifier<wchar_t> &lhs, const parsed_identifier<wchar_t> &rhs);

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

template<typename C = char>
struct parsed_object_base
{
    typedef bool bool_type;
    typedef s64 integer_type;
    typedef double decimal_type;
    typedef string_base<C> string_type;
    typedef parsed_identifier<C> identifier_type;
    typedef linked_list<parsed_object_base<C>> list_type;
    typedef hash_table<string_type, parsed_object_base<C>> table_type;

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

    parsed_object_base<C> &operator[](u64 n) 
    { return data._list[n]; }

    const parsed_object_base<C> &operator[](u64 n) const
    { return data._list[n]; }

    parsed_object_base<C> &operator[](const string_type &i)
    { return data._table[i]; }

    parsed_object_base<C> &operator[](const string_type *i)
    { return data._table[i]; }

    const parsed_object_base<C> &operator[](const string_type &i) const
    { return *search(&data._table, &i); }

    const parsed_object_base<C> &operator[](const string_type *i) const
    { return *search(&data._table, i); }
};

bool operator==(parsed_object_base<char> &lhs, parsed_object_base<char> &rhs);
bool operator==(parsed_object_base<wchar_t> &lhs, parsed_object_base<wchar_t> &rhs);

typedef parsed_object_base<char> parsed_object;
typedef typename parsed_object_base<char>::list_type object_list;
typedef typename parsed_object_base<char>::table_type object_table;

typedef parsed_object_base<wchar_t> wparsed_object;
typedef typename parsed_object_base<wchar_t>::list_type wobject_list;
typedef typename parsed_object_base<wchar_t>::table_type wobject_table;

void init(parsed_object  *obj);
void init(wparsed_object *obj);

bool parse_number_object(parser<char>    *p, parsed_object  *obj, parse_error<char>    *err = nullptr);
bool parse_number_object(parser<wchar_t> *p, wparsed_object *obj, parse_error<wchar_t> *err = nullptr);

bool parse_object_list(parser<char>    *p, object_list  *out, parse_error<char>    *err = nullptr);
bool parse_object_list(parser<wchar_t> *p, wobject_list *out, parse_error<wchar_t> *err = nullptr);

bool parse_object_table(parser<char>    *p, object_table  *out, parse_error<char>    *err = nullptr);
bool parse_object_table(parser<wchar_t> *p, wobject_table *out, parse_error<wchar_t> *err = nullptr);

bool parse_object(parser<char>    *p, parsed_object  *out, parse_error<char>    *err = nullptr);
bool parse_object(parser<wchar_t> *p, wparsed_object *out, parse_error<wchar_t> *err = nullptr);

void free(parsed_object  *obj);
void free(wparsed_object *obj);

// TODO: to_string possibly
