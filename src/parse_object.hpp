
#pragma once

#include <map>
#include "parse.hpp"

enum class parsed_object_type
{
    Bool,
    Integer,
    Decimal,
    String,
    
    List, // TODO: implement
    Table // TODO: implement
};

template<typename CharT>
struct parsed_object;

template<typename CharT = char>
struct parsed_object
{
    using string_type = std::basic_string<CharT>;
    using list_type = std::vector<parsed_object>;
    using table_type = std::map<std::basic_string<CharT>, parsed_object>;

    parsed_object_type type;

    union object_data
    {
        bool boolean;
        s64 integer;
        double decimal;
        string_type string;

        list_type list;
        table_type table;

        object_data() {}
        ~object_data() {}
    } data;
};

template<typename CharT>
parse_iterator parse_object(parse_iterator it, const CharT *input, size_t input_size, parsed_object<CharT> *out)
{
    if (input == nullptr
     || it.i >= input_size)
        throw parse_error(it, input, input_size, "not an object at ", it);

    parse_iterator begin = it;
    it = skip_whitespace(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "no object at ", begin);

    auto c = input[it.i];
    auto lowerc = to_lower(c);

    if (lowerc == 't' || lowerc == 'f')
    {
        it = parse_bool(it, input, input_size, &out->data.boolean);
        out->type = parsed_object_type::Bool;
        return it;
    }

    if (c == '"')
    {
        new (&out->data.string) typename parsed_object<CharT>::string_type;
        it = parse_string(it, input, input_size, &out->data.string);
        
        out->type = parsed_object_type::String;

        return it;
    }

    if (c == '-' || c == '+')
    {
        advance(&it);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "unexpected EOF at ", it, " in object at ", begin);

        c = input[it.i];

        if (c != '.' && !is_hex_digit(c))
            throw parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in object at ", begin);
    }

    if (c == '.')
    {
        advance(&it);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "unexpected EOF at ", it, " in object at ", begin);

        c = input[it.i];

        if (!is_hex_digit(c))
            throw parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in object at ", begin);
    }

    if (is_hex_digit(c))
    {
        auto int_it = it;
        auto dec_it = it;
        parse_error<CharT> int_err("");
        parse_error<CharT> dec_err("");

        s64 integer;
        double decimal;

        try
        {
            int_it = parse_integer(it, input, input_size, &integer);
        }
        catch (parse_error<CharT> &err)
        {
            int_err = err;
        }

        try
        {
            dec_it = parse_decimal(it, input, input_size, &out->data.decimal);
        }
        catch (parse_error<CharT> &err)
        {
            dec_err = err;
        }

        if (int_err.input == nullptr && dec_err.input == nullptr)
        {
            // both parsed successfully
            // if equal length -> integer
            // if one is longer than the other, choose that one

            if (int_it.i >= dec_it.i)
            {
                out->data.integer = integer;
                out->type = parsed_object_type::Integer;
                it = int_it;
            }
            else
            {
                out->data.decimal = decimal;
                out->type = parsed_object_type::Decimal;
                it = dec_it;
            }
        }
        else if (int_err.input == nullptr)
        {
            out->data.integer = integer;
            out->type = parsed_object_type::Integer;
            it = int_it;
        }
        else if (dec_err.input == nullptr)
        {
            out->data.decimal = decimal;
            out->type = parsed_object_type::Decimal;
            it = dec_it;
        }
        else
        {
            // both failed to parse, eject the longest error
            if (int_err.it.i >= dec_err.it.i)
                throw int_err;
            else
                throw dec_err;
        }

        return it;
    }

    // TODO:
    // if [: list
    // if {: table
    // error otherwise
          
    return it;
}
