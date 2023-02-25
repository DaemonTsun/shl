
#include <string.h>
#include <stdlib.h>
#include <type_traits>
#include <cctype>
#include <cwctype>
#include <wchar.h>

#include "shl/string.hpp"

const_string  operator ""_cs(const char *str, u64 n)
{
    return const_string{str, n};
}

const_wstring operator ""_cs(const wchar_t *str, u64 n)
{
    return const_wstring{str, n};
}

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

template<typename CharT>
bool _is_blank(const CharT *s)
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

template<typename CharT>
bool _begins_with(const CharT *string, const CharT *prefix)
{
    return compare_strings(prefix, string, string_length(prefix)) == 0;
}

bool begins_with(const char *string, const char *prefix)
{
    return _begins_with(string, prefix);
}

bool begins_with(const wchar_t *string, const wchar_t *prefix)
{
    return _begins_with(string, prefix);
}

template<typename CharT>
bool _ends_with(const CharT *string, const CharT *suffix)
{
    u64 str_length = string_length(string);
    u64 suffix_length = string_length(suffix);

    return (str_length >= suffix_length) &&
           (compare_strings(string + str_length - suffix_length, suffix, suffix_length) == 0);
}

bool ends_with(const char *string, const char *prefix)
{
    return _ends_with(string, prefix);
}

bool ends_with(const wchar_t *string, const wchar_t *prefix)
{
    return _ends_with(string, prefix);
}

#define DEFINE_INTEGER_BODY(T, NAME, FUNC, WIDEFUNC) \
    T NAME(const char *value, char **pos, int base) { return FUNC(value, pos, base); }\
    T NAME(const wchar_t *value, wchar_t **pos, int base) { return WIDEFUNC(value, pos, base); }

#define DEFINE_DECIMAL_BODY(T, NAME, FUNC, WIDEFUNC) \
    T NAME(const char *value, char **pos) { return FUNC(value, pos); }\
    T NAME(const wchar_t *value, wchar_t **pos) { return WIDEFUNC(value, pos); }

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
