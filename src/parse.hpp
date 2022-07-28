
#pragma once

#include <stdexcept>
#include <ostream>

#include "string.hpp"
#include "number_types.hpp"

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

// does not advance line counter
inline void advance(parse_iterator *it, s64 n = 1)
{
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

template<typename CharT = char>
class parse_error : public std::exception
{
public:
    std::string _what;

    parse_iterator it;
    const CharT *input = nullptr;
    size_t input_size;

    parse_error(const char* msg) : _what(msg) {}
    parse_error(const parse_error& other) = default;

    parse_error(parse_iterator pit, const CharT *pinput, size_t pinput_size, const char* msg)
        : _what(msg), it(pit), input(pinput), input_size(pinput_size)
    {}

    parse_error &operator=(const parse_error& other) = default;

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

template<typename CharT>
parse_iterator parse_string(parse_iterator it, const CharT *input, size_t input_size, std::basic_string<CharT> *out = nullptr, CharT delim = '"', bool include_delims = false)
{
    parse_iterator start = it;

    if (input == nullptr || it.i >= input_size)
        throw parse_error(it, input, input_size, "not a string at ", start);

    auto c = input[it.i];

    if (c != delim)
        throw parse_error(it, input, input_size, "not a string at ", start);

    advance(&it);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "unterminated string starting at ", start);

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
        throw parse_error(it, input, input_size, "unterminated string starting at ", start);

    advance(&it);

    if (out != nullptr)
    {
        if (include_delims)
            *out = std::basic_string<CharT>(input + start.i, it.i - start.i);
        else
            *out = std::basic_string<CharT>(input + start.i + 1, (it.i - start.i) - 2);
    }

    return it;
}

template<typename CharT>
parse_iterator parse_bool(parse_iterator it, const CharT *input, size_t input_size, bool *out = nullptr)
{
    if (input == nullptr || it.i >= input_size)
        throw parse_error(it, input, input_size, "not a boolean at ", it);

    parse_iterator start = it;
    auto c = input[it.i];
    bool val = true;
    
    if (to_lower(c) == 't')
    {
        if (it.i + 3 >= input_size)
        {
            advance(&it, input_size - it.i);
            throw parse_error(it, input, input_size, "not a boolean at ", start);
        }

        for (int i = 0; i < 4;)
        {
            c = input[it.i];
            if (to_lower(c) != "true"[i])
                throw parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in boolean");

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
            throw parse_error(it, input, input_size, "not a boolean at ", start);
        }

        for (int i = 0; i < 5;)
        {
            c = input[it.i];
            if (to_lower(c) != "false"[i])
                throw parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in boolean");

            advance(&it);
            ++i;
        }

        val = false;
    }
    else 
        throw parse_error(it, input, input_size, "unexpected symbol '", c, "' at ", it, " in boolean");

    if (out)
        *out = val;

    return it;
}

template<typename CharT, typename OutT = s64>
parse_iterator parse_integer(parse_iterator it, const CharT *input, size_t input_size, OutT *out = nullptr)
{
    static_assert(std::is_integral_v<OutT>, "parse_integer number type argument must be an integer type");

    if (input == nullptr || it.i >= input_size)
        throw parse_error(it, input, input_size, "not an integer at ", it);

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
    constexpr const size_t digit_size = 68;
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

template<typename CharT, typename OutT = float>
parse_iterator parse_decimal(parse_iterator it, const CharT *input, size_t input_size, OutT *out = nullptr)
{
    static_assert(std::is_floating_point_v<OutT>, "parse_decimal number type argument must be a floating point number type");

    if (input == nullptr || it.i >= input_size)
        throw parse_error(it, input, input_size, "not a decimal number at ", it);

    parse_iterator start = it;
    auto c = input[it.i];
    bool has_digits = false;

    if (c == '-' || c == '+')
    {
        advance(&it);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "not a decimal number at ", start);

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
                throw parse_error(it, input, input_size, "expected decimal number digits at ", it, ", got EOF");
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
            throw parse_error(it, input, input_size, "unexpected symbol '", (char)c, "' at ", it, " in decimal number starting at ", start);

        advance(&it);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "exponent at ", it, " of decimal number starting at ", start, " requires a value");

        c = input[it.i];

        if (c == '+' || c == '-')
        {
            advance(&it);

            if (it.i >= input_size)
                throw parse_error(it, input, input_size, "exponent at ", it, " of decimal number starting at ", start, " requires a value");

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
        throw parse_error(it, input, input_size, "not a decimal number at ", start);
        
parse_decimal_end:
    if (out == nullptr)
        return it;

    // ok a double can have like 1024 digits, but this will suffice for simple things.
    // just change it if you need to.
    constexpr const size_t digit_size = 128;
    CharT buf[digit_size];
    size_t len = it.i - start.i;

    if (len > digit_size - 1)
        throw parse_error(it, input, input_size, "floating point number too large at ", start);

    copy(input + start.i, buf, len);
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
parse_iterator parse_identifier(parse_iterator it, const CharT *input, size_t input_size, std::basic_string<CharT> *out = nullptr)
{
    if (input == nullptr
     || it.i >= input_size)
        throw parse_error(it, input, input_size, "not an identifier at", it);

    parse_iterator start = it;
    auto c = input[it.i];

    if (!is_first_identifier_character(c))
        throw parse_error(it, input, input_size, "invalid identifier character at ", start);

    SKIP_COND(c, it, input, input_size, is_identifier_character);

    if (out != nullptr)
        *out = std::basic_string<CharT>(input + start.i, it.i - start.i);

    return it;
}
