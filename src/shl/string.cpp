
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>

#include "shl/memory.hpp"
#include "shl/compare.hpp"
#include "shl/string.hpp"

const_string  operator ""_cs(const char *str, u64 n)
{
    return const_string{str, n};
}

const_wstring operator ""_cs(const wchar_t *str, u64 n)
{
    return const_wstring{str, n};
}

string  operator ""_s(const char *str, u64 n)
{
    string ret;
    init(&ret, str, n);
    return ret;
}

wstring operator ""_s(const wchar_t *str, u64 n)
{
    wstring ret;
    init(&ret, str, n);
    return ret;
}

template<typename C>
void _init(string_base<C> *str)
{
    assert(str != nullptr);

    init(&str->data);
}

template<typename C>
void _init(string_base<C> *str, u64 size)
{
    assert(str != nullptr);

    init(&str->data, size + 1);
    str->data.size -= 1;
    str->data[size] = '\0';
}

template<typename C>
void _init(string_base<C> *str, const C *c)
{
    init(str, const_string_base<C>{c, string_length(c)});
}

template<typename C>
void _init(string_base<C> *str, const C *c, u64 size)
{
    assert(str != nullptr);

    init(str, const_string_base<C>{c, size});
}

template<typename C>
void _init(string_base<C> *str, const_string_base<C> cs)
{
    assert(str != nullptr);

    init(&str->data, cs.size + 1);
    str->data.size -= 1;
    copy_string(cs, str);
    str->data[cs.size] = '\0';
}

void init(string *str)
{
    _init(str);
}

void init(string *str, u64 size)
{
    _init(str, size);
}

void init(string *str, const char *c)
{
    _init(str, c);
}

void init(string *str, const char *c, u64 size)
{
    _init(str, c, size);
}

void init(string *str, const_string s)
{
    _init(str, s);
}

void init(wstring *str)
{
    _init(str);
}

void init(wstring *str, u64 size)
{
    _init(str, size);
}

void init(wstring *str, const wchar_t *c)
{
    _init(str, c);
}

void init(wstring *str, const wchar_t *c, u64 size)
{
    _init(str, c, size);
}

void init(wstring *str, const_wstring s)
{
    _init(str, s);
}

void clear(string  *str)
{
    assert(str != nullptr);

    clear(&str->data);
    str->data.data[0] = '\0';
}

void clear(wstring *str)
{
    assert(str != nullptr);

    clear(&str->data);
    str->data.data[0] = '\0';
}

void free(string  *str)
{
    assert(str != nullptr);

    free(&str->data);
}

void free(wstring *str)
{
    assert(str != nullptr);

    free(&str->data);
}

// string / char functions
bool is_space(char c)
{
    return isspace(c);
}

bool is_space(wchar_t c)
{
    return iswspace(static_cast<wint_t>(c));
}

bool is_newline(char c)
{
    return c == '\n' || c == '\v' || c == '\f';
}

bool is_newline(wchar_t c)
{
    return c == L'\n' || c == L'\v' || c == L'\f';
}

bool is_alpha(char c)
{
    return isalpha(c);
}

bool is_alpha(wchar_t c)
{
    return iswalpha(static_cast<wint_t>(c));
}

bool is_digit(char c)
{
    return isdigit(c);
}

bool is_digit(wchar_t c)
{
    return iswdigit(static_cast<wint_t>(c));
}

bool is_bin_digit(char c)
{
    return c == '0' || c == '1';
}

bool is_bin_digit(wchar_t c)
{
    return c == L'0' || c == L'1';
}

bool is_oct_digit(char c)
{
    return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7';
}

bool is_oct_digit(wchar_t c)
{
    return c == L'0' || c == L'1' || c == L'2' || c == L'3' || c == L'4' || c == L'5' || c == L'6' || c == L'7';
}

bool is_hex_digit(char c)
{
    return isxdigit(c);
}

bool is_hex_digit(wchar_t c)
{
    return iswxdigit(static_cast<wint_t>(c));
}

bool is_alphanum(char c)
{
    return isalnum(c);
}

bool is_alphanum(wchar_t c)
{
    return iswalnum(static_cast<wint_t>(c));
}

bool is_upper(char c)
{
    return isupper(static_cast<int>(c));
}

bool is_upper(wchar_t c)
{
    return iswupper(static_cast<wint_t>(c));
}

bool is_lower(char c)
{
    return islower(static_cast<int>(c));
}

bool is_lower(wchar_t c)
{
    return iswlower(static_cast<wint_t>(c));
}

template<typename C>
bool _is_empty(const C *s)
{
    return (s != nullptr) && (*s == '\0');
}

bool is_empty(const char *s)
{
    return _is_empty(s);
}

bool is_empty(const wchar_t *s)
{
    return _is_empty(s);
}

template<typename C>
bool _is_empty_cs(const_string_base<C> s)
{
    return (s.c_str != nullptr) && (s.size == 0);
}

bool is_empty(const_string s)
{
    return _is_empty_cs(s);
}

bool is_empty(const_wstring s)
{
    return _is_empty_cs(s);
}

bool is_empty(const string *s)
{
    assert(s != nullptr);
    return _is_empty_cs(to_const_string(s));
}

bool is_empty(const wstring *s)
{
    assert(s != nullptr);
    return _is_empty_cs(to_const_string(s));
}

template<typename C>
bool _is_null_or_empty(const C *s)
{
    return (s == nullptr) || (*s == '\0');
}

bool is_null_or_empty(const char *s)
{
    return _is_null_or_empty(s);
}

bool is_null_or_empty(const wchar_t *s)
{
    return _is_null_or_empty(s);
}

template<typename C>
bool _is_null_or_empty_cs(const_string_base<C> s)
{
    return (s.c_str == nullptr) || (s.size == 0);
}

bool is_null_or_empty(const_string s)
{
    return _is_null_or_empty_cs(s);
}

bool is_null_or_empty(const_wstring s)
{
    return _is_null_or_empty_cs(s);
}

bool is_null_or_empty(const string *s)
{
    assert(s != nullptr);
    return _is_null_or_empty_cs(to_const_string(s));
}

bool is_null_or_empty(const wstring *s)
{
    assert(s != nullptr);
    return _is_null_or_empty_cs(to_const_string(s));
}

template<typename C>
bool _is_blank(const C *s)
{
    if (s == nullptr)
        return true;

    while (*s != '\0')
    {
        if (!is_space(*s))
            return false;

        ++s;
    }

    return true;
}

bool is_blank(const char *s)
{
    return _is_blank(s);
}

bool is_blank(const wchar_t *s)
{
    return _is_blank(s);
}

template<typename C>
bool _is_blank_cs(const_string_base<C> s)
{
    u64 i = 0;
    u64 size = s.size;
    C c;

    while (i < size)
    {
        c = s.c_str[i];

        if (!(is_space(c) || c == '\0'))
            return false;

        ++i;
    }

    return true;
}

bool is_blank(const_string s)
{
    return _is_blank_cs(s);
}

bool is_blank(const_wstring s)
{
    return _is_blank_cs(s);
}

bool is_blank(const string *s)
{
    assert(s != nullptr);
    return _is_blank_cs(to_const_string(s));
}

bool is_blank(const wstring *s)
{
    assert(s != nullptr);
    return _is_blank_cs(to_const_string(s));
}

u64 string_length(const char *s)
{
    return strlen(s);
}

u64 string_length(const wchar_t *s)
{
    return wcslen(s);
}

u64 string_length(const_string s)
{
    return s.size;
}

u64 string_length(const_wstring s)
{
    return s.size;
}

u64 string_length(const string *s)
{
    if (s == nullptr)
        return 0;

    return s->data.size;
}

u64 string_length(const wstring *s)
{
    if (s == nullptr)
        return 0;

    return s->data.size;
}

int compare_strings(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

int compare_strings(const char *s1, const char *s2, u64 n)
{
    return strncmp(s1, s2, n);
}

int compare_strings(const wchar_t *s1, const wchar_t *s2)
{
    return wcscmp(s1, s2);
}

int compare_strings(const wchar_t *s1, const wchar_t *s2, u64 n)
{
    return wcsncmp(s1, s2, n);
}

template<typename C>
int _compare_strings_cs(const_string_base<C> s1, const_string_base<C> s2, u64 n)
{
    int res = 0;
    u64 i = 0;

    if ((s1.size < n || s2.size < n) && s1.size != s2.size)
        return compare_ascending(s1.size, s2.size);

    if (n > s1.size)
        n = s1.size;

    if (n > s2.size)
        n = s2.size;

    while (res == 0 && i < n)
    {
        res = compare_ascending(s1.c_str[i], s2.c_str[i]);
        i += 1;
    }

    return res;
}

int compare_strings(const_string s1, const_string s2)
{
    return _compare_strings_cs(s1, s2, -1);
}

int compare_strings(const_string s1, const_string s2, u64 n)
{
    return _compare_strings_cs(s1, s2, n);
}

int compare_strings(const_wstring s1, const_wstring s2)
{
    return _compare_strings_cs(s1, s2, -1);
}

int compare_strings(const_wstring s1, const_wstring s2, u64 n)
{
    return _compare_strings_cs(s1, s2, n);
}

template<typename C>
int _compare_strings_s(const string_base<C> *s1, const string_base<C> *s2, u64 n)
{
    if (s1 == s2)
        return 0;

    if (s1 == nullptr)
        return -1;

    if (s2 == nullptr)
        return 1;

    return _compare_strings_cs(to_const_string(s1), to_const_string(s2), n);
}

int compare_strings(const string *s1, const string *s2)
{
    return _compare_strings_s(s1, s2, -1);
}

int compare_strings(const string *s1, const string *s2, u64 n)
{
    return _compare_strings_s(s1, s2, n);
}

int compare_strings(const wstring *s1, const wstring *s2)
{
    return _compare_strings_s(s1, s2, -1);
}

int compare_strings(const wstring *s1, const wstring *s2, u64 n)
{
    return _compare_strings_s(s1, s2, n);
}

template<typename C>
bool _begins_with(const C *s, const C *prefix)
{
    return compare_strings(prefix, s, string_length(prefix)) == 0;
}

bool begins_with(const char *s, const char *prefix)
{
    return _begins_with(s, prefix);
}

bool begins_with(const wchar_t *s, const wchar_t *prefix)
{
    return _begins_with(s, prefix);
}

template<typename C>
bool _begins_with_cs(const_string_base<C> s, const_string_base<C> prefix)
{
    return compare_strings(prefix, s, string_length(prefix)) == 0;
}

bool begins_with(const_string s, const_string prefix)
{
    return _begins_with_cs(s, prefix);
}

bool begins_with(const_wstring s, const_wstring prefix)
{
    return _begins_with_cs(s, prefix);
}

template<typename C>
bool _begins_with_s(const string_base<C> *s, const string_base<C> *prefix)
{
    return compare_strings(prefix, s, string_length(prefix)) == 0;
}

bool begins_with(const string *s, const string *prefix)
{
    return _begins_with_s(s, prefix);
}

bool begins_with(const wstring *s, const wstring *prefix)
{
    return _begins_with_s(s, prefix);
}

template<typename C>
bool _ends_with(const C *s, const C *suffix)
{
    u64 str_length = string_length(s);
    u64 suffix_length = string_length(suffix);

    return (str_length >= suffix_length) &&
           (compare_strings(s + str_length - suffix_length, suffix, suffix_length) == 0);
}

bool ends_with(const char *s, const char *prefix)
{
    return _ends_with(s, prefix);
}

bool ends_with(const wchar_t *s, const wchar_t *prefix)
{
    return _ends_with(s, prefix);
}

template<typename C>
bool _ends_with_cs(const_string_base<C> s, const_string_base<C> suffix)
{
    u64 str_length = string_length(s);
    u64 suffix_length = string_length(suffix);

    if (str_length < suffix_length)
        return false;

    const_string_base<C> s2{s.c_str + str_length - suffix_length, suffix_length};

    return compare_strings(s2, suffix, suffix_length) == 0;
}

bool ends_with(const_string s, const_string prefix)
{
    return _ends_with_cs(s, prefix);
}

bool ends_with(const_wstring s, const_wstring prefix)
{
    return _ends_with_cs(s, prefix);
}

bool ends_with(const string *s, const string *suffix)
{
    return _ends_with_cs(to_const_string(s), to_const_string(suffix));
}

bool ends_with(const wstring *s, const wstring *suffix)
{
    return _ends_with_cs(to_const_string(s), to_const_string(suffix));
}

#define DEFINE_INTEGER_BODY(T, NAME, FUNC, WIDEFUNC) \
    T NAME(const char    *s, char    **pos, int base) { return FUNC(s, pos, base); }\
    T NAME(const wchar_t *s, wchar_t **pos, int base) { return WIDEFUNC(s, pos, base); }\
    T NAME(const_string   s, char    **pos, int base) { return FUNC(s.c_str, pos, base); }\
    T NAME(const_wstring  s, wchar_t **pos, int base) { return WIDEFUNC(s.c_str, pos, base); }\
    T NAME(const string  *s, char    **pos, int base) { assert(s != nullptr); return FUNC(s->data.data, pos, base); }\
    T NAME(const wstring *s, wchar_t **pos, int base) { assert(s != nullptr); return WIDEFUNC(s->data.data, pos, base); }

#define DEFINE_DECIMAL_BODY(T, NAME, FUNC, WIDEFUNC) \
    T NAME(const char    *s, char    **pos) { return FUNC(s, pos); }\
    T NAME(const wchar_t *s, wchar_t **pos) { return WIDEFUNC(s, pos); }\
    T NAME(const_string   s, char    **pos) { return FUNC(s.c_str, pos); }\
    T NAME(const_wstring  s, wchar_t **pos) { return WIDEFUNC(s.c_str, pos); }\
    T NAME(const string  *s, char    **pos) { assert(s != nullptr); return FUNC(s->data.data, pos); }\
    T NAME(const wstring *s, wchar_t **pos) { assert(s != nullptr); return WIDEFUNC(s->data.data, pos); }

DEFINE_INTEGER_BODY(int, to_int, strtol, wcstol);
DEFINE_INTEGER_BODY(long, to_long, strtol, wcstol);
DEFINE_INTEGER_BODY(long long, to_long_long, strtoll, wcstoll);
DEFINE_INTEGER_BODY(unsigned int, to_unsigned_int, strtoul, wcstoul);
DEFINE_INTEGER_BODY(unsigned long, to_unsigned_long, strtoul, wcstoul);
DEFINE_INTEGER_BODY(unsigned long long, to_unsigned_long_long, strtoull, wcstoull);
DEFINE_DECIMAL_BODY(float, to_float, strtof, wcstof);
DEFINE_DECIMAL_BODY(double, to_double, strtod, wcstod);
DEFINE_DECIMAL_BODY(long double, to_long_double, strtold, wcstold);

char *copy_string(const char *src, char *dst)
{
    return strcpy(dst, src);
}

wchar_t *copy_string(const wchar_t *src, wchar_t *dst)
{
    return wcscpy(dst, src);
}

char *copy_string(const char *src, char *dst, u64 n)
{
    return strncpy(dst, src, n);
}

wchar_t *copy_string(const wchar_t *src, wchar_t *dst, u64 n)
{
    return wcsncpy(dst, src, n);
}

template<typename C>
void _copy_string_cs_s(const_string_base<C> src, string_base<C> *dst, u64 n)
{
    assert(dst != nullptr);

    bool append_null = false;

    if (n > src.size)
        n = src.size;

    if (n == 0)
        return;

    if (dst->data.reserved_size < n + 1)
    {
        reserve(&dst->data, n + 1);
        dst->data.size = n;
        append_null = true;
    }

    copy_memory(src.c_str, dst->data.data, sizeof(C) * n);

    if (append_null)
        dst->data.data[n] = '\0';
}

void copy_string(const char *src, string *dst)
{
    _copy_string_cs_s(to_const_string(src), dst, -1);
}

void copy_string(const wchar_t *src, wstring *dst)
{
    _copy_string_cs_s(to_const_string(src), dst, -1);
}

void copy_string(const char *src, string *dst, u64 n)
{
    _copy_string_cs_s(to_const_string(src), dst, n);
}

void copy_string(const wchar_t *src, wstring *dst, u64 n)
{
    _copy_string_cs_s(to_const_string(src), dst, n);
}

void copy_string(const_string src, string *dst)
{
    _copy_string_cs_s(src, dst, -1);
}

void copy_string(const_wstring src, wstring *dst)
{
    _copy_string_cs_s(src, dst, -1);
}

void copy_string(const_string src, string *dst, u64 n)
{
    _copy_string_cs_s(src, dst, n);
}

void copy_string(const_wstring src, wstring *dst, u64 n)
{
    _copy_string_cs_s(src, dst, n);
}

void copy_string(const string *src, string *dst)
{
    _copy_string_cs_s(to_const_string(src), dst, -1);
}

void copy_string(const wstring *src, wstring *dst)
{
    _copy_string_cs_s(to_const_string(src), dst, -1);
}

void copy_string(const string *src, string *dst, u64 n)
{
    _copy_string_cs_s(to_const_string(src), dst, n);
}

void copy_string(const wstring *src, wstring *dst, u64 n)
{
    _copy_string_cs_s(to_const_string(src), dst, n);
}

template<typename C>
void _append_string(string_base<C> *dst, const_string_base<C> other)
{
    assert(dst != nullptr);

    if (other.size == 0)
        return;

    u64 size_left = dst->data.reserved_size - dst->data.size;

    if (size_left < other.size + 1)
    {
        u64 required_space = dst->data.reserved_size + ((other.size + 1) - size_left);
        reserve(&dst->data, required_space);
    }

    copy_memory(other.c_str, dst->data.data + dst->data.size, sizeof(C) * other.size);

    dst->data.size += other.size;
    dst->data.data[dst->data.size] = '\0';
}

void append_string(string *dst, const char *other)
{
    _append_string(dst, to_const_string(other));
}

void append_string(wstring *dst, const wchar_t *other)
{
    _append_string(dst, to_const_string(other));
}

void append_string(string  *dst, const_string  other)
{
    _append_string(dst, other);
}

void append_string(wstring *dst, const_wstring other)
{
    _append_string(dst, other);
}

void append_string(string  *dst, const string  *other)
{
    _append_string(dst, to_const_string(other));
}

void append_string(wstring *dst, const wstring *other)
{
    _append_string(dst, to_const_string(other));
}

template<typename C>
void _prepend_string(string_base<C> *dst, const_string_base<C> other)
{
    assert(dst != nullptr);

    if (other.size == 0)
        return;

    u64 size_left = dst->data.reserved_size - dst->data.size;

    if (size_left < other.size + 1)
    {
        u64 required_space = dst->data.reserved_size + ((other.size + 1) - size_left);
        reserve(&dst->data, required_space);
    }

    move_memory(dst->data.data, dst->data.data + other.size, sizeof(C) * dst->data.size);
    copy_memory(other.c_str, dst->data.data, sizeof(C) * other.size);

    dst->data.size += other.size;
    dst->data.data[dst->data.size] = '\0';
}

void prepend_string(string *dst, const char *other)
{
    _prepend_string(dst, to_const_string(other));
}

void prepend_string(wstring *dst, const wchar_t *other)
{
    _prepend_string(dst, to_const_string(other));
}

void prepend_string(string  *dst, const_string  other)
{
    _prepend_string(dst, other);
}

void prepend_string(wstring *dst, const_wstring other)
{
    _prepend_string(dst, other);
}

void prepend_string(string  *dst, const string  *other)
{
    _prepend_string(dst, to_const_string(other));
}

void prepend_string(wstring *dst, const wstring *other)
{
    _prepend_string(dst, to_const_string(other));
}

s64 index_of(const_string  str, const char    *other)
{
    return index_of(str, to_const_string(other), 0);
}

s64 index_of(const_wstring str, const wchar_t *other)
{
    return index_of(str, to_const_string(other), 0);
}

s64 index_of(const_string  str, const_string  other)
{
    return index_of(str, other, 0);
}

s64 index_of(const_wstring str, const_wstring other)
{
    return index_of(str, other, 0);
}

s64 index_of(const_string  str, const string  *other)
{
    return index_of(str, to_const_string(other), 0);
}

s64 index_of(const_wstring str, const wstring *other)
{
    return index_of(str, to_const_string(other), 0);
}

s64 index_of(const string  *str, const char    *other)
{
    return index_of(to_const_string(str), to_const_string(other), 0);
}

s64 index_of(const wstring *str, const wchar_t *other)
{
    return index_of(to_const_string(str), to_const_string(other), 0);
}

s64 index_of(const string  *str, const_string  other)
{
    return index_of(to_const_string(str), other, 0);
}

s64 index_of(const wstring *str, const_wstring other)
{
    return index_of(to_const_string(str), other, 0);
}

s64 index_of(const string  *str, const string  *other)
{
    return index_of(to_const_string(str), to_const_string(other), 0);
}

s64 index_of(const wstring *str, const wstring *other)
{
    return index_of(to_const_string(str), to_const_string(other), 0);
}

template<typename C>
s64 _index_of(const_string_base<C> str, const_string_base<C> other, s64 offset)
{
    if (offset < 0)
        return -1;

    if (other.size == 0)
        return offset;

    if (offset >= str.size)
        return -1;

    if constexpr (sizeof(C) == 1)
    {
        // char
        void *begin = (void*)(str.c_str + offset);
        u64 size = str.size - offset;

        void *f = memmem(begin, size, other.c_str, other.size);

        if (f == nullptr)
            return -1;

        // casting to char ONLY for math
        u64 diff = (char*)f - (char*)str.c_str;
        return static_cast<s64>(diff);
    }
    else
    {
        void *begin = (void*)(str.c_str + offset);
        u64 size = (str.size - offset) * sizeof(C);

        void *f = memmem(begin, size, other.c_str, other.size * sizeof(C));

        if (f == nullptr)
            return -1;

        u64 diff = (char*)f - (char*)str.c_str;
        return static_cast<s64>(diff / sizeof(C));
    }
}

s64 index_of(const_string  str, const char    *other, s64 offset)
{
    return index_of(str, to_const_string(other), offset);
}

s64 index_of(const_wstring str, const wchar_t *other, s64 offset)
{
    return index_of(str, to_const_string(other), offset);
}

s64 index_of(const_string  str, const_string  other, s64 offset)
{
    return _index_of(str, other, offset);
}

s64 index_of(const_wstring str, const_wstring other, s64 offset)
{
    return _index_of(str, other, offset);
}

s64 index_of(const_string  str, const string  *other, s64 offset)
{
    return index_of(str, to_const_string(other), offset);
}

s64 index_of(const_wstring str, const wstring *other, s64 offset)
{
    return index_of(str, to_const_string(other), offset);
}

s64 index_of(const string  *str, const char    *other, s64 offset)
{
    return index_of(to_const_string(str), to_const_string(other), offset);
}

s64 index_of(const wstring *str, const wchar_t *other, s64 offset)
{
    return index_of(to_const_string(str), to_const_string(other), offset);
}

s64 index_of(const string  *str, const_string  other, s64 offset)
{
    return index_of(to_const_string(str), other, offset);
}

s64 index_of(const wstring *str, const_wstring other, s64 offset)
{
    return index_of(to_const_string(str), other, offset);
}

s64 index_of(const string  *str, const string  *other, s64 offset)
{
    return index_of(to_const_string(str), to_const_string(other), offset);
}

s64 index_of(const wstring *str, const wstring *other, s64 offset)
{
    return index_of(to_const_string(str), to_const_string(other), offset);
}

template<typename C>
void _trim_left(string_base<C> *s)
{
    assert(s != nullptr);

    u64 i = 0;
    u64 len = string_length(s);

    C c;
    while (i < len)
    {
        c = s->data.data[i];

        if (!(is_space(c) || c == '\0'))
            break;

        i++;
    }

    if (i > 0)
    {
        remove_elements(&s->data, 0, i);
        s->data.data[s->data.size] = '\0';
    }
}

void trim_left(string *s)
{
    _trim_left(s);
}

void trim_left(wstring *s)
{
    _trim_left(s);
}

template<typename C>
void _trim_right(string_base<C> *s)
{
    assert(s != nullptr);

    u64 len = string_length(s);
    s64 i = len;
    i -= 1;

    C c;
    while (i >= 0)
    {
        c = s->data.data[i];

        if (!(is_space(c) || c == '\0'))
            break;

        i--;
    }

    i++;
    if (i >= 0)
    {
        remove_elements(&s->data, i, len - i);
        s->data.data[s->data.size] = '\0';
    }
}

void trim_right(string *s)
{
    _trim_right(s);
}

void trim_right(wstring *s)
{
    _trim_right(s);
}

template<typename C>
inline void _trim(string_base<C> *s)
{
    _trim_left(s);
    _trim_right(s);
}

void trim(string *s)
{
    _trim(s);
}

void trim(wstring *s)
{
    _trim(s);
}

template<typename C>
void _substring(const C *s, u64 start, u64 length, C *out, u64 out_offset)
{
    copy_string(s + start, out + out_offset, length);
}

void substring(const char    *s, u64 start, u64 length, char    *out)
{
    _substring(s, start, length, out, 0);
}

void substring(const wchar_t *s, u64 start, u64 length, wchar_t *out)
{
    _substring(s, start, length, out, 0);
}

void substring(const char    *s, u64 start, u64 length, char    *out, u64 out_offset)
{
    _substring(s, start, length, out, out_offset);
}

void substring(const wchar_t *s, u64 start, u64 length, wchar_t *out, u64 out_offset)
{
    _substring(s, start, length, out, out_offset);
}

template<typename C>
void _substring_cs_s(const_string_base<C> s, u64 start, u64 length, string_base<C> *out, u64 out_start)
{
    assert(out != nullptr);

    if (length > s.size)
        length = s.size;

    if (length == 0)
        return;

    if (start >= s.size)
        return;

    u64 outlen = string_length(out);
    bool append_null = false;

    if (out_start + length >= outlen)
    {
        reserve(&out->data, out_start + length + 1);
        append_null = true;
    }

    copy_memory(s.c_str + start, out->data.data + out_start, sizeof(C) * length);

    if (out->data.size < out_start + length)
        out->data.size = out_start + length;

    if (append_null)
        out->data.data[out->data.size] = '\0';
}

void substring(const char    *s, u64 start, u64 length, string  *out)
{
    substring(s, start, length, out, 0);
}

void substring(const wchar_t *s, u64 start, u64 length, wstring *out)
{
    substring(s, start, length, out, 0);
}

void substring(const_string   s, u64 start, u64 length, string  *out)
{
    substring(s, start, length, out, 0);
}

void substring(const_wstring  s, u64 start, u64 length, wstring *out)
{
    substring(s, start, length, out, 0);
}

void substring(const string  *s, u64 start, u64 length, string  *out)
{
    substring(s, start, length, out, 0);
}

void substring(const wstring *s, u64 start, u64 length, wstring *out)
{
    substring(s, start, length, out, 0);
}

void substring(const char    *s, u64 start, u64 length, string  *out, u64 out_offset)
{
    substring(to_const_string(s), start, length, out, out_offset);
}

void substring(const wchar_t *s, u64 start, u64 length, wstring *out, u64 out_offset)
{
    substring(to_const_string(s), start, length, out, out_offset);
}

void substring(const_string   s, u64 start, u64 length, string  *out, u64 out_offset)
{
    _substring_cs_s(s, start, length, out, out_offset);
}

void substring(const_wstring  s, u64 start, u64 length, wstring *out, u64 out_offset)
{
    _substring_cs_s(s, start, length, out, out_offset);
}

void substring(const string  *s, u64 start, u64 length, string  *out, u64 out_offset)
{
    assert(s != nullptr);

    substring(to_const_string(s), start, length, out, out_offset);
}

void substring(const wstring *s, u64 start, u64 length, wstring *out, u64 out_offset)
{
    assert(s != nullptr);

    substring(to_const_string(s), start, length, out, out_offset);
}

char to_upper(char c)
{
    return toupper(static_cast<int>(c));
}

wchar_t to_upper(wchar_t c)
{
    return towupper(static_cast<wint_t>(c));
}

template<typename C>
inline void _to_upper(C *s)
{
    while (*s)
    {
        *s = to_upper(*s);
        s++;
    }
}

void to_upper(char *s)
{
    _to_upper(s);
}

void to_upper(wchar_t *s)
{
    _to_upper(s);
}

template<typename C>
inline void _to_upper_s(string_base<C> *s)
{
    assert(s != nullptr);

    for (u64 i = 0; i < s->data.size; ++i)
        s->data.data[i] = to_upper(s->data.data[i]);
}

void to_upper(string *s)
{
    _to_upper_s(s);
}

void to_upper(wstring *s)
{
    _to_upper_s(s);
}

char to_lower(char c)
{
    return tolower(static_cast<int>(c));
}

wchar_t to_lower(wchar_t c)
{
    return towlower(static_cast<wint_t>(c));
}

template<typename C>
inline void _to_lower(C *s)
{
    while (*s)
    {
        *s = to_lower(*s);
        s++;
    }
}

void to_lower(char *s)
{
    _to_lower(s);
}

void to_lower(wchar_t *s)
{
    _to_lower(s);
}

template<typename C>
inline void _to_lower_s(string_base<C> *s)
{
    assert(s != nullptr);

    u64 len = string_length(s);

    for (u64 i = 0; i < len; ++i)
        s->data.data[i] = to_lower(s->data.data[i]);
}

void to_lower(string *s)
{
    _to_lower_s(s);
}

void to_lower(wstring *s)
{
    _to_lower_s(s);
}

template<typename C>
hash_t _hash(const_string_base<C> str)
{
    return hash_data(str.c_str, str.size);
}

hash_t hash(const_string str)
{
    return _hash(str);
}

hash_t hash(const_wstring str)
{
    return _hash(str);
}

hash_t hash(const string *str)
{
    return _hash(to_const_string(str));
}

hash_t hash(const wstring *str)
{
    return _hash(to_const_string(str));
}
