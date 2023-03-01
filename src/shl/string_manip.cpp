
#include <cctype>
#include <cwctype>

#include "shl/string.hpp"
#include "shl/string_manip.hpp"


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

