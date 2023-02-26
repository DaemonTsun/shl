
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <type_traits>
#include <cctype>
#include <cwctype>
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
    return std::isspace(c);
}

bool is_space(wchar_t c)
{
    return std::iswspace(static_cast<wint_t>(c));
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
    return std::isalpha(c);
}

bool is_alpha(wchar_t c)
{
    return std::iswalpha(static_cast<wint_t>(c));
}

bool is_digit(char c)
{
    return std::isdigit(c);
}

bool is_digit(wchar_t c)
{
    return std::iswdigit(static_cast<wint_t>(c));
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
    return std::isxdigit(c);
}

bool is_hex_digit(wchar_t c)
{
    return std::iswxdigit(static_cast<wint_t>(c));
}

bool is_alphanum(char c)
{
    return std::isalnum(c);
}

bool is_alphanum(wchar_t c)
{
    return std::iswalnum(static_cast<wint_t>(c));
}

bool is_upper(char c)
{
    return std::isupper(static_cast<int>(c));
}

bool is_upper(wchar_t c)
{
    return std::iswupper(static_cast<wint_t>(c));
}

bool is_lower(char c)
{
    return std::islower(static_cast<int>(c));
}

bool is_lower(wchar_t c)
{
    return std::iswlower(static_cast<wint_t>(c));
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

    while (i < size)
    {
        if (!is_space(s.c_str[i]))
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
    return _is_blank_cs(to_const_string(s));
}

bool is_blank(const wstring *s)
{
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
    if (s1.size != s2.size)
        return compare_ascending(s1.size, s2.size);

    int res = 0;
    u64 i = 0;

    if (n > s1.size)
        n = s1.size;

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

    return compare_strings(s2, suffix, suffix_length);
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

    if (dst->data.reserved_size < n + 1)
    {
        reserve(&dst->data, n + 1);
        dst->data.size = n;
        append_null = true;
    }

    copy_memory(src.c_str, dst->data.data, sizeof(C) * n);

    if (append_null)
        dst->data.data[src.size] = '\0';
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
