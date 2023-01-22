
#include <cctype>
#include <cwctype>

#include "shl/string.hpp"
#include "shl/string_manip.hpp"

char to_upper(char c)
{
    return std::toupper(static_cast<int>(c));
}

wchar_t to_upper(wchar_t c)
{
    return std::towupper(static_cast<wint_t>(c));
}

template<typename CharT>
inline void _to_upper(CharT *s)
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

char to_lower(char c)
{
    return std::tolower(static_cast<int>(c));
}

wchar_t to_lower(wchar_t c)
{
    return std::towlower(static_cast<wint_t>(c));
}

template<typename CharT>
inline void _to_lower(CharT *s)
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

template<typename CharT>
void _trim_left(std::basic_string<CharT> *s)
{
    size_t i = 0;

    while (::is_space(s->data()[i]))
        i++;

    s->erase(0, i);
}

void trim_left(std::basic_string<char> *s)
{
    _trim_left(s);
}

void trim_left(std::basic_string<wchar_t> *s)
{
    _trim_left(s);
}

template<typename CharT>
void _trim_right(std::basic_string<CharT> *s)
{
    size_t i = s->size() - 1;

    while (i > 0 && ::is_space(s->data()[i]))
        i--;

    s->erase(i+1);
}

void trim_right(std::basic_string<char> *s)
{
    _trim_right(s);
}

void trim_right(std::basic_string<wchar_t> *s)
{
    _trim_right(s);
}

template<typename CharT>
void _trim(std::basic_string<CharT> *s)
{
    _trim_left(s);
    _trim_right(s);
}

void trim(std::basic_string<char> *s)
{
    _trim(s);
}

void trim(std::basic_string<wchar_t> *s)
{
    _trim(s);
}

template<typename CharT>
void _substring(const CharT *s, size_t start, size_t length, CharT *out)
{
    copy_string(s + start, out, length);
}

template<typename CharT>
void _substring_s(const CharT *s, size_t start, size_t length, std::basic_string<CharT> *out)
{
    out->resize(length);
    _substring(s, start, length, out->data());
}

void substring(const char *s, size_t start, size_t length, char *out)
{
    _substring(s, start, length, out);
}

void substring(const char *s, size_t start, size_t length, std::basic_string<char> *out)
{
    _substring_s(s, start, length, out);
}

void substring(const wchar_t *s, size_t start, size_t length, wchar_t *out)
{
    _substring(s, start, length, out);
}

void substring(const wchar_t *s, size_t start, size_t length, std::basic_string<wchar_t> *out)
{
    _substring_s(s, start, length, out);
}

