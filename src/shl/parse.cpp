
#include "shl/parse.hpp"

#define SKIP_COND(c, p, COND) \
    while (COND(c)) \
    { \
        advance(&p->it); \
        if (is_at_end(p)) \
            break; \
        c = parser_current_char(p); \
    }

void init(parse_iterator *it)
{
    it->pos = 0;
    it->line_start = 0;
    it->line = 1;
    it->line_pos = 1;
}

void update_iterator_line_pos(parse_iterator *it)
{
    it->line_pos = (it->pos - it->line_start) + 1;
}

void advance(parse_iterator *it, s64 n)
{
    it->pos += n;
    update_iterator_line_pos(it);
}

void next_line(parse_iterator *it)
{
    it->line++;
    it->line_start = it->pos;
}

static inline void _init(parser_base<char> *p, const char *input, s64 input_size)
{
    init(&p->it);
    p->input = input;
    p->input_size = input_size;
}

void init(parser *p, const char *input, s64 input_size)
{
    _init(p, input, input_size);
}

void init(parser    *p, const_string   input)
{
    _init(p, input.c_str, input.size);
}

void init(parser    *p, const string  *input)
{
    _init(p, input->data, string_length(input));
}

bool is_at_end(const parser *p)
{
    return p->it.pos >= p->input_size;
}

bool is_at_end(const parser *p, s64 offset)
{
    return (p->it.pos + offset) >= p->input_size;
}

bool is_ok(const parser *p)
{
    return (p != nullptr) && (p->input != nullptr) && !is_at_end(p);
}

s64 range_length(const parse_range *range)
{
    return range->end.pos - range->start.pos;
}

//
// parsing functions
//
static inline bool _skip_whitespace(parser_base<char> *p)
{
    if (!is_ok(p))
        return false;

    char c;
    bool skipped = false;

    while (!is_at_end(p))
    {
        c = parser_current_char(p);

        if (is_newline(c))
        {
            skipped = true;
            p->it.pos++;
            next_line(&p->it);
            update_iterator_line_pos(&p->it);
        }
        else if (is_space(c))
        {
            skipped = true;
            advance(&p->it);
        }
        else
            break;
    }

    return skipped;
}

bool skip_whitespace(parser *p)
{
    return _skip_whitespace(p);
}

static inline bool _parse_comment(parser_base<char> *p, parse_range *out)
{
    parse_iterator start = p->it;
    parse_iterator comment_start{};
    parse_iterator comment_end{};

    bool has_comment = false;

    skip_whitespace(p);
    char c;

    if (!is_ok(p))
    {
        has_comment = false;
        goto parse_comment_end;
    }

    c = parser_current_char(p);

    if (c != '/')
    {
        has_comment = false;
        goto parse_comment_end;
    }

    advance(&p->it);

    if (is_at_end(p))
    {
        has_comment = false;
        goto parse_comment_end;
    }

    c = parser_current_char(p);

    if (c == '/')
    {
        advance(&p->it);
        comment_start = p->it;

        // line comment
        while (!is_at_end(p) && parser_current_char(p) != '\n')
            advance(&p->it);

        if (is_at_end(p))
        {
            comment_end = p->it;
            has_comment = true;
            goto parse_comment_end;
        }
        else
        {
            p->it.pos++;
            next_line(&p->it);
            update_iterator_line_pos(&p->it);
            comment_end = p->it;
            has_comment = true;
            goto parse_comment_end;
        }
    }
    else if (c == '*')
    {
        /* block comment */ 
        advance(&p->it);

        comment_start = p->it;

        while (!is_at_end(p))
        {
            skip_whitespace(p);

            if (is_at_end(p))
                break;

            c = parser_current_char(p);

            if (c == '*')
            {
                advance(&p->it);

                if (is_at_end(p))
                    break;

                c = parser_current_char(p);

                if (c == '/')
                {
                    comment_end = p->it;
                    advance(&comment_end, -1);

                    advance(&p->it);
                    has_comment = true;
                    goto parse_comment_end;
                }
            }

            advance(&p->it);
        }

        has_comment = false;
        goto parse_comment_end;
    }

parse_comment_end:
    if (has_comment && out != nullptr)
    {
        out->start = comment_start;
        out->end = comment_end;
    }
    else if (!has_comment)
        p->it = start;


    return has_comment;
}

bool parse_comment(parser *p, parse_range *out)
{
    return _parse_comment(p, out);
}

static inline bool _skip_whitespace_and_comments(parser_base<char> *p)
{
    bool ok = false;

    do
    {
        ok = parse_comment(p, nullptr);
        ok = ok || skip_whitespace(p);
    } while (ok);

    return ok;
}

bool skip_whitespace_and_comments(parser *p)
{
    return _skip_whitespace_and_comments(p);
}

static inline bool _parse_string(parser_base<char> *p, parse_range *out, parse_error *err, char delim, bool include_delims)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    auto c = parser_current_char(p);

    if (c != delim)
    {
        format_parse_error(err, p, "unexpected symbol '%c' at " IT_FMT ", expected '%c'", c, format_it(start), delim);
        return false;
    }

    advance(&p->it);

    if (is_at_end(p))
    {
        format_parse_error(err, p, "unterminated string starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    c = parser_current_char(p);

    while (c != delim)
    {
        if (is_space(c))
        {
            skip_whitespace(p);
        }
        else if (c == '\\')
        {
            advance(&p->it, 2);
        }
        else
            advance(&p->it);

        if (is_at_end(p))
            break;

        c = parser_current_char(p);
    }

    if (c != delim)
    {
        format_parse_error(err, p, "unterminated string starting at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }

    advance(&p->it);

    if (out != nullptr)
    {
        if (include_delims)
        {
            out->start = start;
            out->end = p->it;
        }
        else
        {
            out->start = start;
            advance(&out->start, 1);
            out->end = p->it;
            advance(&out->end, -1);
        }
    }

    return true;
}

bool parse_string(parser *p, parse_range *out, parse_error *err, char delim, bool include_delims)
{
    return _parse_string(p, out, err, delim, include_delims);
}

template<typename C>
static inline bool _parse_bool(parser_base<C> *p, parse_range *out, parse_error *err)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    auto c = parser_current_char(p);
    
    if (to_lower(c) == 't')
    {
        if (is_at_end(p, 3))
        {
            advance(&p->it, p->input_size - p->it.pos);
            format_parse_error(err, p, "not a boolean at" IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        for (int i = 0; i < 4;)
        {
            if (is_at_end(p))
            {
                format_parse_error(err, p, "unexpected end of input at " IT_FMT, format_it(p->it));
                p->it = start;
                return false;
            }

            c = parser_current_char(p);

            if (to_lower(c) != "true"[i])
            {
                format_parse_error(err, p, "unexpected symbol '%c' at " IT_FMT, c, format_it(p->it));
                p->it = start;
                return false;
            }

            advance(&p->it);
            ++i;
        }
    }
    else if (to_lower(c) == 'f')
    {
        if (is_at_end(p, 4))
        {
            advance(&p->it, p->input_size - p->it.pos);
            format_parse_error(err, p, "not a boolean at" IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        for (int i = 0; i < 5;)
        {
            if (is_at_end(p))
            {
                format_parse_error(err, p, "unexpected end of input at " IT_FMT, format_it(p->it));
                p->it = start;
                return false;
            }

            c = parser_current_char(p);

            if (to_lower(c) != "false"[i])
            {
                format_parse_error(err, p, "unexpected symbol '%c' at " IT_FMT, c, format_it(p->it));
                p->it = start;
                return false;
            }

            advance(&p->it);
            ++i;
        }
    }
    else 
    {
        format_parse_error(err, p, "unexpected symbol '%c' at " IT_FMT, c, format_it(p->it));
        p->it = start;
        return false;
    }

    if (out != nullptr)
    {
        out->start = start;
        out->end = p->it;
    }

    return true;
}

bool parse_bool(parser *p, parse_range *out, parse_error *err)
{
    return _parse_bool(p, out, err);
}

template<typename C>
static inline bool _parse_bool_v(parser_base<C> *p, bool *out, parse_error *err)
{
    parse_range range;
    if (!_parse_bool(p, &range, err))
        return false;

    *out = to_lower(p->input[range.start.pos]) == 't';

    return true;
}

bool parse_bool(parser *p, bool *out, parse_error *err)
{
    return _parse_bool_v(p, out, err);
}

template<typename C>
static inline bool _parse_integer(parser_base<C> *p, parse_range *out, parse_error *err)
{
    if (!is_ok(p))
    {
        format_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    auto c = parser_current_char(p);

    if (c == '-' || c == '+')
    {
        advance(&p->it);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "not an integer at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);
        
        if (!is_digit(c))
        {
            format_parse_error(err, p, "not an integer at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }
    }

    if (c == '0')
    {
        advance(&p->it);

        if (!is_at_end(p))
        {
            c = parser_current_char(p);

            if (c == 'x' || c == 'X')
            {
                advance(&p->it);

                if (is_at_end(p))
                {
                    format_parse_error(err, p, "invalid hex integer at " IT_FMT, format_it(start));
                    p->it = start;
                    return false;
                }

                c = parser_current_char(p);

                if (!is_hex_digit(c))
                {
                    format_parse_error(err, p, "invalid hex digit '%c' at " IT_FMT " in hex integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
                    p->it = start;
                    return false;
                }

                SKIP_COND(c, p, is_hex_digit);
            }
            else if (c == 'b' || c == 'B')
            {
                advance(&p->it);

                if (is_at_end(p))
                {
                    format_parse_error(err, p, "invalid binary integer at " IT_FMT, format_it(start));
                    p->it = start;
                    return false;
                }

                c = parser_current_char(p);

                if (!is_bin_digit(c))
                {
                    format_parse_error(err, p, "invalid binary digit '%c' at " IT_FMT " in binary integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
                    p->it = start;
                    return false;
                }

                SKIP_COND(c, p, is_bin_digit);
            }
            else
            {
                advance(&p->it);

                if (is_at_end(p))
                {
                    format_parse_error(err, p, "invalid octal integer at " IT_FMT, format_it(start));
                    p->it = start;
                    return false;
                }

                c = parser_current_char(p);

                if (!is_oct_digit(c))
                {
                    format_parse_error(err, p, "invalid octal digit '%c' at " IT_FMT " in octal integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
                    p->it = start;
                    return false;
                }
                
                SKIP_COND(c, p, is_oct_digit);
            }
        }
    }
    else if (is_hex_digit(c))
    {
        if (!is_digit(c))
        {
            SKIP_COND(c, p, is_hex_digit);
        }
        else 
        {
            SKIP_COND(c, p, is_digit);
        }
    }
    else
    {
        format_parse_error(err, p, "invalid integer digit '%c' at " IT_FMT " in integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
        p->it = start;
        return false;
    }
        
    if (out != nullptr)
    {
        out->start = start;
        out->end = p->it;
    }

    return true;
}

bool parse_integer(parser *p, parse_range *out, parse_error *err)
{
    return _parse_integer(p, out, err);
}

template<typename OutT = s64>
static inline bool _parse_integer_t(parser_base<char> *p, parse_iterator *digit_start, int *base, bool *neg, parse_error *err)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    *digit_start = start;
    auto c = parser_current_char(p);

    if (c == '-' || c == '+')
    {
        if (c == '-')
            *neg = true;

        advance(&p->it);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "not an integer at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);
        
        if (!is_digit(c))
        {
            format_parse_error(err, p, "not an integer at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        *digit_start = p->it;
    }

    if (c == '0')
    {
        advance(&p->it);

        if (!is_at_end(p))
        {
            c = parser_current_char(p);

            if (c == 'x' || c == 'X')
            {
                advance(&p->it);

                if (is_at_end(p))
                {
                    format_parse_error(err, p, "invalid hex integer at " IT_FMT, format_it(start));
                    p->it = start;
                    return false;
                }

                c = parser_current_char(p);

                if (!is_hex_digit(c))
                {
                    format_parse_error(err, p, "invalid hex digit '%c' at " IT_FMT " in hex integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
                    p->it = start;
                    return false;
                }

                *base = 16;
                *digit_start = p->it;

                SKIP_COND(c, p, is_hex_digit);
            }
            else if (c == 'b' || c == 'B')
            {
                advance(&p->it);

                if (is_at_end(p))
                {
                    format_parse_error(err, p, "invalid binary integer at " IT_FMT, format_it(start));
                    p->it = start;
                    return false;
                }


                c = parser_current_char(p);

                if (!is_bin_digit(c))
                {
                    format_parse_error(err, p, "invalid binary digit '%c' at " IT_FMT " in binary integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
                    p->it = start;
                    return false;
                }

                *base = 2;
                *digit_start = p->it;

                SKIP_COND(c, p, is_bin_digit);
            }
            else
            {
                advance(&p->it);

                if (is_at_end(p))
                {
                    format_parse_error(err, p, "invalid octal integer at " IT_FMT, format_it(start));
                    p->it = start;
                    return false;
                }

                c = parser_current_char(p);

                if (!is_oct_digit(c))
                {
                    format_parse_error(err, p, "invalid octal digit '%c' at " IT_FMT " in octal integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
                    p->it = start;
                    return false;
                }
                
                *base = 8;

                SKIP_COND(c, p, is_oct_digit);
            }
        }
    }
    else if (is_hex_digit(c))
    {
        if (!is_digit(c))
        {
            *base = 16;

            SKIP_COND(c, p, is_hex_digit);
        }
        else 
        {
            *base = 10;

            SKIP_COND(c, p, is_digit);
        }
    }
    else
    {
        format_parse_error(err, p, "invalid integer digit '%c' at " IT_FMT " in integer starting at " IT_FMT, c, format_it(p->it), format_it(start));
        p->it = start;
        return false;
    }

    return true;
}

#define DEFINE_PARSE_INTEGER(C, OutT, FUNC)\
bool parse_integer(parser_base<char> *p, OutT *out, parse_error *err)\
{\
    parse_iterator start = p->it;\
    parse_iterator digit_start;\
    int base = 10;\
    bool neg = false;\
\
    if (!_parse_integer_t(p, &digit_start, &base, &neg, err))\
        return false;\
\
    constexpr const s64 digit_size = sizeof(OutT) * 8 + 4;\
    s64 len = p->it.pos - digit_start.pos;\
\
    if (len > digit_size - 1)\
    {\
        format_parse_error(err, p, "integer too large at " IT_FMT, format_it(start));\
        p->it = start;\
        return false;\
    }\
\
    C buf[digit_size] = {(C)0};\
    substring(p->input, digit_start.pos, len, buf);\
    buf[len] = (C)0;\
\
    OutT ret = FUNC(buf, nullptr, base);\
\
    if (neg)\
        ret = -ret;\
\
    *out = ret;\
\
    return true;\
}

DEFINE_PARSE_INTEGER(char, int, to_int);
DEFINE_PARSE_INTEGER(char, long, to_long);
DEFINE_PARSE_INTEGER(char, long long, to_long_long);
DEFINE_PARSE_INTEGER(char, unsigned int, to_unsigned_int);
DEFINE_PARSE_INTEGER(char, unsigned long, to_unsigned_long);
DEFINE_PARSE_INTEGER(char, unsigned long long, to_unsigned_long_long);

static inline bool _parse_decimal(parser_base<char> *p, parse_range *out, parse_error *err)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    auto c = parser_current_char(p);
    bool has_digits = false;

    if (c == '-' || c == '+')
    {
        advance(&p->it);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "not a decimal number at " IT_FMT, format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);
    }

    while (is_digit(c))
    {
        has_digits = true;
        advance(&p->it);

        if (is_at_end(p))
            break;

        c = parser_current_char(p);
    }

    if (is_at_end(p))
        goto parse_decimal_end;

    c = parser_current_char(p);

    if (c == '.')
    {
        advance(&p->it);

        if (is_at_end(p))
        {
            if (has_digits)
                goto parse_decimal_end;
            else
            {
                format_parse_error(err, p, "unexpected end of input at " IT_FMT " in decimal number at " IT_FMT, format_it(p->it), format_it(start));
                p->it = start;
                return false;
            }
        }

        c = parser_current_char(p);

        while (is_digit(c))
        {
            has_digits = true;
            advance(&p->it);

            if (is_at_end(p))
                break;

            c = parser_current_char(p);
        }
    }

    if (c == 'e' || c == 'E')
    {
        if (!has_digits)
        {
            format_parse_error(err, p, "unexpected symbol '%c' at " IT_FMT " in decimal number at " IT_FMT, c, format_it(p->it), format_it(start));
            p->it = start;
            return false;
        }

        advance(&p->it);

        if (is_at_end(p))
        {
            format_parse_error(err, p, "exponent at requires a value at " IT_FMT " in decimal number at " IT_FMT, format_it(p->it), format_it(start));
            p->it = start;
            return false;
        }

        c = parser_current_char(p);

        if (c == '+' || c == '-')
        {
            advance(&p->it);

            if (is_at_end(p))
            {
                format_parse_error(err, p, "exponent at requires a value at " IT_FMT " in decimal number at " IT_FMT, format_it(p->it), format_it(start));
                p->it = start;
                return false;
            }

            c = parser_current_char(p);
        }

        while (is_digit(c))
        {
            has_digits = true;
            advance(&p->it);

            if (is_at_end(p))
                break;

            c = parser_current_char(p);
        }
    }

    if (!has_digits)
    {
        format_parse_error(err, p, "not a decimal number at " IT_FMT, format_it(start));
        p->it = start;
        return false;
    }
        
parse_decimal_end:

    if (out != nullptr)
    {
        out->start = start;
        out->end = p->it;
    }

    return true;
}

bool parse_decimal(parser *p, parse_range *out, parse_error *err)
{
    return _parse_decimal(p, out, err);
}

// ok a double can have like 1024 digits, but this will suffice for simple things.
#define DEFINE_PARSE_DECIMAL(C, OutT, FUNC)\
bool parse_decimal(parser_base<C> *p, OutT *out, parse_error *err)\
{\
    if (!is_ok(p))\
    {\
        set_parse_error(err, p, "input is nullptr or end was reached");\
        return false;\
    }\
\
    parse_iterator start = p->it;\
    parse_range range;\
    \
    if (!_parse_decimal(p, &range, err))\
        return false;\
\
    constexpr const s64 digit_size = 128;\
    s64 len = range_length(&range);\
\
    if (len > digit_size - 1)\
    {\
        format_parse_error(err, p, "floating point number too large at " IT_FMT, format_it(start));\
        p->it = start;\
        return false;\
    }\
\
    C buf[digit_size];\
    substring(p->input, range.start.pos, len, buf);\
    buf[len] = 0;\
\
    *out = FUNC(buf, nullptr);\
\
    return true;\
}

DEFINE_PARSE_DECIMAL(char, float, to_float);
DEFINE_PARSE_DECIMAL(char, double, to_double);
DEFINE_PARSE_DECIMAL(char, long double, to_long_double);

bool is_first_identifier_character(char c)
{
    return c == '_' || is_alpha(c);
}

bool is_identifier_character(char c)
{
    return c == '_' || is_alphanum(c);
}

static inline bool _parse_identifier(parser_base<char> *p, parse_range *out, parse_error *err)
{
    if (!is_ok(p))
    {
        set_parse_error(err, p, "input is nullptr or end was reached");
        return false;
    }

    parse_iterator start = p->it;
    auto c = parser_current_char(p);

    if (!is_first_identifier_character(c))
    {
        format_parse_error(err, p, "unexpected symbol '%c' at " IT_FMT " in identifier starting at " IT_FMT, c, format_it(p->it), format_it(start));
        p->it = start;
        return false;
    }

    SKIP_COND(c, p, is_identifier_character);

    if (out != nullptr)
    {
        out->start = start;
        out->end = p->it;
    }

    return true;
}

bool parse_identifier(parser *p, parse_range *out, parse_error *err)
{
    return _parse_identifier(p, out, err);
}

