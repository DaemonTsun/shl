#pragma once

/* string.hpp
 * v1.1
 *
 * string utility header
 * is_space(c)              returns true if c is a whitespace character
 * is_newline(c)            returns true if c is a newline character
 * is_alpha(c)              returns true if c is an alphabet character
 * is_digit(c)              returns true if c is a digit
 * is_bin_digit(c)          returns true if c is a binary digit (0 or 1)
 * is_oct_digit(c)          returns true if c is an octal digit (0-7)
 * is_hex_digit(c)          returns true if c is a hexadecimal digit
 * is_alphanum(c)           returns true if is_digit(c) || is_alpha(c)
 * is_upper(c)              returns true if c is an uppercase letter
 * is_lower(c)              returns true if c is an lowercase letter
 *
 * is_blank(s)              returns true if s is an empty string or only contains
 *                          whitespaces
 *
 * string_length(s)            returns the length of the string
 * compare_strings(s1, s2)     compares two strings
 * compare_strings(s1, s2, n)  compares two strings, up to n characters
 *
 * begins_with(s, prefix)   returns true if prefix is a prefix of s
 * ends_with(s, suffix)     returns true if suffix is a suffix of s
 *
 * to_int(s)           converts the string to an int
 * to_long(s)          converts the string to a long
 * to_long_long(s)     you get the idea
 * ...
 * to_float(s)         converts the string to a float
 * ...
 *
 * copy_string(src, dest)          copies one string to another
 * copy_string(src, dest, n)       copies one string to another, up to n characters
 */

#include "shl/array.hpp"
#include "shl/number_types.hpp"

template<typename C>
struct const_string_base
{
    typedef C value_type;

    const C *c_str;
    u64 size;

    explicit operator const C *() const { return c_str; }

    C operator[](u64 i) const { return c_str[i]; }
};

typedef const_string_base<char>    const_string;
typedef const_string_base<wchar_t> const_wstring;

const_string  operator ""_cs(const char    *, u64);
const_wstring operator ""_cs(const wchar_t *, u64);

template<typename C>
struct string_base
{
    typedef C value_type;

    array<C> data;

    explicit operator const C *() const { return data.data; }

    explicit operator const_string_base<C>() const
    {
        return const_string_base<C>{data.data, data.size};
    }

    C &operator[](u64 i)       { return data.data[i]; }
    C  operator[](u64 i) const { return data.data[i]; }
};

template<typename C>
inline const_string_base<C> to_const_string(const string_base<C> *s)
{
    return const_string_base<C>{s->data.data, s->data.size};
}

typedef string_base<char>    string;
typedef string_base<wchar_t> wstring;

string  operator ""_s(const char    *, u64);
wstring operator ""_s(const wchar_t *, u64);

void init(string *str);
void init(string *str, u64 size);
void init(string *str, const char *c);
void init(string *str, const char *c, u64 size);
void init(string *str, const_string s);
void init(wstring *str);
void init(wstring *str, u64 size);
void init(wstring *str, const wchar_t *c);
void init(wstring *str, const wchar_t *c, u64 size);
void init(wstring *str, const_wstring s);

void free(string  *str);
void free(wstring *str);

// string / character functions
bool is_space(char    c);
bool is_space(wchar_t c);
bool is_newline(char    c);
bool is_newline(wchar_t c);

bool is_alpha(char    c);
bool is_alpha(wchar_t c);
bool is_digit(char    c);
bool is_digit(wchar_t c);
bool is_bin_digit(char    c);
bool is_bin_digit(wchar_t c);
bool is_oct_digit(char    c);
bool is_oct_digit(wchar_t c);
bool is_hex_digit(char    c);
bool is_hex_digit(wchar_t c);
bool is_alphanum(char    c);
bool is_alphanum(wchar_t c);

bool is_upper(char    c);
bool is_upper(wchar_t c);
bool is_lower(char    c);
bool is_lower(wchar_t c);
bool is_blank(const char    *s);
bool is_blank(const wchar_t *s);
bool is_blank(const_string  *s);
bool is_blank(const_wstring *s);
bool is_blank(const string  *s);
bool is_blank(const wstring *s);

u64 string_length(const char    *s);
u64 string_length(const wchar_t *s);
u64 string_length(const_string  s);
u64 string_length(const_wstring s);
u64 string_length(const string  *s);
u64 string_length(const wstring *s);

int compare_strings(const char *s1, const char *s2);
int compare_strings(const char *s1, const char *s2, u64 n);
int compare_strings(const wchar_t *s1, const wchar_t *s2);
int compare_strings(const wchar_t *s1, const wchar_t *s2, u64 n);
int compare_strings(const_string s1, const_string s2);
int compare_strings(const_string s1, const_string s2, u64 n);
int compare_strings(const_wstring s1, const_wstring s2);
int compare_strings(const_wstring s1, const_wstring s2, u64 n);
int compare_strings(const string *s1, const string *s2);
int compare_strings(const string *s1, const string *s2, u64 n);
int compare_strings(const wstring *s1, const wstring *s2);
int compare_strings(const wstring *s1, const wstring *s2, u64 n);

bool begins_with(const char    *s, const char    *prefix);
bool begins_with(const wchar_t *s, const wchar_t *prefix);
bool begins_with(const_string  s, const_string  prefix);
bool begins_with(const_wstring s, const_wstring prefix);
bool begins_with(const string  *s, const string  *prefix);
bool begins_with(const wstring *s, const wstring *prefix);
bool ends_with(const char    *s, const char    *suffix);
bool ends_with(const wchar_t *s, const wchar_t *suffix);
bool ends_with(const_string  s, const_string  prefix);
bool ends_with(const_wstring s, const_wstring prefix);
bool ends_with(const string  *s, const string  *prefix);
bool ends_with(const wstring *s, const wstring *prefix);

#define DEFINE_INTEGER_SIGNATURE(T, NAME) \
T NAME(const char    *s, char    **pos = nullptr, int base = 10);\
T NAME(const wchar_t *s, wchar_t **pos = nullptr, int base = 10);\
T NAME(const_string   s, char    **pos = nullptr, int base = 10);\
T NAME(const_wstring  s, wchar_t **pos = nullptr, int base = 10);\
T NAME(const string  *s, char    **pos = nullptr, int base = 10);\
T NAME(const wstring *s, wchar_t **pos = nullptr, int base = 10);

#define DEFINE_DECIMAL_SIGNATURE(T, NAME) \
T NAME(const char    *s, char    **pos = nullptr);\
T NAME(const wchar_t *s, wchar_t **pos = nullptr);\
T NAME(const_string   s, char    **pos = nullptr);\
T NAME(const_wstring  s, wchar_t **pos = nullptr);\
T NAME(const string  *s, char    **pos = nullptr);\
T NAME(const wstring *s, wchar_t **pos = nullptr);

DEFINE_INTEGER_SIGNATURE(int, to_int);
DEFINE_INTEGER_SIGNATURE(long, to_long);
DEFINE_INTEGER_SIGNATURE(long long, to_long_long);
DEFINE_INTEGER_SIGNATURE(unsigned int, to_unsigned_int);
DEFINE_INTEGER_SIGNATURE(unsigned long, to_unsigned_long);
DEFINE_INTEGER_SIGNATURE(unsigned long long, to_unsigned_long_long);
DEFINE_DECIMAL_SIGNATURE(float, to_float);
DEFINE_DECIMAL_SIGNATURE(double, to_double);
DEFINE_DECIMAL_SIGNATURE(long double, to_long_double);

#undef DEFINE_INTEGER_SIGNATURE
#undef DEFINE_DECIMAL_SIGNATURE

char    *copy_string(const char    *src, char    *dst);
wchar_t *copy_string(const wchar_t *src, wchar_t *dst);
char    *copy_string(const char    *src, char    *dst, u64 n);
wchar_t *copy_string(const wchar_t *src, wchar_t *dst, u64 n);

void copy_string(const_string   src, string  *dst);
void copy_string(const_wstring  src, string  *dst);
void copy_string(const_string   src, string  *dst, u64 n);
void copy_string(const_wstring  src, string  *dst, u64 n);
void copy_string(const string  *src, string  *dst);
void copy_string(const wstring *src, wstring *dst);
void copy_string(const string  *src, string  *dst, u64 n);
void copy_string(const wstring *src, wstring *dst, u64 n);
