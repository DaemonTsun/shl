#pragma once

/* string.hpp

string utility header

defines const_string and const_wstring, basically const char pointers with
size attached to them. const_strings are not guaranteed to be null terminated,
but in most cases will be (e.g. string literals). can be explicitly converted
to const C *, where C is char in the case of const_string, or wchar_t in the
case of const_wstring.

to construct a const_string, either initialize with const_string{ptr, size} or
use the string literal suffix _cs, such as:

     const_string mystr = "hello world!"_cs;
     const char *cstr = mystr.c_str;
     u64 size = mystr.size; // or string_length(mystr);

since const_strings are relatively lightweight, they can be copied and don't
need to be referenced (they're essentially just a pointer with a size).
const_strings also don't own the string they point to, and as such don't
need to be freed.


string and wstring are also defined here. basically dynamic character arrays
with common string functions. always guaranteed to be null terminated (unless
you explicitly change that). can be implicitly converted to const_string.
can be explicitly converted to const C *, where C is char in the case of
string, and wchar_t in the case of wstring.

to construct a string, either use init(&str, ...) or use the _s suffix for
string literals, such as:

     string mystr = "hello world!"_s;                    // copy of string literal
     const char *cstr = static_cast<const char*>(mystr); // or mystr.data;
     u64 size = string_length(&mystr);
     free(&mystr);                                       // every string must be freed.


string/character functions

c = character, s = string, n = number

init(s, ...)         initializes a string
string_reserve(s, N) allocates enough memory to store at least N+1 characters
clear(s)             sets string size to 0, but does not deallocate anything
free(s)              frees the string object, deallocating memory

is_space(c)              true if c is a whitespace character
is_newline(c)            true if c is a newline character
is_alpha(c)              true if c is an alphabet character
is_digit(c)              true if c is a digit
is_bin_digit(c)          true if c is a binary digit (0 or 1)
is_oct_digit(c)          true if c is an octal digit (0-7)
is_hex_digit(c)          true if c is a hexadecimal digit
is_alphanum(c)           true if is_digit(c) || is_alpha(c)
is_upper(c)              true if c is an uppercase letter
is_lower(c)              true if c is an lowercase letter

is_empty(s)              true if s an empty string, but not nullptr
is_null_or_empty(s)      true if s is nullptr or an empty string
is_blank(s)              true if s is nullptr, an empty string or only contains
                         whitespaces

string_length(s)            returns the length of the string
compare_strings(s1, s2)     compares two strings
compare_strings(s1, s2, n)  compares two strings, up to n characters

begins_with(s, prefix)   returns true if prefix is a prefix of s
ends_with(s, suffix)     returns true if suffix is a suffix of s

to_int(s)           converts the string to an int
to_long(s)          converts the string to a long
to_long_long(s)     you get the idea
...
to_float(s)         converts the string to a float
...

set_string(dest, src)      sets dest to a copy of src

copy_string(src, dest)           copies one string to another
copy_string(src, dest, n)        copies one string to another, up to n characters
copy_string(src, dest, n, off)   copies one string to another, up to n characters, starting in
                                 dest at offset off
copy_string(src)    copies string src and returns a new string instance of the copied string.
copy_string(src, n) copies string src, up to n characters,
                    and returns a new string instance of the copied string.

append_string(dest, other)  appends the string other to the string dest
prepend_string(dest, other) prepends the string other to the string dest

index_of(haystack, needle[, offset]) returns the index of the first occurrence of needle
                                     [starting at offset] within the string haystack, or
                                     -1 if needle was not found.


trim_left(s)                 trims whitespaces from the left of string s, in-place
trim_right(s)                trims whitespaces from the right of string s, in-place
trim(s)                      trims whitespaces from the left and right of string s, in-place
to_upper(c/s)                converts the given character or string to upper case
to_lower(c/s)                converts the given character or string to upper case

substring(const_string src, u64 start, u64 length, string out, u64 out_start)
         copies src, starting at start, for up to length characters, into out,
         starting at out_start.
         allocates more memory in out if out does not have enough to fit
         out_start + length characters.

replace(str, needle, replacement[, offset]) replaces the first occurrence of needle
                                            with replacement [starting at offset] in
                                            str.

replace_all(str, needle, replacement[, offset]) replaces all occurrences of needle
                                                with replacement [starting at offset] in
                                                str.

split(str, delim, out_arr) splits the string str by string or character delimiter delim
                           and stores const_strings (pointing to memory inside str) in
                           the array out_arr. resets the size of out_arr.

join(strings*, N, delim, out) joins strings together, separated by delim, and writes
                              the output to out. strings is a pointer to N
                              strings (const char**, const_string*, string*, etc.).
join(string array, delim, out) same thing as above, except strings is a pointer to an
                               array of strings.

hash(str) returns a 32 bit hash of the string.
*/

#include "shl/hash.hpp"
#include "shl/array.hpp"
#include "shl/compare.hpp"
#include "shl/number_types.hpp"

template<typename C>
struct const_string_base
{
    typedef C value_type;

    const C *c_str;
    u64 size;

    operator bool() const { return c_str != nullptr; }
    explicit operator const C *() const { return c_str; }

    C operator[](u64 i) const { return c_str[i]; }
};

typedef const_string_base<char>    const_string;
typedef const_string_base<wchar_t> const_wstring;

const_string  operator ""_cs(const char    *, u64);
const_wstring operator ""_cs(const wchar_t *, u64);

template<typename C>
inline bool operator==(const_string_base<C> a, const_string_base<C> b)
{
    return compare_strings(a, b) == 0;
}

template<typename C>
inline bool operator!=(const_string_base<C> a, const_string_base<C> b)
{
    return compare_strings(a, b) != 0;
}

template<typename C>
struct string_base
{
    typedef C value_type;

    C *data;
    u64 size;
    u64 reserved_size;

    operator bool() const { return data != nullptr; }
    explicit operator const C *() const { return data; }

    operator const_string_base<C>() const
    {
        return const_string_base<C>{data, size};
    }

    C &operator[](u64 i)       { return data[i]; }
    C  operator[](u64 i) const { return data[i]; }
};

typedef string_base<char>    string;
typedef string_base<wchar_t> wstring;

// these allocate memory
string  operator ""_s(const char    *, u64);
wstring operator ""_s(const wchar_t *, u64);

template<typename C>
inline bool operator==(const string_base<C> &a, const string_base<C> &b)
{
    return compare_strings(a, b) == 0;
}

template<typename C>
inline bool operator!=(const string_base<C> &a, const string_base<C> &b)
{
    return compare_strings(a, b) != 0;
}

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

bool string_reserve(string  *s, u64 total_size);
bool string_reserve(wstring *s, u64 total_size);

void clear(string  *str);
void clear(wstring *str);
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
bool is_empty(const char    *s);
bool is_empty(const wchar_t *s);
bool is_empty(const_string  s);
bool is_empty(const_wstring s);
bool is_empty(const string  *s);
bool is_empty(const wstring *s);
bool is_null_or_empty(const char    *s);
bool is_null_or_empty(const wchar_t *s);
bool is_null_or_empty(const_string  s);
bool is_null_or_empty(const_wstring s);
bool is_null_or_empty(const string  *s);
bool is_null_or_empty(const wstring *s);
bool is_blank(const char    *s);
bool is_blank(const wchar_t *s);
bool is_blank(const_string  s);
bool is_blank(const_wstring s);
bool is_blank(const string  *s);
bool is_blank(const wstring *s);

u64 string_length(const char    *s);
u64 string_length(const wchar_t *s);
u64 string_length(const_string  s);
u64 string_length(const_wstring s);
u64 string_length(const string  *s);
u64 string_length(const wstring *s);

int compare_strings(const char    *s1, const char    *s2);
int compare_strings(const char    *s1, const_string   s2);
int compare_strings(const char    *s1, const string  *s2);
int compare_strings(const_string   s1, const char    *s2);
int compare_strings(const_string   s1, const_string   s2);
int compare_strings(const_string   s1, const string  *s2);
int compare_strings(const string  *s1, const char    *s2);
int compare_strings(const string  *s1, const_string   s2);
int compare_strings(const string  *s1, const string  *s2);
int compare_strings(const char    *s1, const char    *s2, u64 n);
int compare_strings(const char    *s1, const_string   s2, u64 n);
int compare_strings(const char    *s1, const string  *s2, u64 n);
int compare_strings(const_string   s1, const char    *s2, u64 n);
int compare_strings(const_string   s1, const_string   s2, u64 n);
int compare_strings(const_string   s1, const string  *s2, u64 n);
int compare_strings(const string  *s1, const char    *s2, u64 n);
int compare_strings(const string  *s1, const_string   s2, u64 n);
int compare_strings(const string  *s1, const string  *s2, u64 n);
int compare_strings(const wchar_t *s1, const wchar_t *s2);
int compare_strings(const wchar_t *s1, const_wstring  s2);
int compare_strings(const wchar_t *s1, const wstring *s2);
int compare_strings(const_wstring  s1, const wchar_t *s2);
int compare_strings(const_wstring  s1, const_wstring  s2);
int compare_strings(const_wstring  s1, const wstring *s2);
int compare_strings(const wstring *s1, const wchar_t *s2);
int compare_strings(const wstring *s1, const_wstring  s2);
int compare_strings(const wstring *s1, const wstring *s2);
int compare_strings(const wchar_t *s1, const wchar_t *s2, u64 n);
int compare_strings(const wchar_t *s1, const_wstring  s2, u64 n);
int compare_strings(const wchar_t *s1, const wstring *s2, u64 n);
int compare_strings(const_wstring  s1, const wchar_t *s2, u64 n);
int compare_strings(const_wstring  s1, const_wstring  s2, u64 n);
int compare_strings(const_wstring  s1, const wstring *s2, u64 n);
int compare_strings(const wstring *s1, const wchar_t *s2, u64 n);
int compare_strings(const wstring *s1, const_wstring  s2, u64 n);
int compare_strings(const wstring *s1, const wstring *s2, u64 n);

template<> bool equals(string  s1, string  s2);
template<> bool equals(wstring s1, wstring s2);
template<> bool equals_p(const string  *s1, const string  *s2);
template<> bool equals_p(const wstring *s1, const wstring *s2);

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

// string manipulation

// sets dst to src, discarding all previous data in dst
void set_string(string  *dst, const char    *src);
void set_string(wstring *dst, const wchar_t *src);
void set_string(string  *dst, const char    *src, u64 n);
void set_string(wstring *dst, const wchar_t *src, u64 n);
void set_string(string  *dst, const_string   src);
void set_string(wstring *dst, const_wstring  src);
void set_string(string  *dst, const string  *src);
void set_string(wstring *dst, const wstring *src);

char    *copy_string(const char    *src, char    *dst);
wchar_t *copy_string(const wchar_t *src, wchar_t *dst);
char    *copy_string(const char    *src, char    *dst, u64 n);
wchar_t *copy_string(const wchar_t *src, wchar_t *dst, u64 n);

// allocates more memory in dst if dst is not large enough to store src
void copy_string(const char    *src, string  *dst);
void copy_string(const wchar_t *src, wstring *dst);
void copy_string(const char    *src, string  *dst, u64 n);
void copy_string(const wchar_t *src, wstring *dst, u64 n);
void copy_string(const char    *src, string  *dst, u64 n, u64 dst_offset);
void copy_string(const wchar_t *src, wstring *dst, u64 n, u64 dst_offset);
void copy_string(const_string   src, string  *dst);
void copy_string(const_wstring  src, wstring *dst);
void copy_string(const_string   src, string  *dst, u64 n);
void copy_string(const_wstring  src, wstring *dst, u64 n);
void copy_string(const_string   src, string  *dst, u64 n, u64 dst_offset);
void copy_string(const_wstring  src, wstring *dst, u64 n, u64 dst_offset);
void copy_string(const string  *src, string  *dst);
void copy_string(const wstring *src, wstring *dst);
void copy_string(const string  *src, string  *dst, u64 n);
void copy_string(const wstring *src, wstring *dst, u64 n);
void copy_string(const string  *src, string  *dst, u64 n, u64 dst_offset);
void copy_string(const wstring *src, wstring *dst, u64 n, u64 dst_offset);

string copy_string(const char   *src);
string copy_string(const char   *src, u64 n);
string copy_string(const_string  src);
string copy_string(const_string  src, u64 n);
string copy_string(const string *src);
string copy_string(const string *src, u64 n);
wstring copy_string(const wchar_t *src);
wstring copy_string(const wchar_t *src, u64 n);
wstring copy_string(const_wstring  src);
wstring copy_string(const_wstring  src, u64 n);
wstring copy_string(const wstring *src);
wstring copy_string(const wstring *src, u64 n);

void append_string(string  *dst, const char    *other);
void append_string(wstring *dst, const wchar_t *other);
void append_string(string  *dst, const_string  other);
void append_string(wstring *dst, const_wstring other);
void append_string(string  *dst, const string  *other);
void append_string(wstring *dst, const wstring *other);
void prepend_string(string  *dst, const char    *other);
void prepend_string(wstring *dst, const wchar_t *other);
void prepend_string(string  *dst, const_string  other);
void prepend_string(wstring *dst, const_wstring other);
void prepend_string(string  *dst, const string  *other);
void prepend_string(wstring *dst, const wstring *other);

s64 index_of(const_string   haystack, char           needle);
s64 index_of(const_wstring  haystack, wchar_t        needle);
s64 index_of(const_string   haystack, const char    *needle);
s64 index_of(const_wstring  haystack, const wchar_t *needle);
s64 index_of(const_string   haystack, const_string   needle);
s64 index_of(const_wstring  haystack, const_wstring  needle);
s64 index_of(const_string   haystack, const string  *needle);
s64 index_of(const_wstring  haystack, const wstring *needle);
s64 index_of(const string  *haystack, char           needle);
s64 index_of(const wstring *haystack, wchar_t        needle);
s64 index_of(const string  *haystack, const char    *needle);
s64 index_of(const wstring *haystack, const wchar_t *needle);
s64 index_of(const string  *haystack, const_string   needle);
s64 index_of(const wstring *haystack, const_wstring  needle);
s64 index_of(const string  *haystack, const string  *needle);
s64 index_of(const wstring *haystack, const wstring *needle);
s64 index_of(const_string   haystack, char           needle, s64 offset);
s64 index_of(const_wstring  haystack, wchar_t        needle, s64 offset);
s64 index_of(const_string   haystack, const char    *needle, s64 offset);
s64 index_of(const_wstring  haystack, const wchar_t *needle, s64 offset);
s64 index_of(const_string   haystack, const_string   needle, s64 offset);
s64 index_of(const_wstring  haystack, const_wstring  needle, s64 offset);
s64 index_of(const_string   haystack, const string  *needle, s64 offset);
s64 index_of(const_wstring  haystack, const wstring *needle, s64 offset);
s64 index_of(const string  *haystack, char           needle, s64 offset);
s64 index_of(const wstring *haystack, wchar_t        needle, s64 offset);
s64 index_of(const string  *haystack, const char    *needle, s64 offset);
s64 index_of(const wstring *haystack, const wchar_t *needle, s64 offset);
s64 index_of(const string  *haystack, const_string   needle, s64 offset);
s64 index_of(const wstring *haystack, const_wstring  needle, s64 offset);
s64 index_of(const string  *haystack, const string  *needle, s64 offset);
s64 index_of(const wstring *haystack, const wstring *needle, s64 offset);

void trim_left(string  *s);
void trim_left(wstring *s);
void trim_right(string  *s);
void trim_right(wstring *s);
void trim(string  *s);
void trim(wstring *s);

char    to_upper(char    c);
wchar_t to_upper(wchar_t c);
void    to_upper(char    *s);
void    to_upper(wchar_t *s);
void    to_upper(string  *s);
void    to_upper(wstring *s);

char    to_lower(char    c);
wchar_t to_lower(wchar_t c);
void    to_lower(char    *s);
void    to_lower(wchar_t *s);
void    to_lower(string  *s);
void    to_lower(wstring *s);

void substring(const char    *s, u64 start, u64 length, char    *out);
void substring(const wchar_t *s, u64 start, u64 length, wchar_t *out);
void substring(const char    *s, u64 start, u64 length, string  *out);
void substring(const wchar_t *s, u64 start, u64 length, wstring *out);
void substring(const_string   s, u64 start, u64 length, string  *out);
void substring(const_wstring  s, u64 start, u64 length, wstring *out);
void substring(const string  *s, u64 start, u64 length, string  *out);
void substring(const wstring *s, u64 start, u64 length, wstring *out);
void substring(const char    *s, u64 start, u64 length, char    *out, u64 out_offset);
void substring(const char    *s, u64 start, u64 length, string  *out, u64 out_offset);
void substring(const wchar_t *s, u64 start, u64 length, wchar_t *out, u64 out_offset);
void substring(const wchar_t *s, u64 start, u64 length, wstring *out, u64 out_offset);
void substring(const_string   s, u64 start, u64 length, string  *out, u64 out_offset);
void substring(const_wstring  s, u64 start, u64 length, wstring *out, u64 out_offset);
void substring(const string  *s, u64 start, u64 length, string  *out, u64 out_offset);
void substring(const wstring *s, u64 start, u64 length, wstring *out, u64 out_offset);

void replace(string  *s, char           needle, char          replacement);
void replace(string  *s, const char    *needle, const_string  replacement);
void replace(string  *s, const_string   needle, const_string  replacement);
void replace(string  *s, const string  *needle, const_string  replacement);
void replace(string  *s, char           needle, char          replacement, s64 offset);
void replace(string  *s, const char    *needle, const_string  replacement, s64 offset);
void replace(string  *s, const_string   needle, const_string  replacement, s64 offset);
void replace(string  *s, const string  *needle, const_string  replacement, s64 offset);
void replace(wstring *s, wchar_t        needle, wchar_t       replacement);
void replace(wstring *s, const wchar_t *needle, const_wstring replacement);
void replace(wstring *s, const_wstring  needle, const_wstring replacement);
void replace(wstring *s, const wstring *needle, const_wstring replacement);
void replace(wstring *s, wchar_t        needle, wchar_t       replacement, s64 offset);
void replace(wstring *s, const wchar_t *needle, const_wstring replacement, s64 offset);
void replace(wstring *s, const_wstring  needle, const_wstring replacement, s64 offset);
void replace(wstring *s, const wstring *needle, const_wstring replacement, s64 offset);

void replace_all(string  *s, char           needle, char          replacement);
void replace_all(string  *s, const char    *needle, const_string  replacement);
void replace_all(string  *s, const_string   needle, const_string  replacement);
void replace_all(string  *s, const string  *needle, const_string  replacement);
void replace_all(string  *s, char           needle, char          replacement, s64 offset);
void replace_all(string  *s, const char    *needle, const_string  replacement, s64 offset);
void replace_all(string  *s, const_string   needle, const_string  replacement, s64 offset);
void replace_all(string  *s, const string  *needle, const_string  replacement, s64 offset);
void replace_all(wstring *s, wchar_t        needle, wchar_t       replacement);
void replace_all(wstring *s, const wchar_t *needle, const_wstring replacement);
void replace_all(wstring *s, const_wstring  needle, const_wstring replacement);
void replace_all(wstring *s, const wstring *needle, const_wstring replacement);
void replace_all(wstring *s, wchar_t        needle, wchar_t       replacement, s64 offset);
void replace_all(wstring *s, const wchar_t *needle, const_wstring replacement, s64 offset);
void replace_all(wstring *s, const_wstring  needle, const_wstring replacement, s64 offset);
void replace_all(wstring *s, const wstring *needle, const_wstring replacement, s64 offset);

s64 split(const_string   s, char           delim, array<const_string>  *out);
s64 split(const_string   s, const char    *delim, array<const_string>  *out);
s64 split(const_string   s, const_string   delim, array<const_string>  *out);
s64 split(const_string   s, const string  *delim, array<const_string>  *out);
s64 split(const string  *s, char           delim, array<const_string>  *out);
s64 split(const string  *s, const char    *delim, array<const_string>  *out);
s64 split(const string  *s, const_string   delim, array<const_string>  *out);
s64 split(const string  *s, const string  *delim, array<const_string>  *out);
s64 split(const_wstring  s, wchar_t        delim, array<const_wstring> *out);
s64 split(const_wstring  s, const wchar_t *delim, array<const_wstring> *out);
s64 split(const_wstring  s, const_wstring  delim, array<const_wstring> *out);
s64 split(const_wstring  s, const wstring *delim, array<const_wstring> *out);
s64 split(const wstring *s, wchar_t        delim, array<const_wstring> *out);
s64 split(const wstring *s, const wchar_t *delim, array<const_wstring> *out);
s64 split(const wstring *s, const_wstring  delim, array<const_wstring> *out);
s64 split(const wstring *s, const wstring *delim, array<const_wstring> *out);

void join(const char  **strings, u64 count, char          delim, string *out);
void join(const char  **strings, u64 count, const char   *delim, string *out);
void join(const char  **strings, u64 count, const_string  delim, string *out);
void join(const char  **strings, u64 count, const string *delim, string *out);
void join(const_string *strings, u64 count, char          delim, string *out);
void join(const_string *strings, u64 count, const char   *delim, string *out);
void join(const_string *strings, u64 count, const_string  delim, string *out);
void join(const_string *strings, u64 count, const string *delim, string *out);
void join(const string *strings, u64 count, char          delim, string *out);
void join(const string *strings, u64 count, const char   *delim, string *out);
void join(const string *strings, u64 count, const_string  delim, string *out);
void join(const string *strings, u64 count, const string *delim, string *out);
void join(const array<const char*>    *arr, char          delim, string *out);
void join(const array<const char*>    *arr, const char   *delim, string *out);
void join(const array<const char*>    *arr, const_string  delim, string *out);
void join(const array<const char*>    *arr, const string *delim, string *out);
void join(const array<const_string>   *arr, char          delim, string *out);
void join(const array<const_string>   *arr, const char   *delim, string *out);
void join(const array<const_string>   *arr, const_string  delim, string *out);
void join(const array<const_string>   *arr, const string *delim, string *out);
void join(const array<string>         *arr, char          delim, string *out);
void join(const array<string>         *arr, const char   *delim, string *out);
void join(const array<string>         *arr, const_string  delim, string *out);
void join(const array<string>         *arr, const string *delim, string *out);

// TODO: join wstring

/*
these are not the same as hash(const char *c) because hash(const char *c)
calculates the size. every time.
Also hash(const char *c) stops at the first null character because
strlen is used to determine the size, whereas these hash functions
hash the entire size of the string, including all null characters
(except the null character past-the-end).
*/
hash_t hash(const_string  str);
hash_t hash(const_wstring str);
hash_t hash(const const_string  *str);
hash_t hash(const const_wstring *str);
hash_t hash(const string  *str);
hash_t hash(const wstring *str);

template<typename C>
inline const_string_base<C> to_const_string(const C *s)
{
    return const_string_base<C>{s, string_length(s)};
}

template<typename C>
inline const_string_base<C> to_const_string(const C *s, u64 n)
{
    return const_string_base<C>{s, n};
}

template<typename C>
inline const_string_base<C> to_const_string(const string_base<C> *s)
{
    return const_string_base<C>{s->data, s->size};
}
