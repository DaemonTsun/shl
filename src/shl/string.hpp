#pragma once

/* string.hpp

String utility header.

Types:
const_string, const_u16string, const_u32string
    Structs containing pointers to const string data, as well as a size which is the
    number of units in the string.
    const_string    is UTF-8,
    const_u16string is UTF-16,
    const_u32string is UTF-32.

    const_string & co aren't guaranteed to be null terminated and are often
    used as "slices" to strings.

    To construct a const_string, either initialize with const_string{ptr, size},
    use the string literal suffix _cs, or use to_const_string(...), such as:

        const_string mystr = "hello world!"_cs;
        const char *cstr = mystr.c_str;
        s64 size = mystr.size; // or string_length(mystr);

        const_string mystr2 = to_const_string("hello");

    Using the _cs string literal suffix operator on a wchar_t string literal
    (e.g. L"hello"_cs) will yield either a const_u16string or const_u32string,
    depending on the size of wchar_t.

    Since const_strings are relatively lightweight, they can be copied and don't
    need to be referenced (they're essentially just a pointer with a size).
    const_strings also don't own the string they point to, and as such don't
    need to be freed.

string, u16string, u32string
    Modifiable strings with memory management.
    Contain a pointer to their data, unit count, reserved unit count and an allocator.
    string    is UTF-8,
    u16string is UTF-16,
    u32string is UTF-32.

    string & co are null-terminated and can be implicitly converted to const_string & co.
    These manage memory, and as such need to be free'd.

    To construct a string, either use init(&str, ...) or use the _s suffix for
    string literals, such as:

         string mystr = u8"hello world!"_s;                    // copy of string literal
         const char *cstr = mystr.data;
         const_string cs = to_const_string(&mystr);
         s64 size = string_length(&mystr);
         free(&mystr);                                       // every string must be freed.


String/Character functions
--------------------------
(for more comprehensive usage, see tests/string_tests.cpp)

init(string *str, ...)
    Initialized a string. See implementation for parameters, but function accepts pretty much
    anything.

free(string *str)
    Frees the string, deallocating memory.

string_data(...)
    Returns a pointer to the string data of the parameter given. Accepts pointers, strings,
    const_strings, string literals, etc.

string_reserve(string *str, s64 size)
    Allocates enough memory in str to store at least size+1 characters.
    Does nothing if str can already hold size+1 characters.
    (currently) allocates in powers of two.

clear(string *str)
    Sets string size to 0, but does not deallocate anything and does not
    clear any memory... wait...

utf_advance(const_string str)
    Advances the string str by one codepoint and returns it.

Character / Codepoint functions
is_space(u32 cp)
    Returns true if cp is a whitespace codepoint.
is_space(const_string str)
    Returns true if str contains only whitespace codepoints.

is_newline(u32 cp) 
    Returns true if cp is a newline codepoint.
is_newline(const_string str) 
    Returns true if str contains only newline codepoints.

is_alpha(u32 cp)
    Returns true if cp is an alphabet codepoint.
is_alpha(const_string str)
    Returns true if str contains only alphabet codepoints.

is_digit(u32 cp)
    Returns true if cp is a digit codepoint.
is_digit(const_string str)
    Returns true if str contains only digit codepoints.

is_bin_digit(u32 cp)
    Returns true if cp is a binary digit codepoint.
is_bin_digit(const_string str)
    Returns true if str contains only binary digit codepoints.

is_oct_digit(u32 cp)
    Returns true if cp is a octal digit codepoint.
is_oct_digit(const_string str)
    Returns true if str contains only octal digit codepoints.

is_hex_digit(u32 cp)
    Returns true if cp is a hexadecimal digit codepoint.
is_hex_digit(const_string str)
    Returns true if str contains only hexadecimal digit codepoints.

is_alphanum(u32 cp)
    Returns true if cp is an alphabet or digit codepoint.
is_alphanum(const_string str)
    Returns true if str contains only alphabet or digit codepoints.

is_upper(u32 cp)
    Returns true if cp is an upper case alphabetical codepoint.
is_upper(const_string str)
    Returns true if str only contains upper case alphabetical codepoint.

is_lower(u32 cp)
    Returns true if cp is an lower case alphabetical codepoint.
is_lower(const_string str)
    Returns true if str only contains lower case alphabetical codepoint.

string_length(...)
    Returns the number of units, not codepoints, not including null at the end,
    of the given parameter. Accepts most strings as parameter.
    Constexpr for string literals.

to_const_string(...)
    Returns a const_string of the parameter. Accepts most of anything that
    can be a const_string.
    Constexpr for string literals.


NOTE: starting here, functions may have just a "Str" parameter, these are
      (usually) templated functions that accept anything that can become a
      const_string, or are overloaded to accept any string.

string_is_empty(Str)
    Returns true if Str is an empty string (size 0), but not nullptr.

string_is_null_or_empty(Str)
    Returns true if Str is nullptr or an empty string.

string_is_blank(Str)
    Returns true if Str is nullptr, an empty string or only contains
    whitespaces.

string_utf_length(Str)
    Returns the number of UTF codepoints in the string.

string_compare(Str s1, Str s2[, s64 n])
    Compares two strings s1 and s2, optionally up to n units.
    Returns 0 if s1 and s2 are equivalent, non-zero otherwise.

string_begins_with(Str s, Str prefix)
    Returns true if prefix is a prefix of s.

string_ends_with(Str s, Str suffix)
    Returns true if suffix is a suffix of s.

utf_codepoint_digit_value(u32 cp)
    Returns the digit value (0-9) of the given codepoint, or -1 if
    cp is not a digit (or is not implemented).

string_to_s32(Str s, const_string *next = nullptr, int base = 0, error *err = nullptr)
    Converts s into an s32.
    Parses the string similarly to strtol, using base as base.
    On error, next is set to s, returns 0 and sets err to the error according to strtol.
    Otherwise, returns the parsed number and sets next to after the string s.

string_to_s8 (...)
string_to_s16(...)
string_to_s64(...)
string_to_u8 (...)   all same as string_to_s32, but different number type
string_to_u16(...)
string_to_u32(...)
string_to_u64(...)

string_to_float(Str s, const_string *next = nullptr, error *err = nullptr)
    Converts s into a float.
    Parses the string similarly to strtod, using base as base.
    On error, next is set to s, returns 0 and sets err to the error according to strtod.
    Otherwise, returns the parsed number and sets next to after the string s.

string_to_double(...) same as string_to_float

string_set(string *dest, Str src)
    Copies src into dest, discarding anything in dest before setting.
    NOTE: string_set may also convert between UTF types, e.g.:

    string s{}; 
    string_set(&s, U"hello world"); // UTF-32 string literal

string_copy(Str src, string *dest[, s64 n, s64 dest_offset])
    Copies string from src to dest, optionally up to n units, optionally starting
    within dest at dest_offset.
    Does not clear dest before copying, so src is copied directly into dest.

string_copy(Str src[, s64 n])
    Returns a string copy of src, optionally up to n units.

string_append(string *dest, Str other)
    Appends the string other to the string dest, allocating more memory if
    necessary.

string_prepend(string *dest, Str other)
    Prepends the string other to the string dest, allocating more memory if
    necessary.

string_index_of(Str haystack, Str needle[, offset])
    Returns the unit index of the first occurrence of needle,
    optionally starting at offset, within the string haystack,
    or -1 if needle was not found.

string_last_index_of(haystack, needle[, offset])
    Returns the unit index of the last occurrence of needle,
    optionally ending at offset (moving towards the beginning),
    within the string haystack, or -1 if needle was not found.

contains(Str haystack, Str needle)
    Returns true if haystack contains needle, false if not.

string_trim_left(string *s)
    Trims whitespaces from the left of string s, in-place.
    Returns true if anything was trimmed.

string_trim_right(string *s)
    Trims whitespaces from the right of string s, in-place.
    Returns true if anything was trimmed.

string_trim(string *s)
    Trims whitespaces from the left and right of string s, in-place.
    Returns true if anything was trimmed.

char_to_upper(c8/c16/c32)
    Converts the given character or string to upper case.
    NOTE: using character types is probably not what you want when dealing with UTF.

char_to_lower(c8/c16/c32)
    Converts the given character or string to upper case.

utf_codepoint_to_upper(c32 cp)
    Returns the upper case UTF codepoint of cp, or cp if cp is already
    upper case or does not have an upper case (or is not implemented).

utf_codepoint_to_upper(c8* / c16* / c32* )
    Changes the given UTF-8/16/32 codepoint from lower case to upper case.
    Only a single codepoint is affected.

utf_to_upper(string *s)
    Converts the entire string s to upper case.

utf_codepoint_to_lower(c32 cp)
    Returns the lower case UTF codepoint of cp, or cp if cp is already
    lower case or does not have an lower case (or is not implemented).

utf_codepoint_to_lower(c8* / c16* / c32* )
    Changes the given UTF-8/16/32 codepoint from upper case to lower case.
    Only a single codepoint is affected.

utf_to_lower(string *s)
    Converts the entire string s to lower case.

substring(Str src, s64 start[, s64 length])
    Returns a slice (not a copy) of the substring of src, starting at
    unit start, up to length units.

substring(Str src, s64 start, s64 length, string *out[, s64 out_start])
     Copies src, starting at unit start, for up to length units, into out,
     starting at unit out_start.
     Allocates more memory in out if out does not have enough to fit
     out_start + length units.

utf_substring(Str src, s64 start[, s64 length])
    Returns a slice (not a copy) of the substring of src, starting at
    codepoint start, up to length codepoints.

utf_substring(Str src, s64 start, s64 length, string *out[, s64 out_start])
     Copies src, starting at codepoint start, for up to length codepoints, into out,
     starting at codepoint out_start.
     Allocates more memory in out if out does not have enough to fit
     out_start + length codepoints.

string_replace(string *str, Str needle, Str replacement[, s64 offset])
    Replaces the first occurrence of needle with replacement [starting at offset] in
    str.

string_replace_all(string *str, Str needle, Str replacement[, s64 offset])
    Replaces the all occurrences of needle with replacement [starting at offset] in
    str.

string_split(Str s, Str delim, array<const_string> *out)
    Splits Str by delimiter delim and stores slices (const_string, pointing into str)
    in the array out. Resets the size of out.

string_join(Str* strings, s64 n, Str delim, string *out)
    Joins strings together, separated by delim, and writes the output to out.
    strings is a pointer to N strings (const char**, const_string*, string*, etc.).

resolve_environment_variables(string *str, bool aliases = false)
    Replaces "$Varname" parts of str with the contents of environment variables,
    e.g. "hello $HOST" replaces "$HOST" with the HOST environment variable
    (assuming the HOST environment variable is set on the system).
    Any variable not found is replaced by an empty string.
    If aliases is true, uses a number of variable aliases to try to be more OS
    agnostic.
    The list of aliases and their contents:

    On Windows:
        HOME    -> gets replaced by the USERPROFILE environment variable


hash(Str)
    Returns a 32 bit hash of the string.


Iterating a UTF string:
The for_utf_string macro may be used to iterate over a UTF string, yielding:
    The current codepoint in the UTF format of the string,
    the decoded codepoint,
    the unit index within the string where the codepoint is at and
    the UTF index.

    Example:
    const_string uts = u8"今日は привет"_cs;

    for_utf_string(i_utf, i_str, cp, c, uts)
    {
        // i_utf is the UTF index
        // i_str is the unit index
        // cp is the decoded codepoint
        // c is a const_string containing only the current codepoint
        tprint("%#2s %#2s 0x%08x %: ", i_str, i_utf, cp, c.size);
        put(c);
        put("\n");
    }
*/

#include "shl/hash.hpp"
#include "shl/array.hpp"
#include "shl/compare.hpp"
#include "shl/string_encoding.hpp"
#include "shl/error.hpp"
#include "shl/number_types.hpp"

template<typename C>
struct const_string_base
{
    typedef C value_type;

    const C *c_str;
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
typedef string_base<sys_char> sys_string;

// These aren't technically supported anyway.
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
           ++IU_Var, I_Var += C_Var.size, C_Var.c_str = _for_utf_decode(C_Var.c_str + C_Var.size, &P_Var, &C_Var.size))

#define for_utf_string_IPC(I_Var, P_Var, C_Var, STRING)\
    if constexpr (auto P_Var##_str = to_const_string(STRING); true)\
    if (string_length(P_Var##_str) > 0)\
    if constexpr (s64 I_Var = 0;  true)\
    if constexpr (u32 P_Var = utf_decode(P_Var##_str.c_str); true)\
    if constexpr (const_string_base C_Var{string_data(P_Var##_str), utf_codepoint_length(P_Var##_str.c_str)}; true)\
    for (; I_Var < string_length(P_Var##_str);\
           I_Var += C_Var.size, C_Var.c_str = _for_utf_decode(C_Var.c_str + C_Var.size, &P_Var, &C_Var.size))

#define for_utf_string_PC(P_Var, C_Var, STRING)\
    if constexpr (auto C_Var = to_const_string(STRING); true)\
    if (string_length(C_Var) > 0)\
    if constexpr (u32 P_Var = utf_decode(C_Var.c_str); true)\
    if constexpr (s64 C_Var##_size_left = C_Var.size; true)\
    for (C_Var.size = utf_codepoint_length(C_Var.c_str); \
         C_Var##_size_left > 0;\
         C_Var##_size_left -= C_Var.size, C_Var.c_str = _for_utf_decode(C_Var.c_str + C_Var.size, &P_Var, &C_Var.size))

#define for_utf_string_C(C_Var, STRING)\
    if constexpr (auto C_Var = to_const_string(STRING); true)\
    if (string_length(C_Var) > 0)\
    if constexpr (s64 C_Var##_size_left = C_Var.size; true)\
    for (C_Var.size = utf_codepoint_length(C_Var.c_str); \
         C_Var##_size_left > 0;\
         C_Var##_size_left -= C_Var.size, C_Var.c_str = _for_utf_decode(C_Var.c_str + C_Var.size, &C_Var.size))

#define for_utf_string(...) GET_MACRO4(__VA_ARGS__, for_utf_string_IUIPC, for_utf_string_IPC, for_utf_string_PC, for_utf_string_C)(__VA_ARGS__)

template<typename C>
inline const_string_base<C> utf_advance(const_string_base<C> str)
{
    if (str.c_str == nullptr || str.size <= 0)
        return str;

    int len = utf_codepoint_length(str.c_str);
    return const_string_base<C>{str.c_str + len, str.size - len};
}

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

template<typename T>
static inline auto string_is_empty(T str)
    -> decltype(string_data(str) != nullptr && string_length(str) == 0)
{
    return string_data(str) != nullptr && string_length(str) == 0;
}

template<typename T>
static inline auto string_is_null_or_empty(T str)
    -> decltype(string_data(str) == nullptr || string_length(str) == 0)
{
    return string_data(str) == nullptr || string_length(str) == 0;
}

bool _string_is_blank(const_string    s);
bool _string_is_blank(const_u16string s);
bool _string_is_blank(const_u32string s);
template<typename T>
static inline auto string_is_blank(T str)
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

// returns -1 if invalid
int utf_codepoint_digit_value(u32 cp);

#define declare_string_to_integer(NumberType)\
    NumberType _string_to_##NumberType(const_string    s, const_string    *next, int base, error *err);\
    NumberType _string_to_##NumberType(const_u16string s, const_u16string *next, int base, error *err);\
    NumberType _string_to_##NumberType(const_u32string s, const_u32string *next, int base, error *err);\
    \
    template<typename T>\
    auto string_to_##NumberType(T s, const_string_base<typename decltype(to_const_string(s))::value_type> *next = nullptr, int base = 0, error *err = nullptr)\
        -> decltype(_string_to_##NumberType(to_const_string(s), next, base, err))\
    {\
        return _string_to_##NumberType(to_const_string(s), next, base, err);\
    }

declare_string_to_integer(s8);
declare_string_to_integer(s16);
declare_string_to_integer(s32);
declare_string_to_integer(s64);
declare_string_to_integer(u8);
declare_string_to_integer(u16);
declare_string_to_integer(u32);
declare_string_to_integer(u64);

#define declare_string_to_decimal(NumberType)\
    NumberType _string_to_##NumberType(const_string    s, const_string    *next, error *err);\
    NumberType _string_to_##NumberType(const_u16string s, const_u16string *next, error *err);\
    NumberType _string_to_##NumberType(const_u32string s, const_u32string *next, error *err);\
    \
    template<typename T>\
    auto string_to_##NumberType(T s, const_string_base<typename decltype(to_const_string(s))::value_type> *next = nullptr, error *err = nullptr)\
        -> decltype(_string_to_##NumberType(to_const_string(s), next, err))\
    {\
        return _string_to_##NumberType(to_const_string(s), next, err);\
    }

declare_string_to_decimal(float);
declare_string_to_decimal(double);

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

template<typename C, typename T>
auto string_set(string_base<C> *dst, T src)
    -> decltype(_string_set(dst, to_const_string(src)))
{
    return _string_set(dst, to_const_string(src));
}

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

s64 string_index_of(const_string    haystack, c8              needle, s64 offset = 0);
s64 string_index_of(const_u16string haystack, c16             needle, s64 offset = 0);
s64 string_index_of(const_u32string haystack, c32             needle, s64 offset = 0);
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

s64 string_last_index_of(const_string    haystack, c8              needle, s64 offset = max_value(s64));
s64 string_last_index_of(const_u16string haystack, c16             needle, s64 offset = max_value(s64));
s64 string_last_index_of(const_u32string haystack, c32             needle, s64 offset = max_value(s64));
s64 _string_last_index_of(const_string    haystack, const_string    needle, s64 offset);
s64 _string_last_index_of(const_u16string haystack, const_u16string needle, s64 offset);
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
u32  utf_codepoint_to_upper(u32  codepoint);
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

template<typename X, typename C, typename T>
auto string_join(X strings, s64 count, T delim, string_base<C> *out)
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
