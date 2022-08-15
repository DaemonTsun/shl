
/* parse_object.hpp
 * depends on parse.hpp
 *
 * parses strings into objects, similar to json.
 *
 * parse_object(it, input, input_size, *obj)
 *      throws on error or invalid input.
 *
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
#include <variant>
#include "parse.hpp"

#define PARSE_STRING_DELIM '"'
#define PARSE_LIST_BRACKETS "[]"
#define PARSE_LIST_ITEM_DELIM ','
#define PARSE_TABLE_BRACKETS "{}"
#define PARSE_TABLE_ITEM_DELIM ','
#define PARSE_TABLE_KEY_VALUE_DELIM ':'

#define PARSE_LIST_OPENING_BRACKET PARSE_LIST_BRACKETS[0]
#define PARSE_LIST_CLOSING_BRACKET PARSE_LIST_BRACKETS[1]
#define PARSE_TABLE_OPENING_BRACKET PARSE_TABLE_BRACKETS[0]
#define PARSE_TABLE_CLOSING_BRACKET PARSE_TABLE_BRACKETS[1]

template<typename CharT = char>
struct parsed_identifier
{
    std::basic_string<CharT> value;
};

template<typename CharT>
constexpr bool operator==(const parsed_identifier<CharT> &lhs, const parsed_identifier<CharT> &rhs)
{
    return lhs.value == rhs.value;
}

template<typename CharT, typename CharT2>
inline std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &lhs, const parsed_identifier<CharT2> &rhs)
{
    return lhs << rhs.value;
}

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

    parsed_object_data_type data;
};

template<typename CharT>
constexpr bool operator==(const basic_parsed_object<CharT> &lhs, const basic_parsed_object<CharT> &rhs)
{
    return lhs.data == rhs.data;
}

template<typename CharT, typename CharT2>
inline std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &lhs, const basic_parsed_object<CharT2> &rhs)
{
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::bool_type>(rhs.data))
        return lhs << (std::get<typename basic_parsed_object<CharT2>::bool_type>(rhs.data) ? "true" : "false");
    else
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::integer_type>(rhs.data))
        return lhs << std::get<typename basic_parsed_object<CharT2>::integer_type>(rhs.data);
    else
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::decimal_type>(rhs.data))
        return lhs << std::get<typename basic_parsed_object<CharT2>::decimal_type>(rhs.data);
    else
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::string_type>(rhs.data))
        return lhs << PARSE_STRING_DELIM << std::get<typename basic_parsed_object<CharT2>::string_type>(rhs.data) << PARSE_STRING_DELIM;
    else
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::identifier_type>(rhs.data))
        return lhs << std::get<typename basic_parsed_object<CharT2>::identifier_type>(rhs.data).value;
    else
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::list_type>(rhs.data))
    {
        const auto &list = std::get<typename basic_parsed_object<CharT2>::list_type>(rhs.data);
        lhs << PARSE_LIST_OPENING_BRACKET;

        if (!list.empty())
        {
            lhs << list[0];

            for (size_t i = 1; i < list.size(); ++i)
            {
                lhs << PARSE_LIST_ITEM_DELIM << ' ';
                lhs << list[i];
            }
        }

        lhs << PARSE_LIST_CLOSING_BRACKET;

        return lhs;
    }
    else
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::table_type>(rhs.data))
    {
        const auto &tab = std::get<typename basic_parsed_object<CharT2>::table_type>(rhs.data);
        lhs << PARSE_TABLE_OPENING_BRACKET;

        if (!tab.empty())
        {
            auto it = tab.begin();

            lhs << it->first << ' '
                << PARSE_TABLE_KEY_VALUE_DELIM << ' '
                << it->second;

            while (++it != tab.end())
            {
                lhs << PARSE_TABLE_ITEM_DELIM << ' '
                    << it->first << ' '
                    << PARSE_TABLE_KEY_VALUE_DELIM << ' '
                    << it->second;
            }
        }

        lhs << PARSE_TABLE_CLOSING_BRACKET;

        return lhs;
    }

    return lhs;
}

typedef basic_parsed_object<char> parsed_object;
typedef typename basic_parsed_object<char>::list_type object_list;
typedef typename basic_parsed_object<char>::table_type object_table;

typedef basic_parsed_object<wchar_t> wparsed_object;
typedef typename basic_parsed_object<wchar_t>::list_type wobject_list;
typedef typename basic_parsed_object<wchar_t>::table_type wobject_table;

template<typename CharT>
parse_iterator parse_object(parse_iterator it, const CharT *input, size_t input_size, basic_parsed_object<CharT> *out);

template<typename CharT>
parse_iterator parse_number_object(parse_iterator it, const CharT *input, size_t input_size, parse_range *rn, parse_error<CharT> *err, basic_parsed_object<CharT> *obj)
{
    assert(input != nullptr);
    assert(err != nullptr);
    assert(obj != nullptr);
    assert(it.i < input_size);

    err->success = false;

    auto start = it;

    auto int_it = it;
    auto dec_it = it;
    parse_error<CharT> int_err;
    parse_error<CharT> dec_err;

    typename basic_parsed_object<CharT>::integer_type integer;
    typename basic_parsed_object<CharT>::decimal_type decimal;

    try
    {
        int_it = parse_integer(it, input, input_size, &integer);
        int_err.success = true;
    }
    catch (parse_error<CharT> &err)
    {
        int_err = err;
    }

    try
    {
        dec_it = parse_decimal(it, input, input_size, &decimal);
        dec_err.success = true;
    }
    catch (parse_error<CharT> &err)
    {
        dec_err = err;
    }

    if (int_err.success && dec_err.success)
    {
        // both parsed successfully
        // if equal length -> integer
        // if one is longer than the other, choose that one
        err->success = true;

        if (int_it.i >= dec_it.i)
        {
            obj->data.template emplace<decltype(integer)>(integer);
            it = int_it;
        }
        else
        {
            obj->data.template emplace<decltype(decimal)>(decimal);
            it = dec_it;
        }
    }
    else if (int_err.success)
    {
        err->success = true;
        obj->data.template emplace<decltype(integer)>(integer);
        it = int_it;
    }
    else if (dec_err.success)
    {
        err->success = true;
        obj->data.template emplace<decltype(decimal)>(decimal);
        it = dec_it;
    }
    else
    {
        // both failed to parse, eject the longest error
        if (int_err.it.i >= dec_err.it.i)
            *err = int_err;
        else
            *err = dec_err;
    }

    if (err->success && rn != nullptr)
    {
        rn->start = start;
        rn->end = it;
    }

    return it;
}

template<typename CharT>
parse_iterator parse_object_list(parse_iterator it, const CharT *input, size_t input_size, typename basic_parsed_object<CharT>::list_type *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    parse_iterator start = it;

    auto c = input[it.i];

    if (c != PARSE_LIST_OPENING_BRACKET)
        throw parse_error(it, input, input_size, "expected '", PARSE_LIST_OPENING_BRACKET, "', got unexpected '", c, "' in list at ", it);

    advance(&it);

    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "unterminated list starting at ", start);

    c = input[it.i];

    if (c == PARSE_LIST_CLOSING_BRACKET)
    {
        advance(&it);
        return it;
    }

    basic_parsed_object<CharT> obj;
    it = parse_object(it, input, input_size, &obj);
    out->emplace_back(std::move(obj));

    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "unterminated list starting at ", start);

    c = input[it.i];

    while (c == PARSE_LIST_ITEM_DELIM)
    {
        advance(&it);
        it = skip_whitespace_and_comments(it, input, input_size);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "unterminated list starting at ", start);

        it = parse_object(it, input, input_size, &obj);
        out->emplace_back(std::move(obj));

        it = skip_whitespace_and_comments(it, input, input_size);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "unterminated list starting at ", start);

        c = input[it.i];
    }

    if (c != PARSE_LIST_CLOSING_BRACKET)
        throw parse_error(it, input, input_size, "expected '", PARSE_LIST_CLOSING_BRACKET, "', got unexpected '", c, "' in list at ", it);

    advance(&it);

    return it;
}

template<typename CharT>
parse_iterator parse_object_table(parse_iterator it, const CharT *input, size_t input_size, typename basic_parsed_object<CharT>::table_type *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    parse_iterator start = it;

    auto c = input[it.i];

    if (c != PARSE_TABLE_OPENING_BRACKET)
        throw parse_error(it, input, input_size, "expected '", PARSE_TABLE_OPENING_BRACKET, "', got unexpected '", c, "' in table at ", it);

    advance(&it);
    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "unterminated table starting at ", start);

    c = input[it.i];

    if (c == PARSE_TABLE_CLOSING_BRACKET)
    {
        advance(&it);
        return it;
    }

    typename basic_parsed_object<CharT>::identifier_type ident;
    it = parse_identifier(it, input, input_size, &ident.value);
    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "expected '", PARSE_TABLE_KEY_VALUE_DELIM, "', got EOF in table starting at ", start);

    c = input[it.i];

    if (c != PARSE_TABLE_KEY_VALUE_DELIM)
        throw parse_error(it, input, input_size, "expected '", PARSE_TABLE_KEY_VALUE_DELIM, "', got '", c, "' in table at ", it);

    advance(&it);

    basic_parsed_object<CharT> obj;
    auto nit = parse_object(it, input, input_size, &obj);

    auto dup = out->find(ident.value);

    if (dup != out->end())
        throw parse_error(it, input, input_size, "duplicate key '", ident.value, "' in table at ", it);

    it = nit;
    out->insert_or_assign(ident.value, std::move(obj));
    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "unterminated table starting at ", start);

    c = input[it.i];

    while (c == PARSE_TABLE_ITEM_DELIM)
    {
        advance(&it);
        it = skip_whitespace_and_comments(it, input, input_size);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "unterminated table starting at ", start);

        it = parse_identifier(it, input, input_size, &ident.value);
        it = skip_whitespace_and_comments(it, input, input_size);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "expected '", PARSE_TABLE_KEY_VALUE_DELIM, "', got EOF in table starting at ", start);

        c = input[it.i];

        if (c != PARSE_TABLE_KEY_VALUE_DELIM)
            throw parse_error(it, input, input_size, "expected '", PARSE_TABLE_KEY_VALUE_DELIM, "', got '", c, "' in table at ", it);

        advance(&it);

        basic_parsed_object<CharT> obj;
        nit = parse_object(it, input, input_size, &obj);
        dup = out->find(ident.value);

        if (dup != out->end())
            throw parse_error(it, input, input_size, "duplicate key '", ident.value, "' in table at ", it);

        it = nit;
        out->insert_or_assign(ident.value, std::move(obj));
        it = skip_whitespace_and_comments(it, input, input_size);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "unterminated table starting at ", start);

        c = input[it.i];
    }

    if (c != PARSE_TABLE_CLOSING_BRACKET)
        throw parse_error(it, input, input_size, "expected '", PARSE_TABLE_CLOSING_BRACKET, "', got unexpected '", c, "' in table at ", it);

    advance(&it);

    return it;
}

template<typename CharT>
parse_iterator parse_object(parse_iterator it, const CharT *input, size_t input_size, basic_parsed_object<CharT> *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    parse_iterator start = it;
    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "no object at ", start);

    auto c = input[it.i];

    basic_parsed_object<CharT> ret;
    parse_error<CharT> err;

    if (c == PARSE_STRING_DELIM)
    {
        parse_range rn;
        it = parse_string(it, input, input_size, &rn, &err, PARSE_STRING_DELIM);

        if (err)
            throw err;

        ret.data.template emplace<typename basic_parsed_object<CharT>::string_type>
            (input + rn.start.i, rn.length());
    }
    else if (c == PARSE_LIST_OPENING_BRACKET)
    {
        typename basic_parsed_object<CharT>::list_type list;
        it = parse_object_list(it, input, input_size, &list);

        err.success = true;

        ret.data.template emplace<typename basic_parsed_object<CharT>::list_type>
            (std::move(list));
    }
    else if (c == PARSE_TABLE_OPENING_BRACKET)
    {
        typename basic_parsed_object<CharT>::table_type table;
        it = parse_object_table(it, input, input_size, &table);

        err.success = true;

        ret.data.template emplace<typename basic_parsed_object<CharT>::table_type>
            (std::move(table));
    }
    else if (c == '0' || c == '+' || c == '-' || c == '.')
    {
        it = parse_number_object(it, input, input_size, nullptr, &err, &ret);
    }
    else
    {
        // either identifier, boolean or number
        parse_iterator bool_it;
        parse_range bool_rn;
        parse_error<CharT> bool_err;

        bool_it = parse_bool(it, input, input_size, &bool_rn, &bool_err);
        
        parse_iterator id_it;
        parse_range id_rn;
        parse_error<CharT> id_err;
        id_it = parse_identifier(it, input, input_size, &id_rn, &id_err);

        parse_iterator num_it;
        parse_error<CharT> num_err;
        basic_parsed_object<CharT> num_obj;
        num_it = parse_number_object(it, input, input_size, nullptr, &num_err, &num_obj);

        if (bool_err.success)
        {
            if (id_it.i > bool_it.i)
            {
                err.success = true;
                it = id_it;
                ret.data.template emplace<typename basic_parsed_object<CharT>::identifier_type>
                    (parsed_identifier<CharT>{std::basic_string<CharT>{input + id_rn.start.i, id_rn.length()}});
            }
            else
            {
                err.success = true;
                it = bool_it;
                ret.data.template emplace<typename basic_parsed_object<CharT>::bool_type>
                    (to_lower(input[bool_rn.start.i]) == 't');
            }
        }
        else
        {
            if (id_err.success && num_err.success)
            {
                if (id_it.i > num_it.i)
                {
                    err.success = true;
                    it = id_it;
                    ret.data.template emplace<typename basic_parsed_object<CharT>::identifier_type>
                        (parsed_identifier<CharT>{std::basic_string<CharT>{input + id_rn.start.i, id_rn.length()}});
                }
                else
                {
                    err.success = true;
                    it = num_it;
                    ret = num_obj;
                }
            }
            else if (id_err.success)
            {
                err.success = true;
                it = id_it;
                ret.data.template emplace<typename basic_parsed_object<CharT>::identifier_type>
                    (parsed_identifier<CharT>{std::basic_string<CharT>{input + id_rn.start.i, id_rn.length()}});
            }
            else if (num_err.success)
            {
                err.success = true;
                it = num_it;
                ret = num_obj;
            }
            else
            {
                // nothing parsed, eject the furthest error
                if (bool_err.it.i >= id_err.it.i
                 && bool_err.it.i >= num_err.it.i)
                    err = bool_err;
                else if (num_err.it.i >= id_err.it.i)
                    err = num_err;
                else
                    err = id_err;
            }
        }
    }

    if (err)
        throw err;

    if (out != nullptr)
        *out = ret;

    return it;
}
