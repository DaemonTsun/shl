
#pragma once

#include <stdexcept>
#include <ostream>

#include "string.hpp"
#include "number_types.hpp"

struct parse_iterator
{
    u64 i = 0;
    u64 line_start = 0; // character at file offset where line starts
    u64 line = 1; // line number, starting at 1
    u64 line_pos = 1; // character within line, starting at 1
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
    const CharT *input;
    size_t input_size;

    parse_error(const char* msg)
        : _what(msg)
    {}

    parse_error(parse_iterator pit, const CharT *pinput, size_t pinput_size, const char* msg)
        : _what(msg), it(pit), input(pinput), input_size(pinput_size)
    {}

    template<typename... Ts>
    parse_error(parse_iterator pit, const CharT *pinput, size_t pinput_size, Ts &&... ts)
        : _what{str(std::forward<Ts>(ts)...)}, it(pit), input(pinput), input_size(pinput_size)
    {}

    const char* what() const noexcept override
    {
        return _what.c_str();
    }
};

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
parse_iterator parse_string(parse_iterator it, const CharT *input, size_t input_size, std::basic_string<CharT> *out = nullptr)
{
    parse_iterator start = it;

    if (input == nullptr || it.i >= input_size)
        throw parse_error(it, input, input_size, "not a string at ", start);

    auto c = input[it.i];

    if (c != '"')
        throw parse_error(it, input, input_size, "not a string at ", start);

    advance(&it);

    if (it.i >= input_size)
        throw parse_error(it, input, input_size, "unterminated string starting at ", start);

    c = input[it.i];

    while (c != '"')
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

    if (c != '"')
        throw parse_error(it, input, input_size, "unterminated string starting at ", start);

    advance(&it);

    if (out != nullptr)
        *out = std::basic_string<CharT>(input + start.i, it.i - start.i);

    return it;
}

template<typename CharT>
parse_iterator parse_integer(parse_iterator it, const CharT *input, size_t input_size, s64 *out = nullptr)
{
    parse_iterator start = it;
    auto c = input[it.i];

    if (c == '-')
    {
        advance(&it);

        if (it.i >= input_size)
            throw parse_error(it, input, input_size, "not an integer at ", start);

        c = input[it.i];
        
        if (!is_digit(c))
            throw parse_error(it, input, input_size, "not an integer at ", start);
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

                if (!is_hex_digit(c))
                    throw parse_error(it, input, input_size, "invalid hex digit ", c, " at ", it, ", hex integer starting at ", start);

                while (is_hex_digit(c))
                {
                    advance(&it);
                    
                    if (it.i >= input_size)
                        break;

                    c = input[it.i];
                }
            }
            else
            {
                while (is_digit(c))
                {
                    advance(&it);
                    
                    if (it.i >= input_size)
                        break;

                    c = input[it.i];
                }
            }
        }
    }
    else if (is_digit(c))
    {
        while (is_digit(c))
        {
            advance(&it);
            
            if (it.i >= input_size)
                break;

            c = input[it.i];
        }
    }
    else
        throw parse_error(it, input, input_size, "invalid integer digit ", c, " at ", start);
        
    if (out != nullptr)
    {
        std::basic_string<CharT> str(input + start.i, it.i - start.i);
        *out = stoll(str, nullptr, 0);
    }

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
    parse_iterator start = it;
    auto c = input[it.i];

    if (!is_first_identifier_character(c))
        throw parse_error(it, input, input_size, "invalid identifier character at ", start);

    while (is_identifier_character(c))
    {
        advance(&it);

        if (it.i >= input_size)
            break;

        c = input[it.i];
    }

    if (out != nullptr)
        *out = std::basic_string<CharT>(input + start.i, it.i - start.i);

    return it;
}

