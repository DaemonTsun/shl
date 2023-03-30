
#include "shl/string.hpp"
#include "shl/parse_object.hpp"

template<typename CharT>
void init_slice(string_base<CharT> *out, const CharT *input, const parse_range *range)
{
    u64 len = range_length(range);
    init(out, len);
    copy_string(input + range->start.pos, out, len);
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

template<typename C>
bool _equals(parsed_object_base<C> &lhs, parsed_object_base<C> &rhs)
{
    if (lhs.type != rhs.type)
        return false;

    switch (lhs.type)
    {
    case parsed_object_type::None:
        return true;
    case parsed_object_type::Bool:
        return lhs.data._bool == rhs.data._bool;
    case parsed_object_type::Integer:
        return lhs.data._integer == rhs.data._integer;
    case parsed_object_type::Decimal:
        return lhs.data._decimal == rhs.data._decimal;
    case parsed_object_type::String:
        return lhs.data._string == rhs.data._string;
    case parsed_object_type::Identifier:
        return lhs.data._identifier.value == rhs.data._identifier.value;
    case parsed_object_type::List:
        return lhs.data._list == rhs.data._list;
    case parsed_object_type::Table:
        return lhs.data._table == rhs.data._table;
    }

    return false;
}

bool operator==(parsed_object_base<char> &lhs, parsed_object_base<char> &rhs)
{
    return _equals(lhs, rhs);
}

bool operator==(parsed_object_base<wchar_t> &lhs, parsed_object_base<wchar_t> &rhs)
{
    return _equals(lhs, rhs);
}

template<typename CharT>
bool _parse_number_object(parser<CharT> *p, parsed_object_base<CharT> *obj, parse_error<CharT> *err)
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

    typename parsed_object_base<CharT>::integer_type integer;
    typename parsed_object_base<CharT>::decimal_type decimal;

    int_success = parse_integer(&int_p, &integer, &int_err);
    dec_success = parse_decimal(&dec_p, &decimal, &dec_err);

    if (int_success && dec_success)
    {
        // both parsed successfully
        // if equal length -> integer
        // if one is longer than the other, choose that one
        if (int_p.it.pos >= dec_p.it.pos)
        {
            obj->type = parsed_object_type::Integer;
            obj->data._integer = integer;
            p->it = int_p.it;
        }
        else
        {
            obj->type = parsed_object_type::Decimal;
            obj->data._decimal = decimal;
            p->it = dec_p.it;
        }
    }
    else if (int_success)
    {
        obj->type = parsed_object_type::Integer;
        obj->data._integer = integer;
        p->it = int_p.it;
    }
    else if (dec_success)
    {
        obj->type = parsed_object_type::Decimal;
        obj->data._decimal = decimal;
        p->it = dec_p.it;
    }
    else
    {
        // both failed to parse, eject the longest error
        if (int_err.it.pos >= dec_err.it.pos)
        {
            *err = int_err;
            obj->type = parsed_object_type::None;
            return false;
        }
        else
        {
            *err = dec_err;
            obj->type = parsed_object_type::None;
            return false;
        }
    }

    return true;
}

bool parse_number_object(parser<char> *p, parsed_object_base<char> *obj, parse_error<char> *err)
{
    return _parse_number_object(p, obj, err);
}

bool parse_number_object(parser<wchar_t> *p, parsed_object_base<wchar_t> *obj, parse_error<wchar_t> *err)
{
    return _parse_number_object(p, obj, err);
}

template<typename CharT>
bool _parse_object_list(parser<CharT> *p, typename parsed_object_base<CharT>::list_type *out, parse_error<CharT> *err)
{
    init(out);

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

    parsed_object_base<CharT> obj;

    if (!parse_object(p, &obj, err))
    {
        p->it = start;
        return false;
    }
        
    add_elements(out, 1)->value = obj;

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

        add_elements(out, 1)->value = obj;

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
bool _parse_object_table(parser<CharT> *p, typename parsed_object_base<CharT>::table_type *out, parse_error<CharT> *err)
{
    init(out);

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

    typename parsed_object_base<CharT>::identifier_type ident;
    parse_range rn;
    
    if (!parse_identifier(p, &rn, err))
    {
        p->it = start;
        return false;
    }

    init_slice(&ident.value, p->input, &rn);

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

    parsed_object_base<CharT> obj;

    if (!parse_object(p, &obj, err))
    {
        p->it = start;
        return false;
    }

    if (contains(out, &ident.value))
    {
        get_parse_error(CharT, err, p, "duplicate key '%s' at " IT_FMT ", in table at " IT_FMT, ident.value.data.data, format_it(p->it), format_it(start));
        p->it = start;
        return false;
    }

    *add_element_by_key(out, &ident.value) = obj;
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

        // we init again because table stores string anyway
        init_slice(&ident.value, p->input, &rn);

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

        if (!parse_object(p, &obj, err))
        {
            p->it = start;
            return false;
        }

        if (contains(out, &ident.value))
        {
            get_parse_error(CharT, err, p, "duplicate key '%s' at " IT_FMT ", in table at " IT_FMT, ident.value.data.data, format_it(p->it), format_it(start));
            p->it = start;
            return false;
        }

        *add_element_by_key(out, &ident.value) = obj;
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
bool _parse_object(parser<CharT> *p, parsed_object_base<CharT> *out, parse_error<CharT> *err)
{
    if (!is_ok(p))
    {
        get_parse_error(CharT, err, p, "input is nullptr or end was reached");
        out->type = parsed_object_type::None;
        return false;
    }

    parse_iterator start = p->it;
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        get_parse_error(CharT, err, p, "no object at " IT_FMT, format_it(start));
        out->type = parsed_object_type::None;
        p->it = start;
        return false;
    }

    CharT c = current_char(p);

    parsed_object_base<CharT> ret;

    if (c == PARSE_STRING_DELIM)
    {
        parse_range rn;
        if (!parse_string(p, &rn, err, PARSE_STRING_DELIM, false))
        {
            out->type = parsed_object_type::None;
            p->it = start;
            return false;
        }

        ret.type = parsed_object_type::String;
        init_slice(&ret.data._string, p->input, &rn);
    }
    else if (c == PARSE_LIST_OPENING_BRACKET)
    {
        if (!parse_object_list(p, &ret.data._list, err))
        {
            free<true>(&ret.data._list);
            out->type = parsed_object_type::None;
            p->it = start;
            return false;
        }

        ret.type = parsed_object_type::List;
    }
    else if (c == PARSE_TABLE_OPENING_BRACKET)
    {
        if (!parse_object_table(p, &ret.data._table, err))
        {
            free<true, true>(&ret.data._table);
            out->type = parsed_object_type::None;
            p->it = start;
            return false;
        }

        ret.type = parsed_object_type::Table;
    }
    else if (c == '0' || c == '+' || c == '-' || c == '.')
    {
        if (!parse_number_object(p, &ret, err))
        {
            out->type = parsed_object_type::None;
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
        parsed_object_base<CharT> num_obj;
        bool num_success = parse_number_object(&num_p, &num_obj, &num_err);

        if (bool_success)
        {
            if (id_p.it.pos > bool_p.it.pos)
            {
                p->it = id_p.it;
                ret.type = parsed_object_type::Identifier;
                init_slice(&ret.data._identifier.value, p->input, &id_rn);
            }
            else
            {
                p->it = bool_p.it;
                ret.type = parsed_object_type::Bool;
                ret.data._bool = to_lower(bool_p.input[bool_rn.start.pos]) == 't';
            }
        }
        else
        {
            if (id_success && num_success)
            {
                if (id_p.it.pos > num_p.it.pos)
                {
                    p->it = id_p.it;
                    ret.type = parsed_object_type::Identifier;
                    init_slice(&ret.data._identifier.value, p->input, &id_rn);
                }
                else
                {
                    p->it = num_p.it;
                    ret.type = num_obj.type;
                    ret.data = num_obj.data;
                }
            }
            else if (id_success)
            {
                p->it = id_p.it;
                ret.type = parsed_object_type::Identifier;
                init_slice(&ret.data._identifier.value, p->input, &id_rn);
            }
            else if (num_success)
            {
                p->it = num_p.it;
                ret.type = num_obj.type;
                ret.data = num_obj.data;
            }
            else
            {
                // nothing parsed, eject the furthest error
                if (bool_err.it.pos >= id_err.it.pos
                 && bool_err.it.pos >= num_err.it.pos)
                {
                    out->type = parsed_object_type::None;
                    *err = bool_err;
                    return false;
                }
                else if (num_err.it.pos >= id_err.it.pos)
                {
                    out->type = parsed_object_type::None;
                    *err = num_err;
                    return false;
                }
                else
                {
                    out->type = parsed_object_type::None;
                    *err = id_err;
                    return false;
                }
            }
        }
    }

    out->type = ret.type;
    out->data = ret.data;

    return true;
}

bool parse_object(parser<char> *p, parsed_object_base<char> *out, parse_error<char> *err)
{
    return _parse_object(p, out, err);
}

bool parse_object(parser<wchar_t> *p, parsed_object_base<wchar_t> *out, parse_error<wchar_t> *err)
{
    return _parse_object(p, out, err);
}

template<typename C>
void _free(parsed_object_base<C> *obj)
{
    assert(obj != nullptr);

    switch (obj->type)
    {
    case parsed_object_type::String:
        free(&obj->data._string);
        break;
    case parsed_object_type::Identifier:
        free(&obj->data._identifier.value);
        break;
    case parsed_object_type::List:
        free<true>(&obj->data._list);
        break;
    case parsed_object_type::Table:
        free<true, true>(&obj->data._table);
        break;
    default:
        break;
    }
}

void free(parsed_object  *obj)
{
    _free(obj);
}

void free(wparsed_object *obj)
{
    _free(obj);
}

template<typename C>
s64 _parsed_object_to_string(string_base<C> *s, const parsed_object_base<C> *x, u64 offset, format_options<C> opt)
{
    s64 written = 0;

    switch (x->type)
    {
    case parsed_object_type::Bool:
        written += to_string(s, x->data._bool, offset, opt, true);
        break;

    case parsed_object_type::Integer:
        // can't really supply number format info here since all formatting info is lost
        // on parsing, so just write normal signed integer.
        written += to_string(s, x->data._integer, offset, opt /*, ...*/);
        break;

    case parsed_object_type::Decimal:
        written += to_string(s, x->data._decimal, offset, opt /*, ...*/);
        break;

    case parsed_object_type::String:
    {
        u64 len = string_length(&x->data._string);
        string_reserve(s, offset + 2 + len);

        s->data.data[offset] = PARSE_STRING_DELIM;
        written++;

        copy_string(&x->data._string, s, len, written + offset);
        written += len;

        s->data.data[offset + written] = PARSE_STRING_DELIM;
        written++;
        break;
    }

    case parsed_object_type::Identifier:
    {
        u64 len = string_length(&x->data._string);
        string_reserve(s, offset + len);

        copy_string(&x->data._string, s, len, written + offset);
        written += len;
        break;
    }

    case parsed_object_type::List:
    {
        string_reserve(s, offset + 1);
        s->data.data[offset] = PARSE_LIST_OPENING_BRACKET;
        written++;

        auto *list = &x->data._list;

        if (list->size != 0)
        {
            auto *node = list->first;

            written += to_string(s, &node->value, offset + written);
            node = node->next;

            while (node != nullptr)
            {
                string_reserve(s, offset + written + 2);

                s->data.data[offset + written] = PARSE_LIST_ITEM_DELIM;
                written++;
                s->data.data[offset + written] = ' ';
                written++;

                written += to_string(s, &node->value, offset + written);
                node = node->next;
            }
        }

        string_reserve(s, offset + written + 1);
        s->data.data[offset + written] = PARSE_LIST_CLOSING_BRACKET;
        written++;

        break;
    }

    case parsed_object_type::Table:
    {
        string_reserve(s, offset + 1);
        s->data.data[offset] = PARSE_TABLE_OPENING_BRACKET;
        written++;

        auto *table = &x->data._table;

        if (table->size != 0)
        {
            u64 i = 0;
            auto *node = table->data.data;

            while (i < table->data.size)
            {
                node = table->data.data + i;

                if (node->hash >= FIRST_HASH)
                    break;

                i++;
            }

            if (node->hash >= FIRST_HASH)
            {
                written += to_string(s, &node->key, offset + written);
                string_reserve(s, offset + written + 3);

                s->data.data[offset + written] = ' ';
                written++;
                s->data.data[offset + written] = PARSE_TABLE_KEY_VALUE_DELIM;
                written++;
                s->data.data[offset + written] = ' ';
                written++;

                written += to_string(s, &node->value, offset + written);

                i++;

                while (i < table->data.size)
                {
                    node = table->data.data + i;

                    if (node->hash < FIRST_HASH)
                    {
                        i++;
                        continue;
                    }

                    string_reserve(s, offset + written + 2);

                    s->data.data[offset + written] = PARSE_TABLE_ITEM_DELIM;
                    written++;
                    s->data.data[offset + written] = ' ';
                    written++;

                    written += to_string(s, &node->key, offset + written);
                    string_reserve(s, offset + written + 3);

                    s->data.data[offset + written] = ' ';
                    written++;
                    s->data.data[offset + written] = PARSE_TABLE_KEY_VALUE_DELIM;
                    written++;
                    s->data.data[offset + written] = ' ';
                    written++;

                    written += to_string(s, &node->value, offset + written);

                    i++;
                }
            }
        }

        string_reserve(s, offset + written + 1);
        s->data.data[offset + written] = PARSE_TABLE_CLOSING_BRACKET;
        written++;

        break;
    }
    default:
        break;
    }

    return written;
}

#define to_string_body(F, String, Value, Offset, Options, ...)\
{\
    s64 written = F(String, Value, Offset, Options __VA_OPT__(,) __VA_ARGS__);\
\
    if (Options.pad_length < 0) written += pad_string(String, Options.pad_char, -Options.pad_length - written, written + Offset);\
\
    if (written + Offset >= String->data.size)\
    {\
        String->data.size = written + Offset;\
        String->data.data[String->data.size] = '\0';\
    }\
\
    return written;\
}

s64 to_string(string  *s, const parsed_object  *x)             to_string_body(_parsed_object_to_string, s, x, 0, default_format_options<char>);
s64 to_string(string  *s, const parsed_object  *x, u64 offset) to_string_body(_parsed_object_to_string, s, x, offset, default_format_options<char>);
s64 to_string(string  *s, const parsed_object  *x, u64 offset, format_options<char> opt) to_string_body(_parsed_object_to_string, s, x, offset, opt);
s64 to_string(wstring *s, const wparsed_object *x)             to_string_body(_parsed_object_to_string, s, x, 0, default_format_options<wchar_t>);
s64 to_string(wstring *s, const wparsed_object *x, u64 offset) to_string_body(_parsed_object_to_string, s, x, offset, default_format_options<wchar_t>);
s64 to_string(wstring *s, const wparsed_object *x, u64 offset, format_options<wchar_t> opt) to_string_body(_parsed_object_to_string, s, x, offset, opt);
