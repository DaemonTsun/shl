
#include "shl/string.hpp"
#include "shl/parse_object.hpp"

void init_slice(const_string *out, const char *input, const parse_range *range)
{
    s64 len = range_length(range);
    char *buf = alloc<char>(len);
    out->c_str = buf;
    out->size = len;
    string_copy(input + range->start.pos, buf, len);
}

static void free(const_string *str)
{
    dealloc((char*)str->c_str, str->size);
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

bool operator==(const parsed_identifier &lhs, const parsed_identifier &rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(parsed_object &lhs, parsed_object &rhs)
{
    if (lhs.type != rhs.type)
        return false;

    switch (lhs.type)
    {
    case parsed_object_type::None:       return true;
    case parsed_object_type::Bool:       return lhs.data._bool == rhs.data._bool;
    case parsed_object_type::Integer:    return lhs.data._integer == rhs.data._integer;
    case parsed_object_type::Decimal:    return lhs.data._decimal == rhs.data._decimal;
    case parsed_object_type::String:     return lhs.data._string == rhs.data._string;
    case parsed_object_type::Identifier: return lhs.data._identifier.value == rhs.data._identifier.value;
    case parsed_object_type::List:       return lhs.data._list == rhs.data._list;
    case parsed_object_type::Table:      return lhs.data._table == rhs.data._table;
    }

    return false;
}

bool parse_number_object(parser *p, parsed_object *obj, parse_error *err)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    bool int_success = false;
    bool dec_success = false;
    parser int_p = *p;
    parser dec_p = *p;
    parse_error int_err;
    parse_error dec_err;

    typename parsed_object::integer_type integer;
    typename parsed_object::decimal_type decimal;

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

bool parse_number_object(const_string   input, parsed_object  *obj, parse_error *err)
{
    parser p;
    init(&p, input);
    return parse_number_object(&p, obj, err);
}

bool parse_number_object(const string  *input, parsed_object  *obj, parse_error *err)
{
    return parse_number_object(to_const_string(input), obj, err);
}

bool parse_object_list(parser *p, object_list *out, parse_error *err)
{
    init(out);

    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;

    char c = parser_current_char(p);

    if (c != PARSE_LIST_OPENING_BRACKET)
    {
        format_parse_error(err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in list at " IT_FMT, c, format_it(p->it), PARSE_LIST_OPENING_BRACKET, format_it(start));
        return false;
    }

    advance(&p->it);

    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        format_parse_error(err, p, "unterminated list starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = parser_current_char(p);

    if (c == PARSE_LIST_CLOSING_BRACKET)
    {
        advance(&p->it);
        return true;
    }

    parsed_object obj;

    if (!parse_object(p, &obj, err))
    {
        p->it = start;
        return false;
    }
        
    add_elements(out, 1)->value = obj;

    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        format_parse_error(err, p, "unterminated list starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = parser_current_char(p);

    while (c == PARSE_LIST_ITEM_DELIM)
    {
        advance(&p->it);

        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "unterminated list starting at " IT_FMT, format_it(start));
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
            format_parse_error(err, p, "unterminated list starting at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);
    }

    if (c != PARSE_LIST_CLOSING_BRACKET)
    {
        format_parse_error(err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in list at " IT_FMT, c, format_it(p->it), PARSE_LIST_CLOSING_BRACKET, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    return true;
}

bool parse_object_list(const_string   input, object_list  *obj, parse_error *err)
{
    parser p;
    init(&p, input);
    return parse_object_list(&p, obj, err);
}

bool parse_object_list(const string  *input, object_list  *obj, parse_error *err)
{
    return parse_object_list(to_const_string(input), obj, err);
}

bool parse_object_table(parser *p, object_table *out, parse_error *err)
{
    init(out);

    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;

    char c = parser_current_char(p);

    if (c != PARSE_TABLE_OPENING_BRACKET)
    {
        format_parse_error(err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_OPENING_BRACKET, format_it(start));
        return false;
    }

    advance(&p->it);
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        format_parse_error(err, p, "unterminated table starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = parser_current_char(p);

    if (c == PARSE_TABLE_CLOSING_BRACKET)
    {
        advance(&p->it);
        return true;
    }

    typename parsed_object::identifier_type ident;
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
        format_parse_error(err, p, "unexpected EOF at " IT_FMT ", expected '%c' in table at " IT_FMT, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
        p->it = start;
        return false;
    }

    c = parser_current_char(p);

    if (c != PARSE_TABLE_KEY_VALUE_DELIM)
    {
        format_parse_error(err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    parsed_object obj;

    if (!parse_object(p, &obj, err))
    {
        p->it = start;
        return false;
    }

    if (contains(out, &ident.value))
    {
        format_parse_error(err, p, "duplicate key '%s' at " IT_FMT ", in table at " IT_FMT, ident.value.c_str, format_it(p->it), format_it(start));
        p->it = start;
        return false;
    }

    *add_element_by_key(out, &ident.value) = obj;
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        format_parse_error(err, p, "unterminated table starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = parser_current_char(p);

    while (c == PARSE_TABLE_ITEM_DELIM)
    {
        advance(&p->it);
        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "unterminated table starting at " IT_FMT, format_it(start));
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
            format_parse_error(err, p, "unexpected EOF at " IT_FMT ", expected '%c' in table at " IT_FMT, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);

        if (c != PARSE_TABLE_KEY_VALUE_DELIM)
        {
            format_parse_error(err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_KEY_VALUE_DELIM, format_it(start));
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
            format_parse_error(err, p, "duplicate key '%s' at " IT_FMT ", in table at " IT_FMT, ident.value.c_str, format_it(p->it), format_it(start));
            p->it = start;
            return false;
        }

        *add_element_by_key(out, &ident.value) = obj;
        skip_whitespace_and_comments(p);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "unterminated table starting at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);
    }

    if (c != PARSE_TABLE_CLOSING_BRACKET)
    {
        format_parse_error(err, p, "unexpected '%c' at " IT_FMT ", expected '%c' in table at " IT_FMT, c, format_it(p->it), PARSE_TABLE_CLOSING_BRACKET, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    return true;
}

bool parse_object_table(const_string   input, object_table  *obj, parse_error *err)
{
    parser p;
    init(&p, input);
    return parse_object_table(&p, obj, err);
}

bool parse_object_table(const string  *input, object_table  *obj, parse_error *err)
{
    return parse_object_table(to_const_string(input), obj, err);
}

bool parse_object(parser *p, parsed_object *out, parse_error *err)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        out->type = parsed_object_type::None;
        return false;
    }

    parse_iterator start = p->it;
    skip_whitespace_and_comments(p);

    if (is_at_end(p))
    {
        format_parse_error(err, p, "no object at " IT_FMT, format_it(start));
        out->type = parsed_object_type::None;
        p->it = start;
        return false;
    }

    char c = parser_current_char(p);

    parsed_object ret;

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
        parser bool_p = *p;
        parse_range bool_rn;
        parse_error bool_err;
        bool bool_success = parse_bool(&bool_p, &bool_rn, &bool_err);
        
        parser id_p = *p;
        parse_range id_rn;
        parse_error id_err;
        bool id_success = parse_identifier(&id_p, &id_rn, &id_err);

        parser num_p = *p;
        parse_error num_err;
        parsed_object num_obj;
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
                ret.data._bool = char_to_lower(bool_p.input[bool_rn.start.pos]) == 't';
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

bool parse_object(const_string   input, parsed_object  *obj, parse_error *err)
{
    parser p;
    init(&p, input);
    return parse_object(&p, obj, err);
}

bool parse_object(const string  *input, parsed_object  *obj, parse_error *err)
{
    return parse_object(to_const_string(input), obj, err);
}

void free(parsed_object  *obj)
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
    case parsed_object_type::Bool:
    case parsed_object_type::Integer:
    case parsed_object_type::Decimal:
    case parsed_object_type::None:
    default:
        break;
    }
}

static s64 _parsed_object_to_string(string *s, const parsed_object *x, s64 offset, format_options<char> opt)
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
        s64 len = string_length(x->data._string);
        string_reserve(s, offset + 2 + len);

        s->data[offset] = PARSE_STRING_DELIM;
        written++;

        string_copy(x->data._string, s, len, written + offset);
        written += len;

        s->data[offset + written] = PARSE_STRING_DELIM;
        written++;
        break;
    }

    case parsed_object_type::Identifier:
    {
        s64 len = string_length(x->data._string);
        string_reserve(s, offset + len);

        string_copy(x->data._string, s, len, written + offset);
        written += len;
        break;
    }

    case parsed_object_type::List:
    {
        string_reserve(s, offset + 1);
        s->data[offset] = PARSE_LIST_OPENING_BRACKET;
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

                s->data[offset + written] = PARSE_LIST_ITEM_DELIM;
                written++;
                s->data[offset + written] = ' ';
                written++;

                written += to_string(s, &node->value, offset + written);
                node = node->next;
            }
        }

        string_reserve(s, offset + written + 1);
        s->data[offset + written] = PARSE_LIST_CLOSING_BRACKET;
        written++;

        break;
    }

    case parsed_object_type::Table:
    {
        string_reserve(s, offset + 1);
        s->data[offset] = PARSE_TABLE_OPENING_BRACKET;
        written++;

        auto *table = &x->data._table;

        if (table->size != 0)
        {
            s64 i = 0;
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
                written += to_string(s, node->key, offset + written);
                string_reserve(s, offset + written + 3);

                s->data[offset + written] = ' ';
                written++;
                s->data[offset + written] = PARSE_TABLE_KEY_VALUE_DELIM;
                written++;
                s->data[offset + written] = ' ';
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

                    s->data[offset + written] = PARSE_TABLE_ITEM_DELIM;
                    written++;
                    s->data[offset + written] = ' ';
                    written++;

                    written += to_string(s, node->key, offset + written);
                    string_reserve(s, offset + written + 3);

                    s->data[offset + written] = ' ';
                    written++;
                    s->data[offset + written] = PARSE_TABLE_KEY_VALUE_DELIM;
                    written++;
                    s->data[offset + written] = ' ';
                    written++;

                    written += to_string(s, &node->value, offset + written);

                    i++;
                }
            }
        }

        string_reserve(s, offset + written + 1);
        s->data[offset + written] = PARSE_TABLE_CLOSING_BRACKET;
        written++;

        break;
    }

    case parsed_object_type::None:
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
    if (written + Offset >= String->size)\
    {\
        String->size = written + Offset;\
        String->data[String->size] = '\0';\
    }\
\
    return written;\
}

s64 to_string(string  *s, const parsed_object  *x)             to_string_body(_parsed_object_to_string, s, x, 0, default_format_options<char>);
s64 to_string(string  *s, const parsed_object  *x, s64 offset) to_string_body(_parsed_object_to_string, s, x, offset, default_format_options<char>);
s64 to_string(string  *s, const parsed_object  *x, s64 offset, format_options<char> opt) to_string_body(_parsed_object_to_string, s, x, offset, opt);
