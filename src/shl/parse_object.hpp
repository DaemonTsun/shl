
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

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "shl/parse.hpp"

template<typename CharT = char>
struct parsed_identifier
{
    std::basic_string<CharT> value;
};

bool operator==(const parsed_identifier<char> &lhs, const parsed_identifier<char> &rhs);
bool operator==(const parsed_identifier<wchar_t> &lhs, const parsed_identifier<wchar_t> &rhs);

std::basic_ostream<char> &operator<<(std::basic_ostream<char> &lhs, const parsed_identifier<char> &rhs);
std::basic_ostream<char> &operator<<(std::basic_ostream<char> &lhs, const parsed_identifier<wchar_t> &rhs);
std::basic_ostream<wchar_t> &operator<<(std::basic_ostream<wchar_t> &lhs, const parsed_identifier<char> &rhs);
std::basic_ostream<wchar_t> &operator<<(std::basic_ostream<wchar_t> &lhs, const parsed_identifier<wchar_t> &rhs);

template<typename CharT = char>
struct basic_parsed_object
{
    typedef bool bool_type;
    typedef s64 integer_type;
    typedef double decimal_type;
    typedef std::basic_string<CharT> string_type;
    typedef parsed_identifier<CharT> identifier_type;
    typedef std::vector<basic_parsed_object<CharT>> list_type;
    typedef std::map<string_type, basic_parsed_object<CharT>> table_type;

    using parsed_object_data_type =
        std::variant<bool_type,
                     integer_type,
                     decimal_type,
                     string_type,
                     identifier_type,
                     list_type,
                     table_type
                    >;

    explicit operator bool_type&()                   { return get<bool_type>(); }
    explicit operator bool_type() const              { return get<bool_type>(); }
    explicit operator integer_type&()                { return get<integer_type>(); }
    explicit operator integer_type() const           { return get<integer_type>(); }
    explicit operator decimal_type&()                { return get<decimal_type>(); }
    explicit operator decimal_type() const           { return get<decimal_type>(); }
    explicit operator string_type&()                 { return get<string_type>(); }
    explicit operator const string_type&() const     { return get<string_type>(); }
    explicit operator identifier_type&()             { return get<identifier_type>(); }
    explicit operator const identifier_type&() const { return get<identifier_type>(); }
    explicit operator list_type&()                   { return get<list_type>(); }
    explicit operator const list_type&() const       { return get<list_type>(); }
    explicit operator table_type&()                  { return get<table_type>(); }
    explicit operator const table_type&() const      { return get<table_type>(); }

    basic_parsed_object<CharT> &operator[](size_t n) 
    { return get<list_type>()[n]; }

    const basic_parsed_object<CharT> &operator[](size_t n) const
    { return get<list_type>()[n]; }

    basic_parsed_object<CharT> &operator[](const string_type &i)
    { return get<table_type>()[i]; }

    const basic_parsed_object<CharT> &operator[](const string_type &i) const
    { return get<table_type>().at(i); }

    template<typename T>
    T &get() { return std::get<T>(data); }

    template<typename T>
    const T &get() const { return std::get<T>(data); }

    template<typename T>
    bool has_value() const { return std::holds_alternative<T>(data); }

    bool is_bool()       const { return has_value<bool_type>(); }
    bool is_integer()    const { return has_value<integer_type>(); }
    bool is_decimal()    const { return has_value<decimal_type>(); }
    bool is_string()     const { return has_value<string_type>(); }
    bool is_identifier() const { return has_value<identifier_type>(); }
    bool is_list()       const { return has_value<list_type>(); }
    bool is_table()      const { return has_value<table_type>(); }

    parsed_object_data_type data;
};

bool operator==(const basic_parsed_object<char> &lhs, const basic_parsed_object<char> &rhs);
bool operator==(const basic_parsed_object<wchar_t> &lhs, const basic_parsed_object<wchar_t> &rhs);

std::basic_ostream<char> &operator<<(std::basic_ostream<char> &lhs, const basic_parsed_object<char> &rhs);
std::basic_ostream<char> &operator<<(std::basic_ostream<char> &lhs, const basic_parsed_object<wchar_t> &rhs);
std::basic_ostream<wchar_t> &operator<<(std::basic_ostream<wchar_t> &lhs, const basic_parsed_object<char> &rhs);
std::basic_ostream<wchar_t> &operator<<(std::basic_ostream<wchar_t> &lhs, const basic_parsed_object<wchar_t> &rhs);

typedef basic_parsed_object<char> parsed_object;
typedef typename basic_parsed_object<char>::list_type object_list;
typedef typename basic_parsed_object<char>::table_type object_table;

typedef basic_parsed_object<wchar_t> wparsed_object;
typedef typename basic_parsed_object<wchar_t>::list_type wobject_list;
typedef typename basic_parsed_object<wchar_t>::table_type wobject_table;

bool parse_number_object(parser<char> *p, basic_parsed_object<char> *obj, parse_error<char> *err = nullptr);
bool parse_number_object(parser<wchar_t> *p, basic_parsed_object<wchar_t> *obj, parse_error<wchar_t> *err = nullptr);

bool parse_object_list(parser<char> *p, object_list *out, parse_error<char> *err = nullptr);
bool parse_object_list(parser<wchar_t> *p, wobject_list *out, parse_error<wchar_t> *err = nullptr);

bool parse_object_table(parser<char> *p, object_table *out, parse_error<char> *err = nullptr);
bool parse_object_table(parser<wchar_t> *p, wobject_table *out, parse_error<wchar_t> *err = nullptr);

bool parse_object(parser<char> *p, basic_parsed_object<char> *out, parse_error<char> *err = nullptr);
bool parse_object(parser<wchar_t> *p, basic_parsed_object<char> *out, parse_error<wchar_t> *err = nullptr);
