#pragma once

/* string.hpp

string utility header

defines const_string and const_wstring, basically const char pointers with
size attached to them. const_strings are not guaranteed to be null terminated,
but in most cases will be (e.g. string literals). can be explicitly converted
to const C *, where C is char in the case of const_string, or wchar_t in the
case of const_wstring.

to construct a const_string, either initialize with const_string{ptr, size},
use the string literal suffix _cs, or use to_const_string(...), such as:

    const_string mystr = "hello world!"_cs;
    const char *cstr = mystr.c_str;
    u64 size = mystr.size; // or string_length(mystr);

    const_string mystr2 = to_const_string("hello");

to_const_string converts from character pointers (optionally including a size),
other const_strings, or strings to a const_string.

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

last_index_of(haystack, needle[, offset]) returns the index of the last occurrence of needle
                                             [ending at offset, moving towards the beginning]
                                             within the string haystack, or
                                             -1 if needle was not found.

contains(haystack, needle) returns true if haystack contains needle, false if not.

trim_left(s)                 trims whitespaces from the left of string s, in-place
trim_right(s)                trims whitespaces from the right of string s, in-place
trim(s)                      trims whitespaces from the left and right of string s, in-place
to_upper(c/s)                converts the given character or string to upper case
to_lower(c/s)                converts the given character or string to upper case

substring(src, start[, length]) returns a slice (not a copy) of the substring of src
                                starting at start, up to length characters.

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

template<typename C, u64 N>
inline const_string_base<C> to_const_string(const C s[N])
{
    return const_string_base<C>{s, N};
}

template<typename C>
inline const_string_base<C> to_const_string(const string_base<C> *s)
{
    return const_string_base<C>{s->data, s->size};
}

template<typename C>
inline const_string_base<C> to_const_string(string_base<C> s)
{
    return const_string_base<C>{s.data, s.size};
}

template<typename C>
inline const_string_base<C> to_const_string(const_string_base<C> s)
{
    return s;
}

int _compare_strings(const_string   s1, const_string   s2, u64 n);
int _compare_strings(const_wstring  s1, const_wstring  s2, u64 n);

template<typename T1, typename T2>
auto compare_strings(T1 s1, T2 s2, u64 n = max_value(u64))
    -> decltype(_compare_strings(to_const_string(s1), to_const_string(s2), n))
{
    return _compare_strings(to_const_string(s1), to_const_string(s2), n);
}

template<> bool equals(string  s1, string  s2);
template<> bool equals(wstring s1, wstring s2);
template<> bool equals_p(const string  *s1, const string  *s2);
template<> bool equals_p(const wstring *s1, const wstring *s2);

bool _begins_with(const_string  s, const_string  prefix);
bool _begins_with(const_wstring s, const_wstring prefix);

// we need the -> decltype(...) to make sure only things that can become
// const_string_base may be passed as parameters, otherwise this
// template will catch all types.
template<typename T1, typename T2>
auto begins_with(T1 s, T2 prefix)
    -> decltype(_begins_with(to_const_string(s), to_const_string(prefix)))
{
    return _begins_with(to_const_string(s), to_const_string(prefix));
}

bool _ends_with(const_string  s, const_string  prefix);
bool _ends_with(const_wstring s, const_wstring prefix);

template<typename T1, typename T2>
auto ends_with(T1 s, T2 prefix)
    -> decltype(_ends_with(to_const_string(s), to_const_string(prefix)))
{
    return _ends_with(to_const_string(s), to_const_string(prefix));
}

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
void _copy_string(const_string   src, string  *dst, u64 n, u64 dst_offset);
void _copy_string(const_wstring  src, wstring *dst, u64 n, u64 dst_offset);

// T = anything that can become (or is) a const_string_base
template<typename T, typename C>
void copy_string(T src, string_base<C> *dst, u64 n = max_value(u64), u64 dst_offset = 0)
{
    _copy_string(to_const_string(src), dst, n, dst_offset);
}

string  _copy_string(const_string  src, u64 n);
wstring _copy_string(const_wstring src, u64 n);

// T = anything that can become (or is) a const_string_base
template<typename T>
auto copy_string(T src, u64 n = max_value(u64))
{
    return _copy_string(to_const_string(src), n);
}

void _append_string(string  *dst, const_string  other);
void _append_string(wstring *dst, const_wstring other);

// T = anything that can become (or is) a const_string_base
template<typename C, typename T>
void append_string(string_base<C> *dst, T other)
{
    _append_string(dst, to_const_string(other));
}

void _prepend_string(string  *dst, const_string  other);
void _prepend_string(wstring *dst, const_wstring other);

template<typename C, typename T>
void prepend_string(string_base<C> *dst, T other)
{
    _prepend_string(dst, to_const_string(other));
}

s64 _index_of(const_string   haystack, char           needle, s64 offset);
s64 _index_of(const_wstring  haystack, wchar_t        needle, s64 offset);
s64 _index_of(const_string   haystack, const_string   needle, s64 offset);
s64 _index_of(const_wstring  haystack, const_wstring  needle, s64 offset);

template<typename T1, typename T2>
auto index_of(T1 haystack, T2 needle, s64 offset = 0)
    -> decltype(_index_of(to_const_string(haystack), to_const_string(needle), offset))
{
    return _index_of(to_const_string(haystack), to_const_string(needle), offset);
}

template<typename T>
auto index_of(T haystack, char needle, s64 offset = 0)
    -> decltype(_index_of(to_const_string(haystack), needle, offset))
{
    return _index_of(to_const_string(haystack), needle, offset);
}

template<typename T>
auto index_of(T haystack, wchar_t needle, s64 offset = 0)
    -> decltype(_index_of(to_const_string(haystack), needle, offset))
{
    return _index_of(to_const_string(haystack), needle, offset);
}

s64 _last_index_of(const_string   haystack, char           needle, s64 offset);
s64 _last_index_of(const_wstring  haystack, wchar_t        needle, s64 offset);
s64 _last_index_of(const_string   haystack, const_string   needle, s64 offset);
s64 _last_index_of(const_wstring  haystack, const_wstring  needle, s64 offset);

template<typename T1, typename T2>
auto last_index_of(T1 haystack, T2 needle, s64 offset = max_value(s64))
    -> decltype(_last_index_of(to_const_string(haystack), to_const_string(needle), offset))
{
    return _last_index_of(to_const_string(haystack), to_const_string(needle), offset);
}

template<typename T>
auto last_index_of(T haystack, char needle, s64 offset = max_value(s64))
    -> decltype(_last_index_of(to_const_string(haystack), needle, offset))
{
    return _last_index_of(to_const_string(haystack), needle, offset);
}

template<typename T>
auto last_index_of(T haystack, wchar_t needle, s64 offset = max_value(s64))
    -> decltype(_last_index_of(to_const_string(haystack), needle, offset))
{
    return _last_index_of(to_const_string(haystack), needle, offset);
}

template<typename T1, typename T2>
auto contains(T1 haystack, T2 needle)
    -> decltype(index_of(to_const_string(haystack), needle) != -1)
{
    return index_of(to_const_string(haystack), needle) != -1;
}

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

const_string  _substring(const_string  s, u64 start, u64 length);
const_wstring _substring(const_wstring s, u64 start, u64 length);

template<typename T>
auto substring(T str, u64 start, u64 length = max_value(u64))
    -> decltype(_substring(to_const_string(str), start, length))
{
    return _substring(to_const_string(str), start, length);
}

void substring(const char    *s, u64 start, u64 length, char    *out, u64 out_offset = 0);
void substring(const wchar_t *s, u64 start, u64 length, wchar_t *out, u64 out_offset = 0);

void _substring(const_string  s, u64 start, u64 length, string *out,  u64 out_offset);
void _substring(const_wstring s, u64 start, u64 length, wstring *out, u64 out_offset);

template<typename C, typename T>
auto substring(T str, u64 start, u64 length, string_base<C> *out, u64 out_offset = 0)
    -> decltype(_substring(to_const_string(str), start, length, out, out_offset))
{
    _substring(to_const_string(str), start, length, out, out_offset);
}

void replace(string  *s, char    needle, char    replacement, s64 offset = 0);
void replace(wstring *s, wchar_t needle, wchar_t replacement, s64 offset = 0);
void _replace(string  *s, const_string  needle, const_string  replacement, s64 offset);
void _replace(wstring *s, const_wstring needle, const_wstring replacement, s64 offset);

template<typename C, typename TNeedle, typename TReplacement>
auto replace(string_base<C> *s, TNeedle needle, TReplacement replacement, s64 offset = 0)
    -> decltype(_replace(s, to_const_string(needle), to_const_string(replacement), offset))
{
    _replace(s, to_const_string(needle), to_const_string(replacement), offset);
}

void replace_all(string  *s, char    needle, char    replacement, s64 offset = 0);
void replace_all(wstring *s, wchar_t needle, wchar_t replacement, s64 offset = 0);
void _replace_all(string  *s, const_string  needle, const_string  replacement, s64 offset);
void _replace_all(wstring *s, const_wstring needle, const_wstring replacement, s64 offset);

template<typename C, typename TNeedle, typename TReplacement>
auto replace_all(string_base<C> *s, TNeedle needle, TReplacement replacement, s64 offset = 0)
    -> decltype(_replace_all(s, to_const_string(needle), to_const_string(replacement), offset))
{
    _replace_all(s, to_const_string(needle), to_const_string(replacement), offset);
}

s64 _split(const_string  s, char          delim, array<const_string>  *out);
s64 _split(const_wstring s, wchar_t       delim, array<const_wstring> *out);
s64 _split(const_string  s, const_string  delim, array<const_string>  *out);
s64 _split(const_wstring s, const_wstring delim, array<const_wstring> *out);

template<typename C, typename TStr, typename TDelim>
auto split(TStr s, TDelim delim, array<const_string_base<C>> *out)
    -> decltype(_split(to_const_string(s), to_const_string(delim), out))
{
    return _split(to_const_string(s), to_const_string(delim), out);
}

template<typename C, typename T>
auto split(T s, C delim, array<const_string_base<C>> *out)
    -> decltype(_split(to_const_string(s), delim, out))
{
    return _split(to_const_string(s), delim, out);
}

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

void join(const wchar_t **strings, u64 count, wchar_t        delim, wstring *out);
void join(const wchar_t **strings, u64 count, const wchar_t *delim, wstring *out);
void join(const wchar_t **strings, u64 count, const_wstring  delim, wstring *out);
void join(const wchar_t **strings, u64 count, const wstring *delim, wstring *out);
void join(const_wstring *strings,  u64 count, wchar_t        delim, wstring *out);
void join(const_wstring *strings,  u64 count, const wchar_t *delim, wstring *out);
void join(const_wstring *strings,  u64 count, const_wstring  delim, wstring *out);
void join(const_wstring *strings,  u64 count, const wstring *delim, wstring *out);
void join(const wstring *strings,  u64 count, wchar_t        delim, wstring *out);
void join(const wstring *strings,  u64 count, const wchar_t *delim, wstring *out);
void join(const wstring *strings,  u64 count, const_wstring  delim, wstring *out);
void join(const wstring *strings,  u64 count, const wstring *delim, wstring *out);
void join(const array<const wchar_t*>   *arr, wchar_t        delim, wstring *out);
void join(const array<const wchar_t*>   *arr, const wchar_t *delim, wstring *out);
void join(const array<const wchar_t*>   *arr, const_wstring  delim, wstring *out);
void join(const array<const wchar_t*>   *arr, const wstring *delim, wstring *out);
void join(const array<const_wstring>    *arr, wchar_t        delim, wstring *out);
void join(const array<const_wstring>    *arr, const wchar_t *delim, wstring *out);
void join(const array<const_wstring>    *arr, const_wstring  delim, wstring *out);
void join(const array<const_wstring>    *arr, const wstring *delim, wstring *out);
void join(const array<wstring>          *arr, wchar_t        delim, wstring *out);
void join(const array<wstring>          *arr, const wchar_t *delim, wstring *out);
void join(const array<wstring>          *arr, const_wstring  delim, wstring *out);
void join(const array<wstring>          *arr, const wstring *delim, wstring *out);

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
inline bool operator==(const string_base<C> &a, const string_base<C> &b)
{
    return compare_strings(a, b) == 0;
}

template<typename C>
inline bool operator!=(const string_base<C> &a, const string_base<C> &b)
{
    return compare_strings(a, b) != 0;
}
