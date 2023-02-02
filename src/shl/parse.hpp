
/* parse.hpp
 * v1.1
 *
 * defines the parse_iterator, the parser<C> and parse_error structs and their functions.
 *
 * to parse anything, first create a parser<C> and init(p, data, size) it.
 *
 * each parse_X function has the following signatures:
 * parse_X(parser, range, err), where range is the start and end of the parsed object
                                within the input.
 * parse_X(parser, X, err), where X is a pointer to the parsed object.
 *
 * for example: parse_integer(parser*, int*, err*) parses an integer.
 *
 * each parse_X function returns whether or not the parsing was successful, and if not,
 * writes the location of the error as well as an error message to *err.

 * there's also skip_whitespace(p) which just takes a parser and skips to the next
 * non-whitespace character or end of input.

 * parse_comment(p, range) parses a comment and writes the range of the comment within
                           the input to range and returns true on success, or returns
                           false on failure.
                           the style of the comments are C-style line comments and block
                           comments (like this one).

 * skip_whitespace_and_comments(p) skips whitespaces and comments. returns true if
                                   anything was skipped.
 */

#pragma once

#include "shl/error.hpp"
#include "shl/number_types.hpp"

// parse iterator
struct parse_iterator
{
    u64 pos;
    u64 line_start; // character at file offset where line starts
    u64 line; // line number, starting at 1
    u64 line_pos; // character within line, starting at 1
};

#define IT_FMT "%u:%u"
#define format_it(IT) IT.line, IT.line_pos

void init(parse_iterator *it);
void update_iterator_line_pos(parse_iterator *it);
// does not advance line counter
void advance(parse_iterator *it, s64 n = 1);
void next_line(parse_iterator *it);

template<typename CharT>
struct parser
{
    parse_iterator it;
    const CharT *input;
    u64 input_size;
};

void init(parser<char>    *p, const char *input, u64 input_size);
void init(parser<wchar_t> *p, const wchar_t *input, u64 input_size);

bool is_at_end(const parser<char>    *p);
bool is_at_end(const parser<wchar_t> *p);
bool is_at_end(const parser<char>    *p, s64 offset);
bool is_at_end(const parser<wchar_t> *p, s64 offset);

bool is_ok(const parser<char>    *p);
bool is_ok(const parser<wchar_t> *p);

char current_char(const parser<char>    *p);
wchar_t current_char(const parser<wchar_t> *p);

// parse range
struct parse_range
{
    parse_iterator start;
    parse_iterator end;
};

#define RANGE_FMT "%u-%u"
#define format_range(RANGE) RANGE.start.pos, RANGE.end.pos

u64 range_length(const parse_range *range);

// parse error
template<typename CharT>
struct parse_error : public error
{
    parse_iterator it;
    const CharT *input;
    u64 input_size;
};

// returns true if at least one whitespace was skipped
bool skip_whitespace(parser<char>    *p);
bool skip_whitespace(parser<wchar_t> *p);

// if a comment was parsed, returns true and writes comment range to *out
bool parse_comment(parser<char>    *p, parse_range *out);
bool parse_comment(parser<wchar_t> *p, parse_range *out);

bool skip_whitespace_and_comments(parser<char>    *p);
bool skip_whitespace_and_comments(parser<wchar_t> *p);

// if string could not be parsed, returns null and writes error to *err
bool parse_string(parser<char>    *p, parse_range *out, parse_error<char>    *err, char delim = '"', bool include_delims = false);
bool parse_string(parser<wchar_t> *p, parse_range *out, parse_error<wchar_t> *err, wchar_t delim = L'"', bool include_delims = false);

bool parse_bool(parser<char>    *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_bool(parser<wchar_t> *p, parse_range *out, parse_error<wchar_t> *err = nullptr);

bool parse_bool(parser<char>    *p, bool *out, parse_error<char>    *err = nullptr);
bool parse_bool(parser<wchar_t> *p, bool *out, parse_error<wchar_t> *err = nullptr);

bool parse_integer(parser<char>    *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, parse_range *out, parse_error<wchar_t> *err = nullptr);

bool parse_integer(parser<char>    *p, int *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser<char>    *p, long *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser<char>    *p, long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, long long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser<char>    *p, unsigned int *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, unsigned int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser<char>    *p, unsigned long *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, unsigned long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser<char>    *p, unsigned long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(parser<wchar_t> *p, unsigned long long *out, parse_error<wchar_t> *err = nullptr);

bool parse_decimal(parser<char>    *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_decimal(parser<wchar_t> *p, parse_range *out, parse_error<wchar_t> *err = nullptr);

bool parse_decimal(parser<char>    *p, float *out, parse_error<char>    *err = nullptr);
bool parse_decimal(parser<wchar_t> *p, float *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(parser<char>    *p, double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(parser<wchar_t> *p, double *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(parser<char>    *p, long double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(parser<wchar_t> *p, long double *out, parse_error<wchar_t> *err = nullptr);

bool is_first_identifier_character(char c);
bool is_first_identifier_character(wchar_t c);
bool is_identifier_character(char c);
bool is_identifier_character(wchar_t c);

bool parse_identifier(parser<char>    *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_identifier(parser<wchar_t> *p, parse_range *out, parse_error<wchar_t> *err = nullptr);
