
#pragma once

#include <map>
#include <variant>
#include "parse.hpp"

template<typename CharT = char>
struct parsed_identifier
{
    std::basic_string<CharT> value;
};

template<typename CharT = char>
struct parsed_object
{
    typedef std::basic_string<CharT> string_type;
    typedef parsed_identifier<CharT> identifier_type;
    typedef std::vector<parsed_object<CharT>> list_type;
    typedef std::map<string_type, parsed_object<CharT>> table_type;

    using parsed_object_type =
        std::variant<bool,
                     s64,
                     double,
                     string_type,
                     identifier_type,
                     list_type, // TODO
                     table_type // TODO
                    >;

    explicit operator bool&()            { return get<bool>(); }
    explicit operator s64&()             { return get<s64>(); }
    explicit operator double&()          { return get<double>(); }
    explicit operator string_type&()     { return get<string_type>(); }
    explicit operator identifier_type&() { return get<identifier_type>(); }
    explicit operator list_type&()       { return get<list_type>(); }
    explicit operator table_type&()      { return get<table_type>(); }

    template<typename T>
    T &get() { return std::get<T>(data); }

    template<typename T>
    bool has_value() const { return std::holds_alternative<T>(data); }

    parsed_object_type data;
};

template<typename CharT>
struct type_parse
{
    parse_iterator it;
    bool parsed = false;
    parse_error<CharT> error;

    parsed_object<CharT> obj;
};

template<typename CharT>
parse_iterator try_parse_bool_object(parse_iterator it, const CharT *input, size_t input_size, type_parse<CharT> *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    try
    {
        bool data;
        it = parse_bool(it, input, input_size, &data);
        out->obj.data.template emplace<decltype(data)>(data);
    }
    catch (parse_error<CharT> &e)
    {
        out->error = e;
        out->parsed = false;
        return it;
    }

    out->parsed = true;
    return it;
}

template<typename CharT>
parse_iterator try_parse_string_object(parse_iterator it, const CharT *input, size_t input_size, type_parse<CharT> *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    try
    {
        std::basic_string<CharT> data;
        it = parse_string(it, input, input_size, &data);
        out->obj.data.template emplace<decltype(data)>(data);
    }
    catch (parse_error<CharT> &e)
    {
        out->error = e;
        out->parsed = false;
        return it;
    }

    out->parsed = true;
    return it;
}

template<typename CharT>
parse_iterator try_parse_number_object(parse_iterator it, const CharT *input, size_t input_size, type_parse<CharT> *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    auto start = it;

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
        dec_it = parse_decimal(it, input, input_size, &decimal);
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
        out->parsed = true;

        if (int_it.i >= dec_it.i)
        {
            out->obj.data.template emplace<decltype(integer)>(integer);
            it = int_it;
        }
        else
        {
            out->obj.data.template emplace<decltype(decimal)>(decimal);
            it = dec_it;
        }
    }
    else if (int_err.input == nullptr)
    {
        out->parsed = true;
        out->obj.data.template emplace<decltype(integer)>(integer);
        it = int_it;
    }
    else if (dec_err.input == nullptr)
    {
        out->parsed = true;
        out->obj.data.template emplace<decltype(decimal)>(decimal);
        it = dec_it;
    }
    else
    {
        // both failed to parse, eject the longest error
        if (int_err.it.i >= dec_err.it.i)
        {
            out->error = int_err;
            out->parsed = false;
        }
        else
        {
            out->error = dec_err;
            out->parsed = false;
        }
    }

    return it;
}

template<typename CharT>
parse_iterator try_parse_identifier_object(parse_iterator it, const CharT *input, size_t input_size, type_parse<CharT> *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    try
    {
        parsed_identifier<CharT> data;
        it = parse_identifier(it, input, input_size, &data.value);
        out->obj.data.template emplace<decltype(data)>(data);
    }
    catch (parse_error<CharT> &e)
    {
        out->error = e;
        out->parsed = false;
        return it;
    }

    out->parsed = true;
    return it;
}

template<typename CharT>
parse_iterator parse_object(parse_iterator it, const CharT *input, size_t input_size, parsed_object<CharT> *out)
{
    assert(input != nullptr);
    assert(it.i < input_size);

    parse_iterator begin = it;
    it = skip_whitespace_and_comments(it, input, input_size);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "no object at ", begin);

    auto c = input[it.i];

    type_parse<CharT> tp;

    if (c == '"')
    {
        tp.it = try_parse_string_object(it, input, input_size, &tp);
    }
    else if (c == '[')
    {
        // TODO: implement
    }
    else if (c == '{')
    {
        // TODO: implement
    }
    else if (c == '0' || c == '+' || c == '-' || c == '.')
    {
        tp.it = try_parse_number_object(it, input, input_size, &tp);
    }
    else
    {
        // either identifier, boolean or number
        type_parse<CharT> numobj;
        numobj.it = try_parse_number_object(it, input, input_size, &numobj);
        
        type_parse<CharT> idobj;
        idobj.it = try_parse_identifier_object(it, input, input_size, &idobj);
        
        type_parse<CharT> boolobj;
        boolobj.it = try_parse_bool_object(it, input, input_size, &boolobj);

        if (boolobj.parsed)
        {
            if (idobj.it.i > boolobj.it.i)
                tp = idobj;
            else
                tp = boolobj;
        }
        else
        {
            if (idobj.parsed && numobj.parsed)
            {
                if (idobj.it.i > numobj.it.i)
                    tp = idobj;
                else
                    tp = numobj;
            }
            else if (idobj.parsed)
                tp = idobj;
            else if (numobj.parsed)
                tp = numobj;
            else
            {
                // nothing parsed, check errors
                if (boolobj.error.it.i >= idobj.error.it.i
                 && boolobj.error.it.i >= numobj.error.it.i)
                    tp = boolobj;
                else if (numobj.error.it.i >= idobj.error.it.i)
                    tp = numobj;
                else
                    tp = idobj;
            }
        }
    }

    if (!tp.parsed)
        throw tp.error;

    if (out != nullptr)
        *out = tp.obj;

    return tp.it;
}
