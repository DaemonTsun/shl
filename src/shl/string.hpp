#pragma once

/* string.hpp

string utility header

TODO: new docs
defines const_string and const_wstring, basically const char pointers with
size attached to them. const_strings are not guaranteed to be null terminated,
but in most cases will be (e.g. string literals). can be explicitly converted
to const C *, where C is char in the case of const_string, or wchar_t in the
case of const_wstring.

to construct a const_string, either initialize with const_string{ptr, size},
use the string literal suffix _cs, or use to_const_string(...), such as:

    const_string mystr = "hello world!"_cs;
    const char *cstr = mystr.c_str;
    s64 size = mystr.size; // or string_length(mystr);

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
     s64 size = string_length(&mystr);
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

string_is_empty(s)              true if s an empty string, but not nullptr
string_is_null_or_empty(s)      true if s is nullptr or an empty string
string_is_blank(s)              true if s is nullptr, an empty string or only contains
                         whitespaces

string_length(s)            returns the length of the string
string_compare(s1, s2)     compares two strings
string_compare(s1, s2, n)  compares two strings, up to n characters

string_begins_with(s, prefix)   returns true if prefix is a prefix of s
string_ends_with(s, suffix)     returns true if suffix is a suffix of s

to_int(s)           converts the string to an int
to_long(s)          converts the string to a long
to_long_long(s)     you get the idea
...
to_float(s)         converts the string to a float
...

string_set(dest, src)      sets dest to a copy of src
                           can also convert between char types 

string_copy(src, dest)           copies one string to another
string_copy(src, dest, n)        copies one string to another, up to n characters
string_copy(src, dest, n, off)   copies one string to another, up to n characters, starting in
                                 dest at offset off
string_copy(src)    copies string src and returns a new string instance of the copied string.
string_copy(src, n) copies string src, up to n characters,
                    and returns a new string instance of the copied string.

string_append(dest, other)  appends the string other to the string dest
prepend_string(dest, other) prepends the string other to the string dest

string_index_of(haystack, needle[, offset]) returns the index of the first occurrence of needle
                                     [starting at offset] within the string haystack, or
                                     -1 if needle was not found.

string_last_index_of(haystack, needle[, offset]) returns the index of the last occurrence of needle
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

substring(const_string src, s64 start, s64 length, string out, s64 out_start)
         copies src, starting at start, for up to length characters, into out,
         starting at out_start.
         allocates more memory in out if out does not have enough to fit
         out_start + length characters.

string_replace(str, needle, replacement[, offset]) replaces the first occurrence of needle
                                            with replacement [starting at offset] in
                                            str.

string_replace_all(str, needle, replacement[, offset]) replaces all occurrences of needle
                                                with replacement [starting at offset] in
                                                str.

string_split(str, delim, out_arr) splits the string str by string or character delimiter delim
                           and stores const_strings (pointing to memory inside str) in
                           the array out_arr. resets the size of out_arr.

string_join(strings*, N, delim, out) joins strings together, separated by delim, and writes
                              the output to out. strings is a pointer to N
                              strings (const char**, const_string*, string*, etc.).
string_join(string array, delim, out) same thing as above, except strings is a pointer to an
                               array of strings.

resolve_environment_variables(*str, aliases = false)
    Replaces "$Varname" parts of str with the contents of environment variables,
    e.g. "hello $HOST" replaces "$HOST" with the HOST environment variable
    (assuming the HOST environment variable is set on the system).
    Any variable not found is replaced by an empty string.
    If aliases is true, uses a number of variable aliases to try to be more OS
    agnostic.
    The list of aliases and their contents:

    On Windows:
        HOME    -> gets replaced by the USERPROFILE environment variable

resolve_environment_variables(*cstr, N, aliases = false)
    C string alternative which writes at most N characters.

hash(str) returns a 32 bit hash of the string.
*/

#include "shl/hash.hpp"
#include "shl/array.hpp"
#include "shl/compare.hpp"
#include "shl/string_encoding.hpp"
#include "shl/number_types.hpp"

template<typename C>
struct const_string_base
{
    typedef C value_type;

    const C *c_str; // TODO: rename to data
    s64 size; // size in units, not codepoints or "chars"

    operator bool() const { return c_str != nullptr; }
    explicit operator const C *() const { return c_str; }

    C operator[](s64 i) const { return c_str[i]; }
};

typedef const_string_base<c8>  const_string;
typedef const_string_base<c16> const_u16string;
typedef const_string_base<c32> const_u32string;

const_string    operator ""_cs(const c8  *, u64);
const_u16string operator ""_cs(const c16 *, u64);
const_u32string operator ""_cs(const c32 *, u64);

// this will either return const_u16string or const_u32string depending on size of wchar_t
static inline const_string_base<wc_utf_type> operator ""_cs(const wchar_t *s, u64 len)
{
    return const_string_base{char_cast(s), (s64)len};
}

template<typename C>
struct string_base
{
    typedef C value_type;

    C *data;
    s64 size; // again, units, not codepoints or characters.
    s64 reserved_size;
    ::allocator allocator;

    operator bool() const { return data != nullptr; }
    explicit operator const C *() { return data; }

    operator const_string_base<C>()
    {
        return const_string_base<C>{data, size};
    }

    C &operator[](s64 i)       { return data[i]; }
};

typedef string_base<c8>  string;
typedef string_base<c16> u16string;
typedef string_base<c32> u32string;

static inline auto char_cast(const_string_base<wchar_t> str) { return const_string_base<wc_utf_type>{char_cast(str.c_str), str.size}; }
static inline auto char_cast(string_base<wchar_t> *str) { return (string_base<wc_utf_type>*)str; }

void init(string *str);
void init(string *str, s64 size);
void init(string *str, const c8 *c);
void init(string *str, const c8 *c, s64 size);
void init(string *str, const_string s);
void init(u16string *str);
void init(u16string *str, s64 size);
void init(u16string *str, const c16 *c);
void init(u16string *str, const c16 *c, s64 size);
void init(u16string *str, const_u16string s);
void init(u32string *str);
void init(u32string *str, s64 size);
void init(u32string *str, const c32 *c);
void init(u32string *str, const c32 *c, s64 size);
void init(u32string *str, const_u32string s);

void free(string    *str);
void free(u16string *str);
void free(u32string *str);

// these allocate memory
string    operator ""_s(const c8  *, u64);
u16string operator ""_s(const c16 *, u64);
u32string operator ""_s(const c32 *, u64);

static inline auto operator ""_s(const wchar_t *str, u64 len)
{
    string_base<wc_utf_type> ret{};
    init(&ret, char_cast(str), (s64)len);
    return ret;
};

static inline c8  *string_data(c8  *str) { return str; }
static inline c16 *string_data(c16 *str) { return str; }
static inline c32 *string_data(c32 *str) { return str; }
static inline const c8  *string_data(const c8  *str) { return str; }
static inline const c16 *string_data(const c16 *str) { return str; }
static inline const c32 *string_data(const c32 *str) { return str; }
template<u64 N> constexpr const c8  *string_data(const c8  (&str)[N]) { return str; }
template<u64 N> constexpr const c16 *string_data(const c16 (&str)[N]) { return str; }
template<u64 N> constexpr const c32 *string_data(const c32 (&str)[N]) { return str; }

template<typename C>
static inline const C *string_data(const_string_base<C> str) { return str.c_str; }

template<typename C>
static inline C *string_data(string_base<C> *str) { return str != nullptr ? str->data : nullptr; }

bool string_reserve(string    *s, s64 total_size);
bool string_reserve(u16string *s, s64 total_size);
bool string_reserve(u32string *s, s64 total_size);

void clear(string    *str);
void clear(u16string *str);
void clear(u32string *str);

// iteration
#define for_utf_string_IUIPC(IU_Var, I_Var, P_Var, C_Var, STRING)\
    if constexpr (auto P_Var##_str = to_const_string(STRING); true)\
    if (string_length(P_Var##_str) > 0)\
    if constexpr (s64 IU_Var = 0; true)\
    if constexpr (s64 I_Var = 0;  true)\
    if constexpr (u32 P_Var = utf_decode(P_Var##_str.c_str); true)\
    if constexpr (const_string_base C_Var{string_data(P_Var##_str), utf_codepoint_length(P_Var##_str.c_str)}; true)\
    for (; I_Var < string_length(P_Var##_str);\
           ++IU_Var, I_Var += C_Var.size, C_Var.c_str = utf_advance(C_Var.c_str + C_Var.size, &P_Var, &C_Var.size))

#define for_utf_string_IPC(I_Var, P_Var, C_Var, STRING)\
    if constexpr (auto P_Var##_str = to_const_string(STRING); true)\
    if (string_length(P_Var##_str) > 0)\
    if constexpr (s64 I_Var = 0;  true)\
    if constexpr (u32 P_Var = utf_decode(P_Var##_str.c_str); true)\
    if constexpr (const_string_base C_Var{string_data(P_Var##_str), utf_codepoint_length(P_Var##_str.c_str)}; true)\
    for (; I_Var < string_length(P_Var##_str);\
           I_Var += C_Var.size, C_Var.c_str = utf_advance(C_Var.c_str + C_Var.size, &P_Var, &C_Var.size))

#define for_utf_string_PC(P_Var, C_Var, STRING)\
    if constexpr (auto C_Var = to_const_string(STRING); true)\
    if (string_length(C_Var) > 0)\
    if constexpr (u32 P_Var = utf_decode(C_Var.c_str); true)\
    if constexpr (s64 C_Var##_size_left = C_Var.size; true)\
    for (C_Var.size = utf_codepoint_length(C_Var.c_str); \
         C_Var##_size_left > 0;\
         C_Var##_size_left -= C_Var.size, C_Var.c_str = utf_advance(C_Var.c_str + C_Var.size, &P_Var, &C_Var.size))

#define for_utf_string_C(C_Var, STRING)\
    if constexpr (auto C_Var = to_const_string(STRING); true)\
    if (string_length(C_Var) > 0)\
    if constexpr (s64 C_Var##_size_left = C_Var.size; true)\
    for (C_Var.size = utf_codepoint_length(C_Var.c_str); \
         C_Var##_size_left > 0;\
         C_Var##_size_left -= C_Var.size, C_Var.c_str = utf_advance(C_Var.c_str + C_Var.size, &C_Var.size))

#define for_utf_string(...) GET_MACRO4(__VA_ARGS__, for_utf_string_IUIPC, for_utf_string_IPC, for_utf_string_PC, for_utf_string_C)(__VA_ARGS__)

// character/codepoint functions
bool is_space(u32 codepoint);

#define define_string_is_property(CodepointIsProp)\
    template<typename C>\
    bool CodepointIsProp(const_string_base<C> str)\
    {\
        if (str.size <= 0) return false;\
        for_utf_string(cp, _, str) if (!CodepointIsProp(cp)) return false;\
        return true;\
    }

define_string_is_property(is_space);

bool is_newline(u32 codepoint);
define_string_is_property(is_newline);

bool is_alpha(u32 codepoint);
define_string_is_property(is_alpha);
bool is_digit(u32 codepoint);
define_string_is_property(is_digit);
bool is_bin_digit(u32 codepoint);
define_string_is_property(is_bin_digit);
bool is_oct_digit(u32 codepoint);
define_string_is_property(is_oct_digit);
bool is_hex_digit(u32 codepoint);
define_string_is_property(is_hex_digit);
bool is_alphanum(u32 codepoint);
define_string_is_property(is_alphanum);

bool is_upper(u32 codepoint);
define_string_is_property(is_upper);
bool is_lower(u32 codepoint);
define_string_is_property(is_lower);

// string functions

// specifically the number of units (c8, c16, c32) in the string, NOT codepoints.
s64 string_length(const c8        *s);
s64 string_length(const c16       *s);
s64 string_length(const c32       *s);
s64 string_length(const wchar_t   *s);
template<u64 N> constexpr s64 string_length(const c8  (&s)[N]) { return (s64)N-1; }
template<u64 N> constexpr s64 string_length(const c16 (&s)[N]) { return (s64)N-1; }
template<u64 N> constexpr s64 string_length(const c32 (&s)[N]) { return (s64)N-1; }
s64 string_length(const_string     s);
s64 string_length(const_u16string  s);
s64 string_length(const_u32string  s);
s64 string_length(const string    *s);
s64 string_length(const u16string *s);
s64 string_length(const u32string *s);

template<typename T>
auto string_is_empty(T str)
    -> decltype(string_data(str) != nullptr && string_length(str) == 0)
{
    return string_data(str) != nullptr && string_length(str) == 0;
}

bool _string_is_null_or_empty(const_string    s);
bool _string_is_null_or_empty(const_u16string s);
bool _string_is_null_or_empty(const_u32string s);
template<typename T>
auto string_is_null_or_empty(T str)
    -> decltype(_string_is_null_or_empty(to_const_string(str)))
{
    return _string_is_null_or_empty(to_const_string(str));
}

bool _string_is_blank(const_string    s);
bool _string_is_blank(const_u16string s);
bool _string_is_blank(const_u32string s);
template<typename T>
auto string_is_blank(T str)
    -> decltype(_string_is_blank(to_const_string(str)))
{
    return _string_is_blank(to_const_string(str));
}

// number of codepoints
s64 string_utf_length(const c8        *s);
s64 string_utf_length(const c16       *s);
s64 string_utf_length(const c32       *s);
s64 string_utf_length(const wchar_t   *s);
s64 string_utf_length(const_string     s);
s64 string_utf_length(const_u16string  s);
s64 string_utf_length(const_u32string  s);
s64 string_utf_length(const string    *s);
s64 string_utf_length(const u16string *s);
s64 string_utf_length(const u32string *s);

#define char_to_const_string(C) (const_string_base{&(C), 1})

static inline const_string    to_const_string(const c8  *s) { return const_string{s,    string_length(s)}; }
static inline const_u16string to_const_string(const c16 *s) { return const_u16string{s, string_length(s)}; }
static inline const_u32string to_const_string(const c32 *s) { return const_u32string{s, string_length(s)}; }

template<typename C>
static inline const_string_base<C> to_const_string(const C *s, s64 n)
{
    return const_string_base<C>{s, n};
}

template<s64 N>
constexpr const_string to_const_string(const c8 (&s)[N])
{
    return const_string{s, (s64)N-1}; // -1 because string literals count null terminating char 
}

template<u64 N>
static inline const_u16string to_const_string(const c16 (&s)[N])
{ return const_u16string{s, (s64)N-1}; }

template<u64 N>
static inline const_u32string to_const_string(const c32 (&s)[N])
{ return const_u32string{s, (s64)N-1}; }

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

int _string_compare(const_string    s1, const_string    s2, s64 n);
int _string_compare(const_u16string s1, const_u16string s2, s64 n);
int _string_compare(const_u32string s1, const_u32string s2, s64 n);

template<typename T1, typename T2>
auto string_compare(T1 s1, T2 s2, s64 n = max_value(s64))
    -> decltype(_string_compare(to_const_string(s1), to_const_string(s2), n))
{
    return _string_compare(to_const_string(s1), to_const_string(s2), n);
}

template<> bool equals(string    s1, string    s2);
template<> bool equals(u16string s1, u16string s2);
template<> bool equals(u32string s1, u32string s2);
template<> bool equals_p(const string    *s1, const string    *s2);
template<> bool equals_p(const u16string *s1, const u16string *s2);
template<> bool equals_p(const u32string *s1, const u32string *s2);

bool _string_begins_with(const_string    s, const_string    prefix);
bool _string_begins_with(const_u16string s, const_u16string prefix);
bool _string_begins_with(const_u32string s, const_u32string prefix);

#define string_begins_with(S, Prefix) (_string_begins_with(to_const_string(S), to_const_string(Prefix)))

bool _string_ends_with(const_string    s, const_string    prefix);
bool _string_ends_with(const_u16string s, const_u16string prefix);
bool _string_ends_with(const_u32string s, const_u32string prefix);

#define string_ends_with(S, Prefix) (_string_ends_with(to_const_string(S), to_const_string(Prefix)))

#define DEFINE_INTEGER_SIGNATURE(T, NAME) \
T NAME(const c8     *s, c8 **pos = nullptr, int base = 10);\
T NAME(const_string  s, c8 **pos = nullptr, int base = 10);\
T NAME(const string *s, c8 **pos = nullptr, int base = 10);

#define DEFINE_DECIMAL_SIGNATURE(T, NAME) \
T NAME(const c8     *s, c8 **pos = nullptr);\
T NAME(const_string  s, c8 **pos = nullptr);\
T NAME(const string *s, c8 **pos = nullptr);

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

// sets dst to src, discarding all previous data in dst.
void _string_set(string    *dst, const_string    src);
void _string_set(string    *dst, const_u16string src);
void _string_set(string    *dst, const_u32string src);
void _string_set(u16string *dst, const_string    src);
void _string_set(u16string *dst, const_u16string src);
void _string_set(u16string *dst, const_u32string src);
void _string_set(u32string *dst, const_string    src);
void _string_set(u32string *dst, const_u16string src);
void _string_set(u32string *dst, const_u32string src);

#define string_set(StrPtr, Src) (_string_set((StrPtr), to_const_string(Src)))

c8  *string_copy(const c8  *src, c8  *dst);
c8  *string_copy(const c8  *src, c8  *dst, s64 n);
c16 *string_copy(const c16 *src, c16 *dst);
c16 *string_copy(const c16 *src, c16 *dst, s64 n);
c32 *string_copy(const c32 *src, c32 *dst);
c32 *string_copy(const c32 *src, c32 *dst, s64 n);

// allocates more memory in dst if dst is not large enough to store src
void _string_copy(const_string    src, string    *dst, s64 n, s64 dst_offset);
void _string_copy(const_u16string src, u16string *dst, s64 n, s64 dst_offset);
void _string_copy(const_u32string src, u32string *dst, s64 n, s64 dst_offset);

// T = anything that can become (or is) a const_string_base
template<typename T, typename C>
auto string_copy(T src, string_base<C> *dst, s64 n = max_value(s64), s64 dst_offset = 0)
    -> decltype(_string_copy(to_const_string(src), dst, n, dst_offset))
{
    _string_copy(to_const_string(src), dst, n, dst_offset);
}

string    _string_copy(const_string    src, s64 n);
u16string _string_copy(const_u16string src, s64 n);
u32string _string_copy(const_u32string src, s64 n);

template<typename T>
auto string_copy(T src, s64 n = max_value(s64))
    -> decltype(_string_copy(to_const_string(src), n))
{
    return _string_copy(to_const_string(src), n);
}

void _string_append(string    *dst, const_string    other);
void _string_append(u16string *dst, const_u16string other);
void _string_append(u32string *dst, const_u32string other);

#define string_append(StrPtrDst, Other) (_string_append((StrPtrDst), to_const_string(Other)))

void _string_prepend(string    *dst, const_string    other);
void _string_prepend(u16string *dst, const_u16string other);
void _string_prepend(u32string *dst, const_u32string other);

#define string_prepend(StrPtrDst, Other) (_string_prepend((StrPtrDst), to_const_string(Other)))

s64 string_index_of(const_string    haystack, c8              needle, s64 offset);
s64 string_index_of(const_u16string haystack, c16             needle, s64 offset);
s64 string_index_of(const_u32string haystack, c32             needle, s64 offset);
s64 _string_index_of(const_string    haystack, const_string    needle, s64 offset);
s64 _string_index_of(const_u16string haystack, const_u16string needle, s64 offset);
s64 _string_index_of(const_u32string haystack, const_u32string needle, s64 offset);

template<typename T1, typename T2>
auto string_index_of(T1 haystack, T2 needle, s64 offset = 0)
    -> decltype(_string_index_of(to_const_string(haystack), to_const_string(needle), offset))
{
    return _string_index_of(to_const_string(haystack), to_const_string(needle), offset);
}

template<typename T, typename C>
auto string_index_of(T haystack, c8 needle, s64 offset = 0)
    -> decltype(_string_index_of(to_const_string(haystack), needle, offset))
{
    return _string_index_of(to_const_string(haystack), needle, offset);
}

template<typename T, typename C>
auto string_index_of(T haystack, c16 needle, s64 offset = 0)
    -> decltype(_string_index_of(to_const_string(haystack), needle, offset))
{
    return _string_index_of(to_const_string(haystack), needle, offset);
}

template<typename T, typename C>
auto string_index_of(T haystack, c32 needle, s64 offset = 0)
    -> decltype(_string_index_of(to_const_string(haystack), needle, offset))
{
    return _string_index_of(to_const_string(haystack), needle, offset);
}

s64 _string_last_index_of(const_string    haystack, c8              needle, s64 offset);
s64 _string_last_index_of(const_string    haystack, const_string    needle, s64 offset);
s64 _string_last_index_of(const_u16string haystack, c16             needle, s64 offset);
s64 _string_last_index_of(const_u16string haystack, const_u16string needle, s64 offset);
s64 _string_last_index_of(const_u32string haystack, c32             needle, s64 offset);
s64 _string_last_index_of(const_u32string haystack, const_u32string needle, s64 offset);

template<typename T1, typename T2>
auto string_last_index_of(T1 haystack, T2 needle)
    -> decltype(_string_last_index_of(to_const_string(haystack), to_const_string(needle), 0))
{
    auto h = to_const_string(haystack);
    auto n = to_const_string(needle);

    return _string_last_index_of(h, n, h.size - n.size);
}

template<typename T1, typename T2>
auto string_last_index_of(T1 haystack, T2 needle, s64 offset)
    -> decltype(_string_last_index_of(to_const_string(haystack), to_const_string(needle), offset))
{
    return _string_last_index_of(to_const_string(haystack), to_const_string(needle), offset);
}

template<typename T>
auto string_last_index_of(T haystack, c8 needle)
    -> decltype(_string_last_index_of(to_const_string(haystack), needle, 0))
{
    auto h = to_const_string(haystack);
    return _string_last_index_of(h, needle, h.size - 1);
}

template<typename T>
auto string_last_index_of(T haystack, c8 needle, s64 offset)
    -> decltype(_string_last_index_of(to_const_string(haystack), needle, offset))
{
    return _string_last_index_of(to_const_string(haystack), needle, offset);
}

template<typename T>
auto string_last_index_of(T haystack, c16 needle)
    -> decltype(_string_last_index_of(to_const_string(haystack), needle, 0))
{
    auto h = to_const_string(haystack);
    return _string_last_index_of(h, needle, h.size - 1);
}

template<typename T>
auto string_last_index_of(T haystack, c16 needle, s64 offset)
    -> decltype(_string_last_index_of(to_const_string(haystack), needle, offset))
{
    return _string_last_index_of(to_const_string(haystack), needle, offset);
}

template<typename T>
auto string_last_index_of(T haystack, c32 needle)
    -> decltype(_string_last_index_of(to_const_string(haystack), needle, 0))
{
    auto h = to_const_string(haystack);
    return _string_last_index_of(h, needle, h.size - 1);
}

template<typename T>
auto string_last_index_of(T haystack, c32 needle, s64 offset)
    -> decltype(_string_last_index_of(to_const_string(haystack), needle, offset))
{
    return _string_last_index_of(to_const_string(haystack), needle, offset);
}

template<typename T1, typename T2>
auto contains(T1 haystack, T2 needle)
    -> decltype(string_index_of(to_const_string(haystack), needle) != -1)
{
    return string_index_of(to_const_string(haystack), needle) != -1;
}

bool string_trim_left(string    *s);
bool string_trim_left(u16string *s);
bool string_trim_left(u32string *s);
bool string_trim_right(string    *s);
bool string_trim_right(u16string *s);
bool string_trim_right(u32string *s);
bool string_trim(string    *s);
bool string_trim(u16string *s);
bool string_trim(u32string *s);

c8  char_to_upper(c8  c);
c16 char_to_upper(c16 c);
c32 char_to_upper(c32 c);
void utf_codepoint_to_upper(c8  *codepoint);
void utf_codepoint_to_upper(c16 *codepoint);
void utf_codepoint_to_upper(c32 *codepoint);
void utf_to_upper(c8  *s, s64 size);
void utf_to_upper(c16 *s, s64 size);
void utf_to_upper(c32 *s, s64 size);
void utf_to_upper(string    *s);
void utf_to_upper(u16string *s);
void utf_to_upper(u32string *s);

c8  char_to_lower(c8  c);
c16 char_to_lower(c16 c);
c32 char_to_lower(c32 c);
void utf_codepoint_to_lower(c8  *codepoint);
void utf_codepoint_to_lower(c16 *codepoint);
void utf_codepoint_to_lower(c32 *codepoint);
void utf_to_lower(c8  *s, s64 size);
void utf_to_lower(c16 *s, s64 size);
void utf_to_lower(c32 *s, s64 size);
void utf_to_lower(string    *s);
void utf_to_lower(u16string *s);
void utf_to_lower(u32string *s);

// these operate on units, not UTF codepoints
const_string    _substring(const_string    s, s64 start, s64 length);
const_u16string _substring(const_u16string s, s64 start, s64 length);
const_u32string _substring(const_u32string s, s64 start, s64 length);

template<typename T>
auto substring(T str, s64 start, s64 length = max_value(s64))
    -> decltype(_substring(to_const_string(str), start, length))
{
    return _substring(to_const_string(str), start, length);
}

void substring(const c8  *s, s64 start, s64 length, c8  *out, s64 out_offset = 0);
void substring(const c16 *s, s64 start, s64 length, c16 *out, s64 out_offset = 0);
void substring(const c32 *s, s64 start, s64 length, c32 *out, s64 out_offset = 0);

void _substring(const_string    s, s64 start, s64 length, string    *out, s64 out_offset);
void _substring(const_u16string s, s64 start, s64 length, u16string *out, s64 out_offset);
void _substring(const_u32string s, s64 start, s64 length, u32string *out, s64 out_offset);

template<typename C, typename T>
auto substring(T str, s64 start, s64 length, string_base<C> *out, s64 out_offset = 0)
    -> decltype(_substring(to_const_string(str), start, length, out, out_offset))
{
    _substring(to_const_string(str), start, length, out, out_offset);
}

const_string    _utf_substring(const_string    s, s64 start, s64 length);
const_u16string _utf_substring(const_u16string s, s64 start, s64 length);
const_u32string _utf_substring(const_u32string s, s64 start, s64 length);

template<typename T>
auto utf_substring(T str, s64 start, s64 length = max_value(s64))
    -> decltype(_substring(to_const_string(str), start, length))
{
    return _utf_substring(to_const_string(str), start, length);
}

void utf_substring(const c8  *s, s64 start, s64 length, c8  *out, s64 out_offset = 0);
void utf_substring(const c16 *s, s64 start, s64 length, c16 *out, s64 out_offset = 0);
void utf_substring(const c32 *s, s64 start, s64 length, c32 *out, s64 out_offset = 0);

void _utf_substring(const_string    s, s64 start, s64 length, string    *out, s64 out_offset);
void _utf_substring(const_u16string s, s64 start, s64 length, u16string *out, s64 out_offset);
void _utf_substring(const_u32string s, s64 start, s64 length, u32string *out, s64 out_offset);

template<typename C, typename T>
auto utf_substring(T str, s64 start, s64 length, string_base<C> *out, s64 out_offset = 0)
    -> decltype(_substring(to_const_string(str), start, length, out, out_offset))
{
    _utf_substring(to_const_string(str), start, length, out, out_offset);
}

void _string_replace(string    *s, const_string    needle, const_string    replacement, s64 offset);
void _string_replace(u16string *s, const_u16string needle, const_u16string replacement, s64 offset);
void _string_replace(u32string *s, const_u32string needle, const_u32string replacement, s64 offset);

template<typename C, typename TNeedle, typename TReplacement>
auto string_replace(string_base<C> *s, TNeedle needle, TReplacement replacement, s64 offset = 0)
    -> decltype(_string_replace(s, to_const_string(needle), to_const_string(replacement), offset))
{
    _string_replace(s, to_const_string(needle), to_const_string(replacement), offset);
}

void _string_replace_all(string    *s, const_string    needle, const_string    replacement, s64 offset);
void _string_replace_all(u16string *s, const_u16string needle, const_u16string replacement, s64 offset);
void _string_replace_all(u32string *s, const_u32string needle, const_u32string replacement, s64 offset);

template<typename C, typename TNeedle, typename TReplacement>
auto string_replace_all(string_base<C> *s, TNeedle needle, TReplacement replacement, s64 offset = 0)
    -> decltype(_string_replace_all(s, to_const_string(needle), to_const_string(replacement), offset))
{
    _string_replace_all(s, to_const_string(needle), to_const_string(replacement), offset);
}

s64 _string_split(const_string    s, const_string    delim, array<const_string>    *out);
s64 _string_split(const_u16string s, const_u16string delim, array<const_u16string> *out);
s64 _string_split(const_u32string s, const_u32string delim, array<const_u32string> *out);

template<typename C, typename TStr, typename TDelim>
auto string_split(TStr s, TDelim delim, array<const_string_base<C>> *out)
    -> decltype(_string_split(to_const_string(s), to_const_string(delim), out))
{
    return _string_split(to_const_string(s), to_const_string(delim), out);
}

template<typename C, typename T>
auto string_split(T s, C delim, array<const_string_base<C>> *out)
    -> decltype(_string_split(to_const_string(s), delim, out))
{
    return _string_split(to_const_string(s), delim, out);
}

void _string_join(const c8     **strings, s64 count, const_string delim, string *out);
void _string_join(const_string  *strings, s64 count, const_string delim, string *out);
void _string_join(const string  *strings, s64 count, const_string delim, string *out);
void _string_join(const c16       **strings, s64 count, const_u16string delim, u16string *out);
void _string_join(const_u16string  *strings, s64 count, const_u16string delim, u16string *out);
void _string_join(const u16string  *strings, s64 count, const_u16string delim, u16string *out);
void _string_join(const c32       **strings, s64 count, const_u32string delim, u32string *out);
void _string_join(const_u32string  *strings, s64 count, const_u32string delim, u32string *out);
void _string_join(const u32string  *strings, s64 count, const_u32string delim, u32string *out);

template<typename C, typename T>
auto string_join(const C **strings, s64 count, T delim, string_base<C> *out)
    -> decltype(_string_join(strings, count, to_const_string(delim), out))
{
    return _string_join(strings, count, to_const_string(delim), out);
}

void resolve_environment_variables(c8        *str, s64 size, bool aliases = false);
void resolve_environment_variables(string    *str, bool aliases = false);
void resolve_environment_variables(c16       *str, s64 size, bool aliases = false);
void resolve_environment_variables(u16string *str, bool aliases = false);
void resolve_environment_variables(c32       *str, s64 size, bool aliases = false);
void resolve_environment_variables(u32string *str, bool aliases = false);

/* These are not the same as hash(const char *c) because hash(const char *c)
calculates the size. Every time. Also hash(const char *c) stops at the first
null character because string_length is used to determine the size, whereas
these hash functions hash the entire size of the string, including all null
characters (except the null character past-the-end).
*/
hash_t hash(const_string           str);
hash_t hash(const_u16string        str);
hash_t hash(const_u32string        str);
hash_t hash(const const_string    *str);
hash_t hash(const const_u16string *str);
hash_t hash(const const_u32string *str);
hash_t hash(const string          *str);
hash_t hash(const u16string       *str);
hash_t hash(const u32string       *str);

#define define_comparison_operators(T1, T2)\
    static inline bool operator==(T1 a, T2 b)\
    {\
        return string_compare(a, b) == 0;\
    }\
    \
    static inline bool operator!=(T1 a, T2 b)\
    {\
        return string_compare(a, b) != 0;\
    }

define_comparison_operators(const_string,    const_string);
define_comparison_operators(const_u16string, const_u16string);
define_comparison_operators(const_u32string, const_u32string);

