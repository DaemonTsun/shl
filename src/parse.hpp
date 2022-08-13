
/* parse.hpp
 *  depends on string.hpp and number_types.hpp
 * 
 * parsing functions
 * using an iterator (parse_iterator) to keep track of where one is
 * in a string, this file provides functions which uses this iterator
 * to parse certain sequences into basic types and strings.
 *
 * all parsing functions have the convention
 *      parse_X(it, input, input_size, ...)
 *      parse_X(it, input, input_size, outrange, err, ...)
 *      or
 *      parse_X(it, input, input_size, ..., success)
 *
 * where X is the thing parsed, 'it' is the iterator, input is a character
 * pointer of size input_size, and ... are additional arguments, often
 * for output of parsed values. input_size is treated as the EOF of the
 * input, not null characters.
 * overloads with outrange and err dont convert the parsed value and dont
 * throw, instead passing the range in the input where the value is
 * located at and the error, if an error occurred.
 * functions that dont throw by default only have a success parameter.
 *
 * all parsing functions return a new iterator which is directly after the
 * successfully parsed value.
 *
 * if unsuccessful, throws a parse_error with failure information detailing
 * where the failure occurred.
 *
 * the parsing functions (<params> = it, input, input_size):
 *
 * skip_whitespace(<params>)
 *      parses all whitespaces including newlines and returns the new iterator
 *      at the next non-whitespace symbol or EOF.
 *      does not throw.
 *
 * parse_comment(<params>, [*out], [*success])
 *      parses a single line or block comment and optionally writes it to *out
 *      if out is not nullptr.
 *      also skips whitespaces before the comment.
 *      also writes to *success if a comment was parsed and success is not
 *      nullptr.
 *      does not throw.
 *
 * parse_string(<params>, [*out], [delim = '"'], [include_delims = false])
 *      parses a single string between delimiters [delim], optionally including the
 *      delimiters if include_delims is true and writes the parsed value to *out if
 *      out is not nullptr.
 *      throws on invalid input.
 *
 * parse_bool(<params>, [*out])
 *      parses a single boolean value regardless of case and optionally
 *      writes the parsed value to *out if out is not nullptr.
 *      throws on invalid input.
 *
 * parse_integer(<params>, [*out])
 *      parses a single integer value and optionally writes the parsed value
 *      to *out if out is not nullptr.
 *      detects bases such as 0b for binary, 0<oct digits> for octal or
 *      0x for hexadecimal (case insensitive). Base prefix may be omitted for
 *      hexadecimal values, e.g. DEADBEEF.
 *      throws on invalid input.
 *
 * parse_decimal(<params>, [*out])
 *      parses a single floating point value and optionally writes the parsed value
 *      to *out if out is not nullptr.
 *      supports exponents with e+/-X.
 *      throws on invalid input.
 *
 * parse_identifier(<params>, [*out])
 *      parses a single identifier and optionally writes the parsed value
 *      to *out if out is not nullptr.
 *      an identifier may start with a letter or _ and can contain letters,
 *      _ or digits.
 *      throws on invalid input.
 */

#pragma once

#include <assert.h>
#include <stdexcept>
#include <iosfwd>

#include "string.hpp"
#include "number_types.hpp"

// parse iterator
struct parse_iterator
{
    u32 i = 0;
    u32 line_start = 0; // character at file offset where line starts
    u32 line = 1; // line number, starting at 1
    u32 line_pos = 1; // character within line, starting at 1
};

inline void update_iterator_line_pos(parse_iterator *it)
{
    it->line_pos = (it->i - it->line_start) + 1;
}

inline void advance(parse_iterator *it, s64 n = 1)
{
    // does not advance line counter
    it->i += n;
    update_iterator_line_pos(it);
}

inline void next_line(parse_iterator *it)
{
    it->line++;
    it->line_start = it->i;
}

template<typename CharT>
inline std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &lhs, const parse_iterator &rhs)
{
    return lhs << rhs.line << ":" << rhs.line_pos;
}

// parse range
struct parse_range
{
    parse_iterator start;
    parse_iterator end;

    size_t length() const { return end.i - start.i; }
};

template<typename CharT>
inline std::basic_string<CharT> slice(const CharT *input, const parse_range *range)
{
    return std::basic_string<CharT>(input + range->start.i, range->length());
}

template<typename CharT>
inline void slice(const CharT *input, const parse_range *range, CharT *out)
{
    copy(input + range->start.i, out, range->length());
}

template<typename CharT>
inline std::basic_string<CharT> slice(const std::basic_string<CharT> &input, const parse_range *range)
{
    return slice(input.c_str(), range);
}

template<typename CharT>
inline void slice(const std::basic_string<CharT> &input, const parse_range *range, CharT *out)
{
    slice(input.c_str(), range, out);
}

template<typename CharT>
inline std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &lhs, const parse_range &rhs)
{
    return lhs << rhs.start << "-" << rhs.end;
}

// parse error
template<typename CharT = char>
class parse_error : public std::exception
{
public:
    bool success = false;
    std::string _what;

    parse_iterator it;
    const CharT *input = nullptr;
    size_t input_size;

    parse_error() {}
    parse_error(const char* msg) : _what(msg) {}
    parse_error(const parse_error& other) = default;

    parse_error(parse_iterator pit, const CharT *pinput, size_t pinput_size, const char* msg)
        : _what(msg), it(pit), input(pinput), input_size(pinput_size)
    {}

    parse_error &operator=(const parse_error& other) = default;

    // for "if error" scenarios
    operator bool() const { return !success; }

    template<typename... Ts>
    parse_error(parse_iterator pit, const CharT *pinput, size_t pinput_size, Ts &&... ts)
        : _what{str(std::forward<Ts>(ts)...)}, it(pit), input(pinput), input_size(pinput_size)
    {}

    const char* what() const noexcept override
    {
        return _what.c_str();
    }
};

#define SKIP_COND(c, it, input, input_size, COND) \
    while (COND(c)) \
    { \
        advance(&it); \
        if (it.i >= input_size) \
            break; \
        c = input[it.i]; \
    }

template<typename CharT>
parse_iterator skip_whitespace(parse_iterator it, const CharT *input, size_t input_size)
{
    if (input == nullptr || it.i >= input_size)
        return it;

    CharT c;

    while (it.i < input_size)
    {
        c = input[it.i];

        if (is_newline(c))
        {
            it.i++;
            next_line(&it);
            update_iterator_line_pos(&it);
        }
        else if (is_space(c))
        {
            advance(&it);
        }
        else
            break;
    }

    return it;
}

// sets success to true if at least one whitespace was skipped
template<typename CharT>
parse_iterator skip_whitespace(parse_iterator it, const CharT *input, size_t input_size, bool *success)
{
    if (input == nullptr || it.i >= input_size)
    {
        *success = false;
        return it;
    }

    CharT c;
    bool skipped = false;

    while (it.i < input_size)
    {
        c = input[it.i];

        if (is_newline(c))
        {
            skipped = true;
            it.i++;
            next_line(&it);
            update_iterator_line_pos(&it);
        }
        else if (is_space(c))
        {
            skipped = true;
            advance(&it);
        }
        else
            break;
    }

    *success = skipped;

    return it;
}

template<typename CharT>
parse_iterator parse_comment(parse_iterator it, const CharT *input, size_t input_size, parse_range *out, bool *success = nullptr)
{
    parse_iterator ret = it;
    parse_iterator comment_start;
    parse_iterator comment_end;

    bool has_comment = false;

    it = skip_whitespace(it, input, input_size);
    CharT c;

    if (input == nullptr || it.i >= input_size)
    {
        has_comment = false;
        goto parse_comment_end;
    }

    c = input[it.i];

    if (c != '/')
    {
        has_comment = false;
        goto parse_comment_end;
    }

    advance(&it);

    if (input == nullptr || it.i >= input_size)
    {
        has_comment = false;
        goto parse_comment_end;
    }

    c = input[it.i];

    if (c == '/')
    {
        advance(&it);

        comment_start = it;

        // line comment
        while (it.i < input_size && input[it.i] != '\n')
            advance(&it);

        if (it.i >= input_size)
        {
            comment_end = it;
            ret = it;
            has_comment = true;
            goto parse_comment_end;
        }
        else
        // if (input[it.i] == '\n')
        {
            it.i++;
            next_line(&it);
            update_iterator_line_pos(&it);
            comment_end = it;
            ret = it;
            has_comment = true;
            goto parse_comment_end;
        }
    }
    else if (c == '*')
    {
        /* block comment */ 
        advance(&it);

        comment_start = it;

        while (it.i < input_size)
        {
            it = skip_whitespace(it, input, input_size);

            if (it.i >= input_size)
                break;

            c = input[it.i];

            if (c == '*')
            {
                advance(&it);

                if (it.i >= input_size)
                    break;

                c = input[it.i];

                if (c == '/')
                {
                    comment_end = it;
                    advance(&comment_end, -1);

                    advance(&it);
                    ret = it;
                    has_comment = true;
                    goto parse_comment_end;
                }
            }

            advance(&it);
        }

        has_comment = false;
        goto parse_comment_end;
    }

parse_comment_end:
    if (success != nullptr)
        *success = has_comment;

    if (has_comment && out != nullptr)
    {
        out->start = comment_start;
        out->end = comment_end;
    }

    return ret;
}

template<typename CharT>
parse_iterator parse_comment(parse_iterator it, const CharT *input, size_t input_size, std::basic_string<CharT> *out, bool *success = nullptr)
{
    parse_range range;

    it = parse_comment(it, input, input_size, &range, success);

    if (*success)
        *out = slice(input, &range);

    return it;
}

template<typename CharT>
parse_iterator parse_comment(parse_iterator it, const CharT *input, size_t input_size, bool *success = nullptr)
{
    return parse_comment(it, input, input_size, static_cast<parse_range*>(nullptr), success);
}

template<typename CharT>
parse_iterator skip_whitespace_and_comments(parse_iterator it, const CharT *input, size_t input_size)
{
    bool ok;

    do
    {
        it = parse_comment(it, input, input_size, &ok);
        it = skip_whitespace(it, input, input_size);
    } while (ok);

    return it;
}

template<typename CharT>
parse_iterator parse_string(parse_iterator it, const CharT *input, size_t input_size, parse_range *out, parse_error<CharT> *err, CharT delim = '"', bool include_delims = false)
{
    parse_iterator start = it;

    assert(input != nullptr);
    assert(err != nullptr);
    assert(it.i < input_size);

    err->success = false;
    auto c = input[it.i];

    if (c != delim)
    {
        *err = parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", start, ", expected '", delim, "'");
        return start;
    }

    advance(&it);

    if (it.i >= input_size)
    {
        *err = parse_error(it, input, input_size, "unterminated string starting at ", start);
        return start;
    }

    c = input[it.i];

    while (c != delim)
    {
        if (is_space(c))
        {
            it = skip_whitespace(it, input, input_size);
        }
        else if (c == '\\')
        {
            advance(&it, 2);
        }
        else
            advance(&it);

        if (it.i >= input_size)
            break;

        c = input[it.i];
    }

    if (c != delim)
    {
        *err = parse_error(it, input, input_size, "unterminated string starting at ", start);
        return start;
    }

    advance(&it);

    if (out != nullptr)
    {
        if (include_delims)
        {
            out->start = start;
            out->end = it;
        }
        else
        {
            out->start = start;
            advance(&out->start, 1);
            out->end = it;
            advance(&out->end, -1);
        }
    }

    err->success = true;
    return it;
}

template<typename CharT>
parse_iterator parse_string(parse_iterator it, const CharT *input, size_t input_size, std::basic_string<CharT> *out, CharT delim = '"', bool include_delims = false)
{
    parse_range range;
    parse_error<CharT> err;
    it = parse_string(it, input, input_size, &range, &err, delim, include_delims);

    if (err)
        throw err;

    if (out != nullptr)
        *out = slice(input, &range);

    return it;
}

template<typename CharT>
parse_iterator parse_string(parse_iterator it, const CharT *input, size_t input_size, CharT delim = '"', bool include_delims = false)
{
    return parse_string(it, input, input_size, static_cast<parse_range*>(nullptr), delim, include_delims);
}

template<typename CharT>
parse_iterator parse_bool(parse_iterator it, const CharT *input, size_t input_size, parse_range *out, parse_error<CharT> *err)
{
    assert(input != nullptr);
    assert(it.i < input_size);
    assert(err != nullptr);

    err->success = false;

    parse_iterator start = it;
    auto c = input[it.i];
    bool val = true;
    
    if (to_lower(c) == 't')
    {
        if (it.i + 3 >= input_size)
        {
            advance(&it, input_size - it.i);
            *err = parse_error(it, input, input_size, "not a boolean at ", start);
            return start;
        }

        for (int i = 0; i < 4;)
        {
            c = input[it.i];
            if (to_lower(c) != "true"[i])
            {
                *err = parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in boolean");
                return start;
            }

            advance(&it);
            ++i;
        }

        val = true;
    }
    else if (to_lower(c) == 'f')
    {
        if (it.i + 4 >= input_size)
        {
            advance(&it, input_size - it.i);
            *err = parse_error(it, input, input_size, "not a boolean at ", start);
            return start;
        }

        for (int i = 0; i < 5;)
        {
            c = input[it.i];
            if (to_lower(c) != "false"[i])
            {
                *err = parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in boolean");
                return start;
            }

            advance(&it);
            ++i;
        }

        val = false;
    }
    else 
    {
        *err = parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in boolean");
        return start;
    }

    if (out != nullptr)
    {
        out->start = start;
        out->end = it;
    }

    err->success = true;

    return it;
}

template<typename CharT>
parse_iterator parse_bool(parse_iterator it, const CharT *input, size_t input_size, bool *out = nullptr)
{
    parse_range range;
    parse_error<CharT> err;
    it = parse_bool(it, input, input_size, &range, &err);

    if (err)
        throw err;

    if (out != nullptr)
    // bad check, but fine as long as parse_bool doesnt change
        *out = to_lower(input[range.start.i]) == 't';

    return it;
}

template<typename CharT>
parse_iterator parse_integer(parse_iterator it, const CharT *input, size_t input_size, parse_range *out, parse_error<CharT> *err)
{
    assert(input != nullptr);
    assert(it.i < input_size);
    assert(err != nullptr);

    err->success = false;
    parse_iterator start = it;
    auto c = input[it.i];

    if (c == '-' || c == '+')
    {
        advance(&it);

        if (it.i >= input_size)
        {
            *err = parse_error(it, input, input_size, "not an integer at ", start);
            return start;
        }

        c = input[it.i];
        
        if (!is_digit(c))
        {
            *err = parse_error(it, input, input_size, "not an integer at ", start);
            return start;
        }
    }

    if (c == '0')
    {
        advance(&it);

        if (it.i < input_size)
        {
            c = input[it.i];

            if (c == 'x' || c == 'X')
            {
                advance(&it);

                if (it.i >= input_size)
                {
                    *err = parse_error(it, input, input_size, "invalid hex integer at ", start);
                    return start;
                }

                c = input[it.i];

                if (!is_hex_digit(c))
                {
                    *err = parse_error(it, input, input_size, "invalid hex digit ", c, " at ", it, ", hex integer starting at ", start);
                    return start;
                }

                SKIP_COND(c, it, input, input_size, is_hex_digit);
            }
            else if (c == 'b' || c == 'B')
            {
                advance(&it);

                if (it.i >= input_size)
                {
                    *err = parse_error(it, input, input_size, "invalid binary integer at ", start);
                    return start;
                }

                c = input[it.i];

                if (!is_bin_digit(c))
                {
                    *err = parse_error(it, input, input_size, "invalid binary digit ", c, " at ", it, ", hex integer starting at ", start);
                    return start;
                }

                SKIP_COND(c, it, input, input_size, is_bin_digit);
            }
            else
            {
                advance(&it);

                if (it.i >= input_size)
                {
                    *err = parse_error(it, input, input_size, "invalid octal integer at ", start);
                    return start;
                }

                c = input[it.i];

                if (!is_oct_digit(c))
                {
                    *err = parse_error(it, input, input_size, "invalid octal digit ", c, " at ", it, ", hex integer starting at ", start);
                    return start;
                }
                
                SKIP_COND(c, it, input, input_size, is_oct_digit);
            }
        }
    }
    else if (is_hex_digit(c))
    {
        if (!is_digit(c))
        {
            SKIP_COND(c, it, input, input_size, is_hex_digit);
        }
        else 
        {
            SKIP_COND(c, it, input, input_size, is_digit);
        }
    }
    else
    {
        *err = parse_error(it, input, input_size, "invalid integer digit ", c, " at ", start);
        return start;
    }
        
    if (out != nullptr)
    {
        out->start = start;
        out->end = it;
    }

    err->success = true;
    return it;
}

template<typename CharT, typename OutT = s64>
parse_iterator parse_integer(parse_iterator it, const CharT *input, size_t input_size, OutT *out = nullptr)
{
    static_assert(std::is_integral_v<OutT>, "parse_integer number type argument must be an integer type");

    assert(input != nullptr);
    assert(it.i < input_size);

    parse_iterator start = it;
    parse_iterator digit_start = it;
    auto c = input[it.i];
    int base = 10;
    bool neg = false;

    if (c == '-' || c == '+')
    {
        if (c == '-')
            neg = true;

        advance(&it);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "not an integer at ", start);

        c = input[it.i];
        
        if (!is_digit(c))
            throw parse_error(it, input, input_size, "not an integer at ", start);

        digit_start = it;
    }

    if (c == '0')
    {
        advance(&it);

        if (it.i < input_size)
        {
            c = input[it.i];

            if (c == 'x' || c == 'X')
            {
                advance(&it);

                if (it.i >= input_size)
                    throw parse_error(it, input, input_size, "invalid hex integer at ", start);

                c = input[it.i];

                if (!is_hex_digit(c))
                    throw parse_error(it, input, input_size, "invalid hex digit ", c, " at ", it, ", hex integer starting at ", start);

                base = 16;
                digit_start = it;

                SKIP_COND(c, it, input, input_size, is_hex_digit);
            }
            else if (c == 'b' || c == 'B')
            {
                advance(&it);

                if (it.i >= input_size)
                    throw parse_error(it, input, input_size, "invalid binary integer at ", start);

                c = input[it.i];

                if (!is_bin_digit(c))
                    throw parse_error(it, input, input_size, "invalid binary digit ", c, " at ", it, ", hex integer starting at ", start);

                base = 2;
                digit_start = it;

                SKIP_COND(c, it, input, input_size, is_bin_digit);
            }
            else
            {
                advance(&it);

                if (it.i >= input_size)
                    throw parse_error(it, input, input_size, "invalid octal integer at ", start);

                c = input[it.i];

                if (!is_oct_digit(c))
                    throw parse_error(it, input, input_size, "invalid octal digit ", c, " at ", it, ", hex integer starting at ", start);
                
                base = 8;

                SKIP_COND(c, it, input, input_size, is_oct_digit);
            }
        }
    }
    else if (is_hex_digit(c))
    {
        if (!is_digit(c))
        {
            base = 16;

            SKIP_COND(c, it, input, input_size, is_hex_digit);
        }
        else 
        {
            base = 10;

            SKIP_COND(c, it, input, input_size, is_digit);
        }
    }
    else
        throw parse_error(it, input, input_size, "invalid integer digit ", c, " at ", start);
        
    if (out == nullptr)
        return it;

    // 64 binary digits + 0b + - + 1
    constexpr const size_t digit_size = sizeof(OutT) * 8 + 4;
    CharT buf[digit_size];
    size_t len = it.i - digit_start.i;

    if (len > digit_size - 1)
        throw parse_error(it, input, input_size, "integer too large at ", start);

    copy(input + digit_start.i, buf, len);
    buf[len] = 0;

    OutT ret = to_integer<OutT, CharT>(buf, nullptr, base);

    if (neg)
        ret = -ret;

    *out = ret;

    return it;
}

template<typename CharT>
parse_iterator parse_decimal(parse_iterator it, const CharT *input, size_t input_size, parse_range *out, parse_error<CharT> *err)
{
    assert(input != nullptr);
    assert(it.i < input_size);
    assert(err != nullptr);

    err->success = false;
    parse_iterator start = it;
    auto c = input[it.i];
    bool has_digits = false;

    if (c == '-' || c == '+')
    {
        advance(&it);

        if (it.i >= input_size)
        {
            *err = parse_error(it, input, input_size, "not a decimal number at ", start);
            return start;
        }

        c = input[it.i];
    }

    while (is_digit(c))
    {
        has_digits = true;
        advance(&it);

        if (it.i >= input_size)
            break;

        c = input[it.i];
    }

    if (it.i >= input_size)
        goto parse_decimal_end;

    c = input[it.i];

    if (c == '.')
    {
        advance(&it);

        if (it.i >= input_size)
        {
            if (has_digits)
                goto parse_decimal_end;
            else
            {
                *err = parse_error(it, input, input_size, "expected decimal number digits at ", it, ", got EOF");
                return start;
            }
        }

        c = input[it.i];

        while (is_digit(c))
        {
            has_digits = true;
            advance(&it);

            if (it.i >= input_size)
                break;

            c = input[it.i];
        }
    }

    if (c == 'e' || c == 'E')
    {
        if (!has_digits)
        {
            *err = parse_error(it, input, input_size, "unexpected symbol '", (char)c, "' at ", it, " in decimal number starting at ", start);
            return start;
        }

        advance(&it);

        if (it.i >= input_size)
        {
            *err = parse_error(it, input, input_size, "exponent at ", it, " of decimal number starting at ", start, " requires a value");
            return start;
        }

        c = input[it.i];

        if (c == '+' || c == '-')
        {
            advance(&it);

            if (it.i >= input_size)
            {
                *err = parse_error(it, input, input_size, "exponent at ", it, " of decimal number starting at ", start, " requires a value");
                return start;
            }

            c = input[it.i];
        }

        while (is_digit(c))
        {
            has_digits = true;
            advance(&it);

            if (it.i >= input_size)
                break;

            c = input[it.i];
        }
    }

    if (!has_digits)
    {
        *err = parse_error(it, input, input_size, "not a decimal number at ", start);
        return start;
    }
        
parse_decimal_end:

    if (out != nullptr)
    {
        out->start = start;
        out->end = it;
    }

    err->success = true;

    return it;
}

template<typename CharT, typename OutT = float>
parse_iterator parse_decimal(parse_iterator it, const CharT *input, size_t input_size, OutT *out = nullptr)
{
    static_assert(std::is_floating_point_v<OutT>, "parse_decimal number type argument must be a floating point number type");

    parse_range range;
    parse_error<CharT> err;
    it = parse_decimal(it, input, input_size, &range, &err);

    if (err)
        throw err;

    if (out == nullptr)
        return it;

    // ok a double can have like 1024 digits, but this will suffice for simple things.
    // just change it if you need to.
    constexpr const size_t digit_size = 128;
    CharT buf[digit_size];
    size_t len = range.length();

    if (len > digit_size - 1)
        throw parse_error(it, input, input_size, "floating point number too large at ", range.start);

    copy(input + range.start.i, buf, len);
    buf[len] = 0;

    *out = to_decimal<OutT, CharT>(buf, nullptr);

    return it;
}

template<typename CharT>
inline bool is_first_identifier_character(CharT c)
{
    return c == '_' || is_alpha(c);
}

template<typename CharT>
inline bool is_identifier_character(CharT c)
{
    return c == '_' || is_alphanum(c);
}

template<typename CharT>
parse_iterator parse_identifier(parse_iterator it, const CharT *input, size_t input_size, parse_range *out, parse_error<CharT> *err)
{
    assert(input != nullptr);
    assert(it.i < input_size);
    assert(err != nullptr);

    err->success = false;

    parse_iterator start = it;
    auto c = input[it.i];

    if (!is_first_identifier_character(c))
    {
        *err = parse_error(it, input, input_size, "invalid identifier character at ", start);
        return start;
    }

    SKIP_COND(c, it, input, input_size, is_identifier_character);

    if (out != nullptr)
    {
        out->start = start;
        out->end = it;
    }

    err->success = true;
    return it;
}

template<typename CharT>
parse_iterator parse_identifier(parse_iterator it, const CharT *input, size_t input_size, std::basic_string<CharT> *out)
{
    parse_range range;
    parse_error<CharT> err;
    it = parse_identifier(it, input, input_size, &range, &err);

    if (err)
        throw err;

    if (out != nullptr)
        *out = slice(input, &range);

    return it;
}

template<typename CharT>
parse_iterator parse_identifier(parse_iterator it, const CharT *input, size_t input_size)
{
    return parse_identifier(it, input, input_size, static_cast<parse_range*>(nullptr));
}
