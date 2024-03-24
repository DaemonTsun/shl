
/* parse.hpp
v1.1

defines the parse_iterator, the parser<C> and parse_error structs and their functions.

to parse anything, first create a parser<C> and init(p, data, size) it.

each parse_X function has the following signatures:
parse_X(parser, range, err), where range is the start and end of the parsed object
                             within the input.
parse_X(parser, X, err), where X is a pointer to the parsed object.

for example: parse_integer(parser*, int*, err*) parses an integer.

each parse_X function returns whether or not the parsing was successful, and if not,
writes the location of the error as well as an error message to *err.

there's also skip_whitespace(p) which just takes a parser and skips to the next
non-whitespace character or end of input.

parse_comment(p, range) parses a comment and writes the range of the comment within
                        the input to range and returns true on success, or returns
                        false on failure.
                        the style of the comments are C-style line comments and block
                        comments (like this one).

skip_whitespace_and_comments(p) skips whitespaces and comments. returns true if
                                anything was skipped.
 */

#pragma once

#include "shl/string.hpp"
#include "shl/error.hpp"
#include "shl/number_types.hpp"

// parse iterator
struct parse_iterator
{
    s64 pos;
    s64 line_start; // character at file offset where line starts
    s64 line; // line number, starting at 1
    s64 line_pos; // character within line, starting at 1
};

#define IT_FMT "%u:%u"
#define format_it(IT) IT.line, IT.line_pos

void init(parse_iterator *it);
void update_iterator_line_pos(parse_iterator *it);
void advance(parse_iterator *it, s64 n = 1);
void next_line(parse_iterator *it);

template<typename CharT>
struct parser_base
{
    parse_iterator it;
    const CharT *input;
    s64 input_size;
};

typedef parser_base<char>    parser;

void init(parser *p, const char *input, s64 input_size);
void init(parser *p, const_string input);
void init(parser *p, const string *input);

bool is_at_end(const parser *p);
bool is_at_end(const parser *p, s64 offset);

bool is_ok(const parser *p);

#define parser_current_char(P) ((P)->input[(P)->it.pos])

// parse range
struct parse_range
{
    parse_iterator start;
    parse_iterator end;
};

#define RANGE_FMT "%u-%u"
#define format_range(RANGE) RANGE.start.pos, RANGE.end.pos

s64 range_length(const parse_range *range);

// parse error
struct parse_error
{
    const char *what;

#ifndef NDEBUG
    const char *file;
    unsigned long line;
#endif

    parse_iterator it;
    const char *input;
    s64 input_size;
};

#ifndef NDEBUG
#define set_parse_error(ERR, Parser, MSG) \
    do { if ((ERR) != nullptr) { *(ERR) = parse_error{.what = MSG, .file = __FILE__, .line = __LINE__, .it = Parser->it, .input = Parser->input, .input_size = Parser->input_size}; } } while (0)
#else
#define set_parse_error(ERR, Parser, MSG) \
    do { if ((ERR) != nullptr) { *(ERR) = parse_error{.what = MSG, .it = Parser->it, .input = Parser->input, .input_size = Parser->input_size}; } } while (0)
#endif

#define format_parse_error(ERR, Parser, FMT, ...) \
    set_parse_error(ERR, Parser, format_error_message(FMT __VA_OPT__(,) __VA_ARGS__))

// returns true if at least one whitespace was skipped
bool skip_whitespace(parser *p);

// if a comment was parsed, returns true and writes comment range to *out
bool parse_comment(parser *p, parse_range *out);

bool skip_whitespace_and_comments(parser *p);

// if string could not be parsed, returns false and writes error to *err
bool parse_string(parser *p, parse_range *out, parse_error *err = nullptr, char delim = '"', bool include_delims = false);


#define define_parse_func(Func, TypeToParse)\
template<typename T>\
auto Func(T input, TypeToParse *out, parse_error *err = nullptr)\
    -> decltype(to_const_string(input), true)\
{\
    parser p;\
    init(&p, to_const_string(input));\
    return Func(&p, out, err);\
}

bool parse_bool(parser *p, parse_range *out, parse_error *err = nullptr);

bool parse_bool(parser *p, bool *out, parse_error *err = nullptr);
define_parse_func(parse_bool, bool)

bool parse_integer(parser *p, parse_range *out, parse_error *err = nullptr);

bool parse_integer(parser *p, int *out, parse_error *err = nullptr);
bool parse_integer(parser *p, long *out, parse_error *err = nullptr);
bool parse_integer(parser *p, long long *out, parse_error *err = nullptr);
bool parse_integer(parser *p, unsigned int *out, parse_error *err = nullptr);
bool parse_integer(parser *p, unsigned long *out, parse_error *err = nullptr);
bool parse_integer(parser *p, unsigned long long *out, parse_error *err = nullptr);

define_parse_func(parse_integer, int)
define_parse_func(parse_integer, long)
define_parse_func(parse_integer, long long)
define_parse_func(parse_integer, unsigned int)
define_parse_func(parse_integer, unsigned long)
define_parse_func(parse_integer, unsigned long long)

bool parse_decimal(parser *p, parse_range *out, parse_error *err = nullptr);

bool parse_decimal(parser *p, float *out, parse_error *err = nullptr);
bool parse_decimal(parser *p, double *out, parse_error *err = nullptr);
bool parse_decimal(parser *p, long double *out, parse_error *err = nullptr);
define_parse_func(parse_decimal, float)
define_parse_func(parse_decimal, double)
define_parse_func(parse_decimal, long double)

bool is_first_identifier_character(char c);
bool is_identifier_character(char c);

bool parse_identifier(parser *p, parse_range *out, parse_error *err = nullptr);
