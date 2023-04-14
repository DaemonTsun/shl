
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
struct parser_base
{
    parse_iterator it;
    const CharT *input;
    u64 input_size;
};

typedef parser_base<char>    parser;
typedef parser_base<wchar_t> wparser;

void init(parser  *p, const char *input, u64 input_size);
void init(wparser *p, const wchar_t *input, u64 input_size);
void init(parser  *p, const_string   input);
void init(wparser *p, const_wstring  input);
void init(parser  *p, const string  *input);
void init(wparser *p, const wstring *input);

bool is_at_end(const parser  *p);
bool is_at_end(const wparser *p);
bool is_at_end(const parser  *p, s64 offset);
bool is_at_end(const wparser *p, s64 offset);

bool is_ok(const parser  *p);
bool is_ok(const wparser *p);

char current_char(const parser  *p);
wchar_t current_char(const wparser *p);

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
bool skip_whitespace(parser  *p);
bool skip_whitespace(wparser *p);

// if a comment was parsed, returns true and writes comment range to *out
bool parse_comment(parser  *p, parse_range *out);
bool parse_comment(wparser *p, parse_range *out);

bool skip_whitespace_and_comments(parser  *p);
bool skip_whitespace_and_comments(wparser *p);

// if string could not be parsed, returns false and writes error to *err
bool parse_string(parser  *p, parse_range *out, parse_error<char>    *err = nullptr, char delim = '"', bool include_delims = false);
bool parse_string(wparser *p, parse_range *out, parse_error<wchar_t> *err = nullptr, wchar_t delim = L'"', bool include_delims = false);

bool parse_bool(parser  *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_bool(wparser *p, parse_range *out, parse_error<wchar_t> *err = nullptr);

bool parse_bool(parser  *p, bool *out, parse_error<char>    *err = nullptr);
bool parse_bool(wparser *p, bool *out, parse_error<wchar_t> *err = nullptr);
bool parse_bool(const_string   input, bool *out, parse_error<char>    *err = nullptr);
bool parse_bool(const_wstring  input, bool *out, parse_error<wchar_t> *err = nullptr);
bool parse_bool(const string  *input, bool *out, parse_error<char>    *err = nullptr);
bool parse_bool(const wstring *input, bool *out, parse_error<wchar_t> *err = nullptr);

bool parse_integer(parser  *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, parse_range *out, parse_error<wchar_t> *err = nullptr);

bool parse_integer(parser  *p, int *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const_string   input, int *out, parse_error<char>    *err = nullptr);
bool parse_integer(const_wstring  input, int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const string  *input, int *out, parse_error<char>    *err = nullptr);
bool parse_integer(const wstring *input, int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser  *p, long *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const_string   input, long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const_wstring  input, long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const string  *input, long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const wstring *input, long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser  *p, long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, long long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const_string   input, long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const_wstring  input, long long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const string  *input, long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const wstring *input, long long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser  *p, unsigned int *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, unsigned int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const_string   input, unsigned int *out, parse_error<char>    *err = nullptr);
bool parse_integer(const_wstring  input, unsigned int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const string  *input, unsigned int *out, parse_error<char>    *err = nullptr);
bool parse_integer(const wstring *input, unsigned int *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser  *p, unsigned long *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, unsigned long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const_string   input, unsigned long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const_wstring  input, unsigned long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const string  *input, unsigned long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const wstring *input, unsigned long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(parser  *p, unsigned long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(wparser *p, unsigned long long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const_string   input, unsigned long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const_wstring  input, unsigned long long *out, parse_error<wchar_t> *err = nullptr);
bool parse_integer(const string  *input, unsigned long long *out, parse_error<char>    *err = nullptr);
bool parse_integer(const wstring *input, unsigned long long *out, parse_error<wchar_t> *err = nullptr);

bool parse_decimal(parser  *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_decimal(wparser *p, parse_range *out, parse_error<wchar_t> *err = nullptr);

bool parse_decimal(parser  *p, float *out, parse_error<char>    *err = nullptr);
bool parse_decimal(wparser *p, float *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(const_string   input, float *out, parse_error<char>    *err = nullptr);
bool parse_decimal(const_wstring  input, float *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(const string  *input, float *out, parse_error<char>    *err = nullptr);
bool parse_decimal(const wstring *input, float *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(parser  *p, double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(wparser *p, double *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(const_string   input, double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(const_wstring  input, double *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(const string  *input, double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(const wstring *input, double *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(parser  *p, long double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(wparser *p, long double *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(const_string   input, long double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(const_wstring  input, long double *out, parse_error<wchar_t> *err = nullptr);
bool parse_decimal(const string  *input, long double *out, parse_error<char>    *err = nullptr);
bool parse_decimal(const wstring *input, long double *out, parse_error<wchar_t> *err = nullptr);

bool is_first_identifier_character(char c);
bool is_first_identifier_character(wchar_t c);
bool is_identifier_character(char c);
bool is_identifier_character(wchar_t c);

bool parse_identifier(parser  *p, parse_range *out, parse_error<char>    *err = nullptr);
bool parse_identifier(wparser *p, parse_range *out, parse_error<wchar_t> *err = nullptr);
