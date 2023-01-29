
#include "shl/string_manip.hpp"
#include "shl/parse_object.hpp"

template<typename CharT>
std::basic_string<CharT> slice(const CharT *input, const parse_range *range)
{
    return std::basic_string<CharT>(input + range->start.pos, range_length(range));
}

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

#define get_parse_error_(CharT, p, FMT, ...) \
    parse_error<CharT>{format_error(FMT __VA_OPT__(,) __VA_ARGS__), __FILE__, __LINE__, p->it, p->input, p->input_size}

#define get_parse_error(CharT, ERR, p, FMT, ...) \
    if (ERR != nullptr) { *ERR = get_parse_error_(CharT, p, FMT, __VA_ARGS__); }

#define DEFINE_PARSED_IDENTIFIER_EQUALITY_OPERATOR(CharT)\
bool operator==(const parsed_identifier<CharT> &lhs, const parsed_identifier<CharT> &rhs)\
{\
    return lhs.value == rhs.value;\
}

DEFINE_PARSED_IDENTIFIER_EQUALITY_OPERATOR(char);
DEFINE_PARSED_IDENTIFIER_EQUALITY_OPERATOR(wchar_t);

template<typename CharT>
std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &lhs, const parsed_identifier<CharT> &rhs)
{
    return lhs << rhs.value;
}

#define DEFINE_PARSED_OBJECT_STREAM_SHIFT_OPERATOR(CharT, CharT2)\
std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &lhs, const basic_parsed_object<CharT2> &rhs)\
{\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::bool_type>(rhs.data))\
        return lhs << (std::get<typename basic_parsed_object<CharT2>::bool_type>(rhs.data) ? "true" : "false");\
    else\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::integer_type>(rhs.data))\
        return lhs << std::get<typename basic_parsed_object<CharT2>::integer_type>(rhs.data);\
    else\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::decimal_type>(rhs.data))\
        return lhs << std::get<typename basic_parsed_object<CharT2>::decimal_type>(rhs.data);\
    else\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::string_type>(rhs.data))\
        return lhs << PARSE_STRING_DELIM << std::get<typename basic_parsed_object<CharT2>::string_type>(rhs.data) << PARSE_STRING_DELIM;\
    else\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::identifier_type>(rhs.data))\
        return lhs << std::get<typename basic_parsed_object<CharT2>::identifier_type>(rhs.data).value;\
    else\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::list_type>(rhs.data))\
    {\
        const auto &list = std::get<typename basic_parsed_object<CharT2>::list_type>(rhs.data);\
        lhs << PARSE_LIST_OPENING_BRACKET;\
\
        if (!list.empty())\
        {\
            lhs << list[0];\
\
            for (size_t i = 1; i < list.size(); ++i)\
            {\
                lhs << PARSE_LIST_ITEM_DELIM << ' ';\
                lhs << list[i];\
            }\
        }\
\
        lhs << PARSE_LIST_CLOSING_BRACKET;\
\
        return lhs;\
    }\
    else\
    if (std::holds_alternative<typename basic_parsed_object<CharT2>::table_type>(rhs.data))\
    {\
        const auto &tab = std::get<typename basic_parsed_object<CharT2>::table_type>(rhs.data);\
        lhs << PARSE_TABLE_OPENING_BRACKET;\
\
        if (!tab.empty())\
        {\
            auto it = tab.begin();\
\
            lhs << it->first << ' '\
                << PARSE_TABLE_KEY_VALUE_DELIM << ' '\
                << it->second;\
\
            while (++it != tab.end())\
            {\
                lhs << PARSE_TABLE_ITEM_DELIM << ' '\
                    << it->first << ' '\
                    << PARSE_TABLE_KEY_VALUE_DELIM << ' '\
                    << it->second;\
            }\
        }\
\
        lhs << PARSE_TABLE_CLOSING_BRACKET;\
\
        return lhs;\
    }\
\
    return lhs;\
}

DEFINE_PARSED_OBJECT_STREAM_SHIFT_OPERATOR(char, char);
DEFINE_PARSED_OBJECT_STREAM_SHIFT_OPERATOR(wchar_t, wchar_t);

#define DEFINE_PARSED_OBJECT_EQUALITY_OPERATOR(CharT)\
bool operator==(const basic_parsed_object<CharT> &lhs, const basic_parsed_object<CharT> &rhs)\
{\
    return lhs.data == rhs.data;\
}

DEFINE_PARSED_OBJECT_EQUALITY_OPERATOR(char);
DEFINE_PARSED_OBJECT_EQUALITY_OPERATOR(wchar_t);

template<typename CharT>
bool _parse_number_object(parser<CharT> *p, basic_parsed_object<CharT> *obj, parse_error<CharT> *err)
{
    if (!is_ok(p))
    {
        get_parse_error(CharT, err, p, "input is nullptr or end was reached");
        return false;
    }

    bool int_success = false;
    bool dec_success = false;
    parser<CharT> int_p = *p;
    parser<CharT> dec_p = *p;
    parse_error<CharT> int_err;
    parse_error<CharT> dec_err;

    typename basic_parsed_object<CharT>::integer_type integer;
    typename basic_parsed_object<CharT>::decimal_type decimal;

    int_success = parse_integer(&int_p, &integer, &int_err);
    dec_success = parse_decimal(&dec_p, &decimal, &dec_err);

    if (int_success && dec_success)
    {
        // both parsed successfully
        // if equal length -> integer
        // if one is longer than the other, choose that one
        if (int_p.it.pos >= dec_p.it.pos)
        {
            obj->data.template emplace<decltype(integer)>(integer);
            p->it = int_p.it;
        }
        else
        {
            obj->data.template emplace<decltype(decimal)>(decimal);
            p->it = dec_p.it;
        }
    }
    else if (int_success)
    {
        obj->data.template emplace<decltype(integer)>(integer);
        p->it = int_p.it;
    }
    else if (dec_success)
    {
        obj->data.template emplace<decltype(decimal)>(decimal);
        p->it = dec_p.it;
    }
    else
    {
        // both failed to parse, eject the longest error
        if (int_err.it.pos >= dec_err.it.pos)
        {
            *err = int_err;
            return false;
        }
        else
        {
            *err = dec_err;
            return false;
        }
    }

    return true;
}

bool parse_number_object(parser<char> *p, basic_parsed_object<char> *obj, parse_error<char> *err)
{
    return _parse_number_object(p, obj, err);
}

bool parse_number_object(parser<wchar_t> *p, basic_parsed_object<wchar_t> *obj, parse_error<wchar_t> *err)
{
    return _parse_number_object(p, obj, err);
}

template<typename CharT>
bool _parse_object_list(parser<CharT> *p, typename basic_parsed_object<CharT>::list_type *out, parse_error<CharT> *err)
{
    if (!is_ok(p))
    {
        get_parse_error(CharT, err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;

    CharT c = current_char(p);

    if (c != PARSE_LIST_OPENING_BRACKET)
    {
        get_parse_error(CharT, err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in list at " IT_FMT, c, format_it(p->it), PARSE_LIST_OPENING_BRACKET, format_it(start));
        return false;
    }

    advance(&p->it);

    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "unterminated list starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = current_char(p);

    if (c == PARSE_LIST_CLOSING_BRACKET)
    {
        advance(&p->it);
        return true;
    }

    basic_parsed_object<CharT> obj;

    if (!parse_object(p, &obj, err))
    {
        p->it = start;
        return false;
    }
        
    out->emplace_back(std::move(obj));

    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "unterminated list starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = current_char(p);

    while (c == PARSE_LIST_ITEM_DELIM)
    {
        advance(&p->it);

        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            get_parse_error(CharT, err, p, "unterminated list starting at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        if (!parse_object(p, &obj, err))
        {
            p->it = start;
            return false;
        }

        out->emplace_back(std::move(obj));

        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            get_parse_error(CharT, err, p, "unterminated list starting at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = current_char(p);
    }

    if (c != PARSE_LIST_CLOSING_BRACKET)
    {
        get_parse_error(CharT, err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in list at " IT_FMT, c, format_it(p->it), PARSE_LIST_CLOSING_BRACKET, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    return true;
}

bool parse_object_list(parser<char> *p, object_list *out, parse_error<char> *err)
{
    return _parse_object_list(p, out, err);
}

bool parse_object_list(parser<wchar_t> *p, wobject_list *out, parse_error<wchar_t> *err)
{
    return _parse_object_list(p, out, err);
}

template<typename CharT>
bool _parse_object_table(parser<CharT> *p, typename basic_parsed_object<CharT>::table_type *out, parse_error<CharT> *err)
{
    if (!is_ok(p))
    {
        get_parse_error(CharT, err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;

    CharT c = current_char(p);

    if (c != PARSE_TABLE_OPENING_BRACKET)
    {
        get_parse_error(CharT, err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_OPENING_BRACKET, format_it(start));
        return false;
    }

    advance(&p->it);
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "unterminated table starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = current_char(p);

    if (c == PARSE_TABLE_CLOSING_BRACKET)
    {
        advance(&p->it);
        return true;
    }

    typename basic_parsed_object<CharT>::identifier_type ident;
    parse_range rn;
    
    if (!parse_identifier(p, &rn, err))
    {
        p->it = start;
        return false;
    }

    ident.value = slice(p->input, &rn);

    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "unexpected EOF at " IT_FMT ", expected '%c' in table at " IT_FMT, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
        p->it = start;
        return false;
    }

    c = current_char(p);

    if (c != PARSE_TABLE_KEY_VALUE_DELIM)
    {
        get_parse_error(CharT, err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    basic_parsed_object<CharT> obj;
    if (!parse_object(p, &obj, err))
    {
        p->it = start;
        return false;
    }

    auto dup = out->find(ident.value);

    if (dup != out->end())
    {
        get_parse_error(CharT, err, p, "duplicate key '%s' at " IT_FMT ", in table at " IT_FMT, ident.value.c_str(), format_it(p->it), format_it(start));
        p->it = start;
        return false;
    }

    out->insert_or_assign(ident.value, std::move(obj));
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "unterminated table starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = current_char(p);

    while (c == PARSE_TABLE_ITEM_DELIM)
    {
        advance(&p->it);
        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            get_parse_error(CharT, err, p, "unterminated table starting at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        if (!parse_identifier(p, &rn, err))
        {
            p->it = start;
            return false;
        }

        ident.value = slice(p->input, &rn);

        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            get_parse_error(CharT, err, p, "unexpected EOF at " IT_FMT ", expected '%c' in table at " IT_FMT, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
            p->it = start;
            return false;
        }

        c = current_char(p);

        if (c != PARSE_TABLE_KEY_VALUE_DELIM)
        {
            get_parse_error(CharT, err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
            p->it = start;
            return false;
        }

        advance(&p->it);

        basic_parsed_object<CharT> obj;

        if (!parse_object(p, &obj, err))
        {
            p->it = start;
            return false;
        }

        dup = out->find(ident.value);

        if (dup != out->end())
        {
            get_parse_error(CharT, err, p, "duplicate key '%s' at " IT_FMT ", in table at " IT_FMT, ident.value.c_str(), format_it(p->it), format_it(start));
            p->it = start;
            return false;
        }

        out->insert_or_assign(ident.value, std::move(obj));
        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            get_parse_error(CharT, err, p, "unterminated table starting at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = current_char(p);
    }

    if (c != PARSE_TABLE_CLOSING_BRACKET)
    {
        get_parse_error(CharT, err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_CLOSING_BRACKET, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    return true;
}

bool parse_object_table(parser<char> *p, object_table *out, parse_error<char> *err)
{
    return _parse_object_table(p, out, err);
}

bool parse_object_table(parser<wchar_t> *p, wobject_table *out, parse_error<wchar_t> *err)
{
    return _parse_object_table(p, out, err);
}

template<typename CharT>
bool _parse_object(parser<CharT> *p, basic_parsed_object<CharT> *out, parse_error<CharT> *err)
{
    if (!is_ok(p))
    {
        get_parse_error(CharT, err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "no object at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    CharT c = current_char(p);

    basic_parsed_object<CharT> ret;

    if (c == PARSE_STRING_DELIM)
    {
        parse_range rn;
        if (!parse_string(p, &rn, err, PARSE_STRING_DELIM, false))
        {
            p->it = start;
            return false;
        }

        ret.data.template emplace<typename basic_parsed_object<CharT>::string_type>
            (p->input + rn.start.pos, range_length(&rn));
    }
    else if (c == PARSE_LIST_OPENING_BRACKET)
    {
        typename basic_parsed_object<CharT>::list_type list;
        
        if (!parse_object_list(p, &list, err))
        {
            p->it = start;
            return false;
        }

        ret.data.template emplace<typename basic_parsed_object<CharT>::list_type>
            (std::move(list));
    }
    else if (c == PARSE_TABLE_OPENING_BRACKET)
    {
        typename basic_parsed_object<CharT>::table_type table;
        
        if (!parse_object_table(p, &table, err))
        {
            p->it = start;
            return false;
        }

        ret.data.template emplace<typename basic_parsed_object<CharT>::table_type>
            (std::move(table));
    }
    else if (c == '0' || c == '+' || c == '-' || c == '.')
    {
        if (!parse_number_object(p, &ret, err))
        {
            p->it = start;
            return false;
        }
    }
    else
    {
        // either identifier, boolean or number
        parser<CharT> bool_p = *p;
        parse_range bool_rn;
        parse_error<CharT> bool_err;
        bool bool_success = parse_bool(&bool_p, &bool_rn, &bool_err);
        
        parser<CharT> id_p = *p;
        parse_range id_rn;
        parse_error<CharT> id_err;
        bool id_success = parse_identifier(&id_p, &id_rn, &id_err);

        parser<CharT> num_p = *p;
        parse_error<CharT> num_err;
        basic_parsed_object<CharT> num_obj;
        bool num_success = parse_number_object(&num_p, &num_obj, &num_err);

        if (bool_success)
        {
            if (id_p.it.pos > bool_p.it.pos)
            {
                p->it = id_p.it;
                ret.data.template emplace<typename basic_parsed_object<CharT>::identifier_type>
                    (parsed_identifier<CharT>{std::basic_string<CharT>{id_p.input + id_rn.start.pos, range_length(&id_rn)}});
            }
            else
            {
                p->it = bool_p.it;
                ret.data.template emplace<typename basic_parsed_object<CharT>::bool_type>
                    (to_lower(bool_p.input[bool_rn.start.pos]) == 't');
            }
        }
        else
        {
            if (id_success && num_success)
            {
                if (id_p.it.pos > num_p.it.pos)
                {
                    p->it = id_p.it;
                    ret.data.template emplace<typename basic_parsed_object<CharT>::identifier_type>
                        (parsed_identifier<CharT>{std::basic_string<CharT>{id_p.input + id_rn.start.pos, range_length(&id_rn)}});
                }
                else
                {
                    p->it = num_p.it;
                    ret.data = std::move(num_obj.data);
                }
            }
            else if (id_success)
            {
                p->it = id_p.it;
                ret.data.template emplace<typename basic_parsed_object<CharT>::identifier_type>
                    (parsed_identifier<CharT>{std::basic_string<CharT>{id_p.input + id_rn.start.pos, range_length(&id_rn)}});
            }
            else if (num_success)
            {
                p->it = num_p.it;
                ret.data = std::move(num_obj.data);
            }
            else
            {
                // nothing parsed, eject the furthest error
                if (bool_err.it.pos >= id_err.it.pos
                 && bool_err.it.pos >= num_err.it.pos)
                {
                    *err = bool_err;
                    return false;
                }
                else if (num_err.it.pos >= id_err.it.pos)
                {
                    *err = num_err;
                    return false;
                }
                else
                {
                    *err = id_err;
                    return false;
                }
            }
        }
    }

    out->data = std::move(ret.data);

    return true;
}

bool parse_object(parser<char> *p, basic_parsed_object<char> *out, parse_error<char> *err)
{
    return _parse_object(p, out, err);
}

bool parse_object(parser<wchar_t> *p, basic_parsed_object<wchar_t> *out, parse_error<wchar_t> *err)
{
    return _parse_object(p, out, err);
}
