
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>

#include "shl/assert.hpp"
#include "shl/memory.hpp"
#include "shl/type_functions.hpp"
#include "shl/compare.hpp"
#include "shl/defer.hpp"
#include "shl/platform.hpp"
#include "shl/environment.hpp"
#include "shl/string.hpp"

#define LIT(C, Literal)\
    inline_const_if(is_same(C, char), Literal, L##Literal)

#define as_array_ptr(C, str) (array<C>*)(str)

#if Windows
// https://stackoverflow.com/a/52989329
void* memmem(const void* haystack, size_t haystack_len,
    const void* const needle, const size_t needle_len)
{
    if (haystack == NULL) return NULL; // or assert(haystack != NULL);
    if (haystack_len == 0) return NULL;
    if (needle == NULL) return NULL; // or assert(needle != NULL);
    if (needle_len == 0) return NULL;

    for (const char* h = (const char*)haystack;
        haystack_len >= needle_len;
        ++h, --haystack_len) {
        if (!memcmp(h, needle, needle_len)) {
            return (void*)h;
        }
    }
    return NULL;
}

template<typename T>
T *stpncpy(T *dst, const T *src, s64 len)
{
    if (memcpy(dst, src, len * sizeof(T)) == nullptr)
        return nullptr;

    return dst + len;
}

wchar_t *wcpncpy(wchar_t *dst, const wchar_t *src, s64 len)
{
    return stpncpy<wchar_t>(dst, src, len);
}

template<typename T>
T *stpcpy(T *dst, const T *src)
{
    return stpncpy(dst, src, string_length(src));
}

wchar_t *wcpcpy(wchar_t *dst, const wchar_t *src)
{
    return stpcpy<wchar_t>(dst, src);
}
#endif

const_string  operator ""_cs(const char *str, u64 n)
{
    return const_string{str, (s64)n};
}

const_wstring operator ""_cs(const wchar_t *str, u64 n)
{
    return const_wstring{str, (s64)n};
}

string  operator ""_s(const char *str, u64 n)
{
    string ret;
    init(&ret, str, (s64)n);
    return ret;
}

wstring operator ""_s(const wchar_t *str, u64 n)
{
    wstring ret;
    init(&ret, str, (s64)n);
    return ret;
}

template<typename C>
void _init(string_base<C> *str)
{
    assert(str != nullptr);

    init(as_array_ptr(C, str));
}

template<typename C>
void _init(string_base<C> *str, s64 size)
{
    assert(str != nullptr);

    init(as_array_ptr(C, str), size + 1);
    str->size -= 1;
    str->data[size] = '\0';
}

template<typename C>
void _init(string_base<C> *str, const C *c)
{
    init(str, const_string_base<C>{c, string_length(c)});
}

template<typename C>
void _init(string_base<C> *str, const C *c, s64 size)
{
    assert(str != nullptr);

    init(str, const_string_base<C>{c, size});
}

template<typename C>
void _init(string_base<C> *str, const_string_base<C> cs)
{
    assert(str != nullptr);

    init(as_array_ptr(C, str), cs.size + 1);
    str->size -= 1;
    copy_string(cs, str);
    str->data[cs.size] = '\0';
}

void init(string *str)
{
    _init(str);
}

void init(string *str, s64 size)
{
    _init(str, size);
}

void init(string *str, const char *c)
{
    _init(str, c);
}

void init(string *str, const char *c, s64 size)
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

void init(wstring *str, s64 size)
{
    _init(str, size);
}

void init(wstring *str, const wchar_t *c)
{
    _init(str, c);
}

void init(wstring *str, const wchar_t *c, s64 size)
{
    _init(str, c, size);
}

void init(wstring *str, const_wstring s)
{
    _init(str, s);
}

template<typename C>
bool _string_reserve(string_base<C> *s, s64 size)
{
    // +1 for \0
    if (s->reserved_size < size + 1)
    {
        reserve_exp2(as_array_ptr(C, s), size + 1);
        return true;
    }

    return false;
}

bool string_reserve(string *s, s64 size)
{
    return _string_reserve(s, size);
}

bool string_reserve(wstring *s, s64 size)
{
    return _string_reserve(s, size);
}

void clear(string  *str)
{
    assert(str != nullptr);

    clear(as_array_ptr(string::value_type, str));
    str->data[0] = '\0';
}

void clear(wstring *str)
{
    assert(str != nullptr);

    clear(as_array_ptr(wstring::value_type, str));
    str->data[0] = L'\0';
}

void free(string  *str)
{
    assert(str != nullptr);

    free(as_array_ptr(string::value_type, str));
}

void free(wstring *str)
{
    assert(str != nullptr);

    free(as_array_ptr(wstring::value_type, str));
}

// string / char functions
bool is_space(char c)
{
    return (bool)isspace(c);
}

bool is_space(wchar_t c)
{
    return (bool)iswspace(static_cast<wint_t>(c));
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
    return (bool)isalpha(c);
}

bool is_alpha(wchar_t c)
{
    return (bool)iswalpha(static_cast<wint_t>(c));
}

bool is_digit(char c)
{
    return (bool)isdigit(c);
}

bool is_digit(wchar_t c)
{
    return (bool)iswdigit(static_cast<wint_t>(c));
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
    return (bool)isxdigit(c);
}

bool is_hex_digit(wchar_t c)
{
    return (bool)iswxdigit(static_cast<wint_t>(c));
}

bool is_alphanum(char c)
{
    return (bool)isalnum(c);
}

bool is_alphanum(wchar_t c)
{
    return (bool)iswalnum(static_cast<wint_t>(c));
}

bool is_upper(char c)
{
    return (bool)isupper(static_cast<int>(c));
}

bool is_upper(wchar_t c)
{
    return (bool)iswupper(static_cast<wint_t>(c));
}

bool is_lower(char c)
{
    return (bool)islower(static_cast<int>(c));
}

bool is_lower(wchar_t c)
{
    return (bool)iswlower(static_cast<wint_t>(c));
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
    s64 i = 0;
    s64 size = s.size;
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

s64 string_length(const char *s)
{
    if (s == nullptr)
        return 0;

    return strlen(s);
}

s64 string_length(const wchar_t *s)
{
    if (s == nullptr)
        return 0;

    return wcslen(s);
}

s64 string_length(const_string s)
{
    return s.size;
}

s64 string_length(const_wstring s)
{
    return s.size;
}

s64 string_length(const string *s)
{
    if (s == nullptr)
        return 0;

    return s->size;
}

s64 string_length(const wstring *s)
{
    if (s == nullptr)
        return 0;

    return s->size;
}

template<typename C>
int _compare_strings_cs(const_string_base<C> s1, const_string_base<C> s2, s64 n)
{
    int res = 0;
    s64 i = 0;

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

int _compare_strings(const_string   s1, const_string   s2, s64 n)
{
    return _compare_strings_cs(s1, s2, n);
}

int _compare_strings(const_wstring   s1, const_wstring   s2, s64 n)
{
    return _compare_strings_cs(s1, s2, n);
}

template<> bool equals(string  s1, string  s2)
{
    return compare_strings(&s1, &s2) == 0;
}

template<> bool equals(wstring s1, wstring s2)
{
    return compare_strings(&s1, &s2) == 0;
}

template<> bool equals_p(const string  *s1, const string  *s2)
{
    return compare_strings(s1, s2) == 0;
}

template<> bool equals_p(const wstring *s1, const wstring *s2)
{
    return compare_strings(s1, s2) == 0;
}

template<typename C>
bool _begins_with_cs(const_string_base<C> s, const_string_base<C> prefix)
{
    return compare_strings(prefix, s, string_length(prefix)) == 0;
}

bool _begins_with(const_string s, const_string prefix)
{
    return _begins_with_cs(s, prefix);
}

bool _begins_with(const_wstring s, const_wstring prefix)
{
    return _begins_with_cs(s, prefix);
}

template<typename C>
bool _ends_with_cs(const_string_base<C> s, const_string_base<C> suffix)
{
    s64 str_length = string_length(s);
    s64 suffix_length = string_length(suffix);

    if (str_length < suffix_length)
        return false;

    const_string_base<C> s2{s.c_str + str_length - suffix_length, suffix_length};

    return compare_strings(s2, suffix, suffix_length) == 0;
}

bool _ends_with(const_string s, const_string prefix)
{
    return _ends_with_cs(s, prefix);
}

bool _ends_with(const_wstring s, const_wstring prefix)
{
    return _ends_with_cs(s, prefix);
}

#define DEFINE_INTEGER_BODY(T, NAME, FUNC, WIDEFUNC) \
    T NAME(const char    *s, char    **pos, int base) { return FUNC(s, pos, base); }\
    T NAME(const wchar_t *s, wchar_t **pos, int base) { return WIDEFUNC(s, pos, base); }\
    T NAME(const_string   s, char    **pos, int base) { return FUNC(s.c_str, pos, base); }\
    T NAME(const_wstring  s, wchar_t **pos, int base) { return WIDEFUNC(s.c_str, pos, base); }\
    T NAME(const string  *s, char    **pos, int base) { assert(s != nullptr); return FUNC(s->data, pos, base); }\
    T NAME(const wstring *s, wchar_t **pos, int base) { assert(s != nullptr); return WIDEFUNC(s->data, pos, base); }

#define DEFINE_DECIMAL_BODY(T, NAME, FUNC, WIDEFUNC) \
    T NAME(const char    *s, char    **pos) { return FUNC(s, pos); }\
    T NAME(const wchar_t *s, wchar_t **pos) { return WIDEFUNC(s, pos); }\
    T NAME(const_string   s, char    **pos) { return FUNC(s.c_str, pos); }\
    T NAME(const_wstring  s, wchar_t **pos) { return WIDEFUNC(s.c_str, pos); }\
    T NAME(const string  *s, char    **pos) { assert(s != nullptr); return FUNC(s->data, pos); }\
    T NAME(const wstring *s, wchar_t **pos) { assert(s != nullptr); return WIDEFUNC(s->data, pos); }

DEFINE_INTEGER_BODY(int, to_int, strtol, wcstol);
DEFINE_INTEGER_BODY(long, to_long, strtol, wcstol);
DEFINE_INTEGER_BODY(long long, to_long_long, strtoll, wcstoll);
DEFINE_INTEGER_BODY(unsigned int, to_unsigned_int, strtoul, wcstoul);
DEFINE_INTEGER_BODY(unsigned long, to_unsigned_long, strtoul, wcstoul);
DEFINE_INTEGER_BODY(unsigned long long, to_unsigned_long_long, strtoull, wcstoull);
DEFINE_DECIMAL_BODY(float, to_float, strtof, wcstof);
DEFINE_DECIMAL_BODY(double, to_double, strtod, wcstod);
DEFINE_DECIMAL_BODY(long double, to_long_double, strtold, wcstold);

// manip
template<typename C>
void _set_string(string_base<C> *dst, const_string_base<C> src)
{
    assert(dst != nullptr);

    if (dst->reserved_size < src.size + 1)
        string_reserve(dst, src.size);

    dst->size = src.size;

    copy_memory(src.c_str, dst->data, sizeof(C) * src.size);

    dst->data[dst->size] = '\0';
}

void set_string(string  *dst, const char    *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(string  *dst, const char    *src, s64 n)
{
    set_string(dst, to_const_string(src, n));
}

void set_string(string  *dst, const_string   src)
{
    _set_string(dst, src);
}

void set_string(string  *dst, const string  *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(string  *dst, const wchar_t    *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(string  *dst, const wchar_t    *src, s64 n)
{
    set_string(dst, to_const_string(src, n));
}

void set_string(string  *dst, const_wstring   src)
{
    string_reserve(dst, src.size);
    ::fill_memory((void*)dst->data, 0, dst->reserved_size * sizeof(char));

    dst->size = ::wcstombs(dst->data, src.c_str, src.size * sizeof(wchar_t));
}

void set_string(string  *dst, const wstring  *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(wstring *dst, const wchar_t *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(wstring *dst, const wchar_t *src, s64 n)
{
    set_string(dst, to_const_string(src, n));
}

void set_string(wstring *dst, const_wstring  src)
{
    _set_string(dst, src);
}

void set_string(wstring *dst, const wstring *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(wstring *dst, const char *src)
{
    set_string(dst, to_const_string(src));
}

void set_string(wstring *dst, const char *src, s64 n)
{
    set_string(dst, to_const_string(src, n));
}

void set_string(wstring *dst, const_string  src)
{
    string_reserve(dst, src.size);
    ::fill_memory((void*)dst->data, 0, dst->reserved_size * sizeof(wchar_t));

    dst->size = ::mbstowcs(dst->data, src.c_str, src.size * sizeof(char));
}

void set_string(wstring *dst, const string *src)
{
    set_string(dst, to_const_string(src));
}

char *copy_string(const char *src, char *dst)
{
    return stpcpy(dst, src);
}

wchar_t *copy_string(const wchar_t *src, wchar_t *dst)
{
    return wcpcpy(dst, src);
}

char *copy_string(const char *src, char *dst, s64 n)
{
    return stpncpy(dst, src, n);
}

wchar_t *copy_string(const wchar_t *src, wchar_t *dst, s64 n)
{
    return wcpncpy(dst, src, n);
}

template<typename C>
void _copy_string_cs_s(const_string_base<C> src, string_base<C> *dst, s64 n, s64 dst_offset)
{
    assert(dst != nullptr);

    bool append_null = false;

    if (n > src.size)
        n = src.size;

    if (n == 0)
        return;

    s64 prev_size = string_length(dst);
    s64 size_needed = n + dst_offset;

    if (dst->reserved_size < size_needed + 1)
        string_reserve(dst, size_needed);

    if (prev_size < size_needed)
    {
        dst->size = size_needed;
        append_null = true;
    }

    copy_memory(src.c_str, dst->data + dst_offset, sizeof(C) * n);

    if (append_null)
        dst->data[dst->size] = '\0';
}

void _copy_string(const_string src, string *dst, s64 n, s64 dst_offset)
{
    _copy_string_cs_s(src, dst, n, dst_offset);
}

void _copy_string(const_wstring src, wstring *dst, s64 n, s64 dst_offset)
{
    _copy_string_cs_s(src, dst, n, dst_offset);
}

template<typename C>
string_base<C> _copy_new_string(const_string_base<C> src, s64 n)
{
    string_base<C> ret;
    init(&ret);
    _copy_string(src, &ret, n, 0);

    return ret;
}

string _copy_string(const_string  src, s64 n)
{
    return _copy_new_string<char>(src, n);
}

wstring _copy_string(const_wstring  src, s64 n)
{
    return _copy_new_string<wchar_t>(src, n);
}

template<typename C>
void _append_string_cs(string_base<C> *dst, const_string_base<C> other)
{
    assert(dst != nullptr);

    if (other.size == 0)
        return;

    s64 size_left = dst->reserved_size - dst->size;

    if (size_left < other.size + 1)
    {
        s64 required_space = dst->reserved_size + ((other.size + 1) - size_left);
        string_reserve(dst, required_space);
    }

    copy_memory(other.c_str, dst->data + dst->size, sizeof(C) * other.size);

    dst->size += other.size;
    dst->data[dst->size] = '\0';
}

void append_string(string  *dst, char c)
{
    _append_string_cs(dst, const_string{.c_str = (const char *)&c, .size = 1});
}

void append_string(wstring *dst, wchar_t c)
{
    _append_string_cs(dst, const_wstring{.c_str = (const wchar_t *)&c, .size = 1});
}

void _append_string(string  *dst, const_string  other)
{
    _append_string_cs(dst, other);
}

void _append_string(wstring *dst, const_wstring other)
{
    _append_string_cs(dst, other);
}

template<typename C>
void _prepend_string_cs(string_base<C> *dst, const_string_base<C> other)
{
    assert(dst != nullptr);

    if (other.size == 0)
        return;

    s64 size_left = dst->reserved_size - dst->size;

    if (size_left < other.size + 1)
    {
        s64 required_space = dst->reserved_size + ((other.size + 1) - size_left);
        string_reserve(dst, required_space);
    }

    move_memory(dst->data, dst->data + other.size, sizeof(C) * dst->size);
    copy_memory(other.c_str, dst->data, sizeof(C) * other.size);

    dst->size += other.size;
    dst->data[dst->size] = '\0';
}

void _prepend_string(string  *dst, const_string  other)
{
    _prepend_string_cs(dst, other);
}

void _prepend_string(wstring *dst, const_wstring other)
{
    _prepend_string_cs(dst, other);
}

template<typename C>
s64 _index_of_c(const_string_base<C> str, C needle, s64 offset)
{
    if (offset < 0)
        return -1;

    if (offset >= str.size)
        return -1;

    for (s64 i = offset; i < str.size; ++i)
        if (str.c_str[i] == needle)
            return i;

    return -1;
}

s64 _index_of(const_string str, char    needle, s64 offset)
{
    return _index_of_c(str, needle, offset);
}

s64 _index_of(const_wstring str, wchar_t needle, s64 offset)
{
    return _index_of_c(str, needle, offset);
}

template<typename C>
s64 _index_of_s(const_string_base<C> str, const_string_base<C> needle, s64 offset)
{
    if (offset < 0)
        return -1;

    if (needle.size == 0)
        return offset;

    if (offset >= str.size)
        return -1;

    if (needle.size > str.size)
        return -1;

    if constexpr (sizeof(C) == 1)
    {
        // char
        void *begin = (void*)(str.c_str + offset);
        s64 size = str.size - offset;

        void *f = memmem(begin, size, needle.c_str, needle.size);

        if (f == nullptr)
            return -1;

        // casting to char ONLY for math
        s64 diff = (char*)f - (char*)str.c_str;
        return static_cast<s64>(diff);
    }
    else
    {
        void *begin = (void*)(str.c_str + offset);
        s64 size = (str.size - offset) * sizeof(C);

        void *f = memmem(begin, size, needle.c_str, needle.size * sizeof(C));

        if (f == nullptr)
            return -1;

        s64 diff = (char*)f - (char*)str.c_str;
        return static_cast<s64>(diff / sizeof(C));
    }
}

s64 _index_of(const_string  str, const_string  needle, s64 offset)
{
    return _index_of_s(str, needle, offset);
}

s64 _index_of(const_wstring str, const_wstring needle, s64 offset)
{
    return _index_of_s(str, needle, offset);
}

template<typename C>
s64 _last_index_of_c(const_string_base<C> str, C needle, s64 offset)
{
    if (offset < 0)
        return -1;

    if (offset + 1 >= str.size)
        offset = str.size - 1;

    for (s64 i = offset; i >= 0; --i)
        if (str.c_str[i] == needle)
            return i;

    return -1;
}

s64 _last_index_of(const_string str, char    needle, s64 offset)
{
    return _last_index_of_c(str, needle, offset);
}

s64 _last_index_of(const_wstring str, wchar_t needle, s64 offset)
{
    return _last_index_of_c(str, needle, offset);
}

template<typename C>
s64 _last_index_of_s(const_string_base<C> str, const_string_base<C> needle, s64 offset)
{
    if (offset < 0)
        return -1;

    if (needle.size == 0)
        return offset;

    if (needle.size > str.size)
        return -1;

    if (offset + needle.size > str.size)
        offset = str.size - needle.size;

    s64 i = offset;
    s64 matching_count = 0;

    while (i >= 0)
    {
        while (str[i + matching_count] == needle[matching_count])
        {
            matching_count += 1;

            if (matching_count == needle.size)
                return i;
        }
        
        matching_count = 0;
        i -= 1;
    }

    return -1;
}

s64 _last_index_of(const_string  str, const_string  needle, s64 offset)
{
    return _last_index_of_s(str, needle, offset);
}

s64 _last_index_of(const_wstring str, const_wstring needle, s64 offset)
{
    return _last_index_of_s(str, needle, offset);
}

template<typename C>
void _trim_left(string_base<C> *s)
{
    assert(s != nullptr);

    s64 i = 0;
    s64 len = string_length(s);

    C c;
    while (i < len)
    {
        c = s->data[i];

        if (!(is_space(c) || c == '\0'))
            break;

        i++;
    }

    if (i > 0)
    {
        remove_elements(as_array_ptr(C, s), 0, i);
        s->data[s->size] = '\0';
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

    s64 len = string_length(s);
    s64 i = len;
    i -= 1;

    C c;
    while (i >= 0)
    {
        c = s->data[i];

        if (!(is_space(c) || c == '\0'))
            break;

        i--;
    }

    i++;
    if (i >= 0)
    {
        remove_elements(as_array_ptr(C, s), i, len - i);
        s->data[s->size] = '\0';
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

char to_upper(char c)
{
    return (char)toupper(static_cast<int>(c));
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

    for (s64 i = 0; i < s->size; ++i)
        s->data[i] = to_upper(s->data[i]);
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
    return (char)tolower(static_cast<int>(c));
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

    s64 len = string_length(s);

    for (s64 i = 0; i < len; ++i)
        s->data[i] = to_lower(s->data[i]);
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
const_string_base<C> _substring_nocopy(const_string_base<C> str, s64 start, s64 length)
{
    if (start >= str.size)
    {
        start = str.size;
        length = 0;
    }
    else if (length > str.size + start)
        length = str.size - start;

    return const_string_base<C>{.c_str = str.c_str + start, .size = length};
}

const_string  _substring(const_string  s, s64 start, s64 length)
{
    return _substring_nocopy(s, start, length);
}

const_wstring _substring(const_wstring s, s64 start, s64 length)
{
    return _substring_nocopy(s, start, length);
}

template<typename C>
void _substring_c_str(const C *s, s64 start, s64 length, C *out, s64 out_offset)
{
    copy_string(s + start, out + out_offset, length);
}

void substring(const char    *s, s64 start, s64 length, char    *out, s64 out_offset)
{
    _substring_c_str(s, start, length, out, out_offset);
}

void substring(const wchar_t *s, s64 start, s64 length, wchar_t *out, s64 out_offset)
{
    _substring_c_str(s, start, length, out, out_offset);
}

template<typename C>
void _substring_copy(const_string_base<C> s, s64 start, s64 length, string_base<C> *out, s64 out_offset)
{
    assert(out != nullptr);

    const_string_base<C> subs = _substring_nocopy(s, start, length);
    s64 offsize = out_offset + subs.size;
    bool append_null = false;

    if (offsize >= out->size)
    {
        string_reserve(out, offsize);
        append_null = true;
    }

    copy_memory(subs.c_str, out->data + out_offset, sizeof(C) * subs.size);

    if (out->size < offsize)
        out->size = offsize;

    if (append_null)
        out->data[out->size] = '\0';
}

void _substring(const_string  s, s64 start, s64 length, string *out,  s64 out_offset)
{
    _substring_copy(s, start, length, out, out_offset);
}

void _substring(const_wstring s, s64 start, s64 length, wstring *out, s64 out_offset)
{
    _substring_copy(s, start, length, out, out_offset);
}

template<typename C>
void _replace_c(string_base<C> *s, C needle, C replacement, s64 offset)
{
    s64 idx = index_of(s, needle, offset);

    if (idx < 0)
        return;

    s->data[idx] = replacement;
}

template<typename C>
void _replace_s(string_base<C> *s, const_string_base<C> needle, const_string_base<C> replacement, s64 offset)
{
    if (needle.size == 0)
        return;

    s64 idx = index_of(s, needle, offset);

    if (idx < 0)
        return;

    if (needle.size > replacement.size)
    {
        s64 size_diff = needle.size - replacement.size;
        s64 remaining_size = string_length(s) - idx - needle.size;

        s->size -= size_diff;

        move_memory(s->data + idx + needle.size, s->data + idx + replacement.size, sizeof(C) * remaining_size);
        s->data[s->size] = '\0';
    }
    else if (needle.size < replacement.size)
    {
        s64 size_diff = replacement.size - needle.size;
        s64 remaining_size = string_length(s) - idx - needle.size;

        string_reserve(s, string_length(s) + size_diff);
        s->size += size_diff;

        move_memory(s->data + idx + needle.size, s->data + idx + replacement.size, sizeof(C) * remaining_size);
        s->data[s->size] = '\0';
    }

    copy_string(replacement, s, max_value(s64), idx);
}

void replace(string *s, char needle, char replacement, s64 offset)
{
    _replace_c(s, needle, replacement, offset);
}

void replace(wstring *s, wchar_t needle, wchar_t replacement, s64 offset)
{
    _replace_c(s, needle, replacement, offset);
}

void _replace(string  *s, const_string  needle, const_string  replacement, s64 offset)
{
    _replace_s(s, needle, replacement, offset);
}

void _replace(wstring *s, const_wstring needle, const_wstring replacement, s64 offset)
{
    _replace_s(s, needle, replacement, offset);
}

template<typename C>
void _replace_all_c(string_base<C> *s, C needle, C replacement, s64 offset)
{
    s64 idx = index_of(s, needle, offset);

    while (idx >= 0)
    {
        s->data[idx] = replacement;
        idx = index_of(s, needle, idx + 1);
    }
}

template<typename C>
void _replace_all_s(string_base<C> *s, const_string_base<C> needle, const_string_base<C> replacement, s64 offset)
{
    if (needle.size == 0)
        return;

    s64 idx = index_of(s, needle, offset);

    while (idx >= 0)
    {
        if (needle.size > replacement.size)
        {
            s64 size_diff = needle.size - replacement.size;
            s64 remaining_size = string_length(s) - idx - needle.size;

            s->size -= size_diff;

            move_memory(s->data + idx + needle.size, s->data + idx + replacement.size, sizeof(C) * remaining_size);
            s->data[s->size] = '\0';
        }
        else if (needle.size < replacement.size)
        {
            s64 size_diff = replacement.size - needle.size;
            s64 remaining_size = string_length(s) - idx - needle.size;

            string_reserve(s, string_length(s) + size_diff);
            s->size += size_diff;

            move_memory(s->data + idx + needle.size, s->data + idx + replacement.size, sizeof(C) * remaining_size);
            s->data[s->size] = '\0';
        }

        copy_string(replacement, s, max_value(s64), idx);

        idx = index_of(s, needle, idx + 1);
    }
}

void replace_all(string *s, char needle, char replacement, s64 offset)
{
    _replace_all_c(s, needle, replacement, offset);
}

void replace_all(wstring *s, wchar_t needle, wchar_t replacement, s64 offset)
{
    _replace_all_c(s, needle, replacement, offset);
}

void _replace_all(string  *s, const_string  needle, const_string  replacement, s64 offset)
{
    _replace_all_s(s, needle, replacement, offset);
}

void _replace_all(wstring *s, const_wstring needle, const_wstring replacement, s64 offset)
{
    _replace_all_s(s, needle, replacement, offset);
}

template<typename C>
s64 _split_c(const_string_base<C> s, C delim, array<const_string_base<C>> *out)
{
    s64 split_count = 0;
    out->size = 0;

    s64 begin = 0;
    s64 end = index_of(s, delim);
    s64 diff = 0;

    const_string_base<C> to_add;

    while (end >= 0)
    {
        split_count++;
        diff = end - begin;

        if (diff <= 0)
        {
            to_add.c_str = LIT(C, "");
            to_add.size = 0;
        }
        else
        {
            to_add.c_str = s.c_str + begin;
            to_add.size = end - begin;
        }

        add_at_end(out, to_add);

        begin = end + 1;
        end = index_of(s, delim, end + 1);
    }

    end = s.size;
    diff = end - begin;

    if (diff <= 0)
    {
        to_add.c_str = LIT(C, "");
        to_add.size = 0;
    }
    else
    {
        to_add.c_str = s.c_str + begin;
        to_add.size = end - begin;
    }

    add_at_end(out, to_add);

    return split_count;
}

template<typename C>
s64 _split_s(const_string_base<C> s, const_string_base<C> delim, array<const_string_base<C>> *out)
{
    if (delim.size == 0)
        return 0;

    s64 split_count = 0;
    out->size = 0;

    s64 begin = 0;
    s64 end = index_of(s, delim);
    s64 diff = 0;

    const_string_base<C> to_add;

    while (end >= 0)
    {
        split_count++;
        diff = end - begin;

        if (diff <= 0)
        {
            to_add.c_str = LIT(C, "");
            to_add.size = 0;
        }
        else
        {
            to_add.c_str = s.c_str + begin;
            to_add.size = end - begin;
        }

        add_at_end(out, to_add);

        begin = end + delim.size;
        end = index_of(s, delim, end + delim.size);
    }

    end = s.size;
    diff = end - begin;

    if (diff <= 0)
    {
        to_add.c_str = LIT(C, "");
        to_add.size = 0;
    }
    else
    {
        to_add.c_str = s.c_str + begin;
        to_add.size = end - begin;
    }

    add_at_end(out, to_add);

    return split_count;
}

s64 _split(const_string  s, char          delim, array<const_string>  *out)
{
    return _split_c(s, delim, out);
}

s64 _split(const_wstring s, wchar_t       delim, array<const_wstring> *out)
{
    return _split_c(s, delim, out);
}

s64 _split(const_string  s, const_string  delim, array<const_string>  *out)
{
    return _split_s(s, delim, out);
}

s64 _split(const_wstring s, const_wstring delim, array<const_wstring> *out)
{
    return _split_s(s, delim, out);
}

template<typename C> inline const_string_base<C> _to_const_string(const C **s) { return to_const_string(*s); }
template<typename C> inline const_string_base<C> _to_const_string(const_string_base<C> *s) { return *s; }
template<typename C> inline const_string_base<C> _to_const_string(const string_base<C> *s) { return to_const_string(s); }
template<typename C> inline s64 _string_length(const C  **s) { return string_length(*s); }
template<typename C> inline s64 _string_length(const_string_base<C> *s) { return s->size; }
template<typename C> inline s64 _string_length(const string_base<C> *s) { return string_length(s); }

template<typename Str, typename C>
void _join_c(Str *strings, s64 count, C delim, string_base<C> *out)
{
    if (count == 0)
        return;

    s64 total_size = 0;
    s64 i = 0;

    total_size += count - 1; // delims

    for (; i < count ; ++i)
        total_size += _string_length(strings + i);

    string_reserve(out, total_size);

    s64 offset = 0;
    i = 0;

    const_string_base<C> s = _to_const_string(strings + i);
    
    if (s.size > 0)
        copy_memory(s.c_str, out->data + offset, sizeof(C) * s.size);

    offset += s.size;
    i++;

    while (i < count)
    {
        out->data[offset] = delim;
        offset++;

        s = _to_const_string(strings + i);

        if (s.size > 0)
            copy_memory(s.c_str, out->data + offset, sizeof(C) * s.size);

        offset += s.size;
        i++;
    }

    out->size = total_size;
    out->data[total_size] = 0;
}

template<typename Str, typename C>
void _join(Str *strings, s64 count, const_string_base<C> delim, string_base<C> *out)
{
    if (count == 0)
        return;

    s64 total_size = 0;
    s64 i = 0;

    total_size += (count - 1) * delim.size;

    for (; i < count ; ++i)
        total_size += _string_length(strings + i);

    string_reserve(out, total_size);

    s64 offset = 0;
    i = 0;

    const_string_base<C> s = _to_const_string(strings + i);
    
    if (s.size > 0)
        copy_memory(s.c_str, out->data + offset, sizeof(C) * s.size);

    offset += s.size;
    i++;

    while (i < count)
    {
        if (delim.size > 0)
            copy_memory(delim.c_str, out->data + offset, sizeof(C) * delim.size);

        offset += delim.size;
        s = _to_const_string(strings + i);

        if (s.size > 0)
            copy_memory(s.c_str, out->data + offset, sizeof(C) * s.size);

        offset += s.size;
        i++;
    }

    out->size = total_size;
    out->data[total_size] = 0;
}

void join(const char  **strings, s64 count, char          delim, string *out)
{
    _join_c(strings, count, delim, out);
}

void join(const char  **strings, s64 count, const char   *delim, string *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const char  **strings, s64 count, const_string  delim, string *out)
{
    _join(strings, count, delim, out);
}

void join(const char  **strings, s64 count, const string *delim, string *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const_string *strings, s64 count, char          delim, string *out)
{
    _join_c(strings, count, delim, out);
}

void join(const_string *strings, s64 count, const char   *delim, string *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const_string *strings, s64 count, const_string  delim, string *out)
{
    _join(strings, count, delim, out);
}

void join(const_string *strings, s64 count, const string *delim, string *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const string *strings, s64 count, char          delim, string *out)
{
    _join_c(strings, count, delim, out);
}

void join(const string *strings, s64 count, const char   *delim, string *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const string *strings, s64 count, const_string  delim, string *out)
{
    _join(strings, count, delim, out);
}

void join(const string *strings, s64 count, const string *delim, string *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const wchar_t  **strings, s64 count, wchar_t          delim, wstring *out)
{
    _join_c(strings, count, delim, out);
}

void join(const wchar_t  **strings, s64 count, const wchar_t   *delim, wstring *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const wchar_t  **strings, s64 count, const_wstring  delim, wstring *out)
{
    _join(strings, count, delim, out);
}

void join(const wchar_t  **strings, s64 count, const wstring *delim, wstring *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const_wstring *strings, s64 count, wchar_t          delim, wstring *out)
{
    _join_c(strings, count, delim, out);
}

void join(const_wstring *strings, s64 count, const wchar_t   *delim, wstring *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const_wstring *strings, s64 count, const_wstring  delim, wstring *out)
{
    _join(strings, count, delim, out);
}

void join(const_wstring *strings, s64 count, const wstring *delim, wstring *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const wstring *strings, s64 count, wchar_t          delim, wstring *out)
{
    _join_c(strings, count, delim, out);
}

void join(const wstring *strings, s64 count, const wchar_t   *delim, wstring *out)
{
    _join(strings, count, to_const_string(delim), out);
}

void join(const wstring *strings, s64 count, const_wstring  delim, wstring *out)
{
    _join(strings, count, delim, out);
}

void join(const wstring *strings, s64 count, const wstring *delim, wstring *out)
{
    _join(strings, count, to_const_string(delim), out);
}

static inline s64 _convert_str(char *dst, const wchar_t *src, s64 n)
{
    return wcstombs(dst, src, n);
}

static inline s64 _convert_str(wchar_t *dst, const char *src, s64 n)
{
    return mbstowcs(dst, src, n);
}

template<typename C>
static bool _get_converted_environment_variable(const C *name, s64 namesize, C **outbuf, s64 *outsize, sys_char **sysbuf, s64 *syssize)
{
    if constexpr (is_same(C, sys_char))
    {
        *outbuf = const_cast<C*>(get_environment_variable(name, namesize));
    }
    else
    {
        s64 char_count = _convert_str(nullptr, name, namesize);

        if (char_count == -1)
            return false;

        if (*syssize < Max((s64)64, char_count + 1))
        {
            s64 old_size = *syssize;
            *syssize = Max((s64)64, char_count + 1);
            *sysbuf = realloc_T<sys_char>(*sysbuf, old_size, *syssize);
            fill_memory((void*)*sysbuf, 0, (*syssize) * sizeof(sys_char));
        }

        _convert_str(*sysbuf, name, char_count);

        const sys_char *envvar = get_environment_variable(*sysbuf, namesize);

        if (envvar != nullptr)
        {
            char_count = _convert_str(nullptr, envvar, 0);

            if (char_count == -1)
                return false;

            if (*outsize < Max((s64)64, char_count + 1))
            {
                s64 old_size = *outsize;
                *outsize = Max((s64)64, char_count + 1);
                *outbuf = realloc_T<C>(*outbuf, old_size, *outsize);
                fill_memory((void*)*outbuf, 0, *outsize * sizeof(C));
            }

            _convert_str(*outbuf, envvar, char_count);
        }
        else
        {
            s64 old_size = *outsize;
            *outsize = Max((s64)64, *outsize);
            *outbuf = realloc_T<C>(*outbuf, old_size, *outsize);
            fill_memory((void*)*outbuf, 0, *outsize * sizeof(C));
        }
    }

    return true;
}

template<typename C>
static void _alias_environment_variables([[maybe_unused]] const_string_base<C> *str)
{
#if Windows
    if (*str == LIT(C, "HOME"_cs))
    {
        *str = LIT(C, "USERPROFILE"_cs);
    }
#endif
}

template<typename C>
static void _resolve_environment_variables(C *str, s64 _size, bool aliases)
{
    // this converts variables inside the string and environment variables
    // on the fly. I wonder if converting the entire string, resolving
    // variables and converting back performs better......
    s64 size = (s64)_size;
    s64 i = 0;

    C *_val = nullptr;
    s64 _val_buf_size = 0;
    sys_char *_sys_char_buf = nullptr;
    s64 _sys_char_buf_size = 0;

    defer {
        if constexpr (!is_same(C, sys_char))
        {
            if (_val != nullptr)
                dealloc_T<C>(_val, _val_buf_size);

            if (_sys_char_buf != nullptr)
                dealloc_T<sys_char>(_sys_char_buf, _sys_char_buf_size);
        }
    };

    while (i < size)
    {
        while (i + 1 < size && str[i] == '\\' && str[i + 1] == '$')
        {
            move_memory(str + i + 1, str + i, (size - i) * sizeof(C));
            str[size - 1] = '\0';

            i += 1;
        }

        if (i >= size)
            break;

        if (str[i] != '$')
        {
            i += 1;
            continue;
        }

        // start of variable
        s64 start = i + 1;
        s64 end = start;

        while ((end < size) && (is_alphanum(str[end]) || (str[end] == '_')))
            end += 1;

        const_string_base<C> varname{str + start, (s64)(end - start)};
        const_string_base<C> varname_to_resolve = varname;

        if (aliases)
            _alias_environment_variables(&varname_to_resolve);
        
        if (!_get_converted_environment_variable(varname_to_resolve.c_str, varname_to_resolve.size, &_val, &_val_buf_size, &_sys_char_buf, &_sys_char_buf_size))
            return;

        const_string_base<C> var{_val, string_length(_val)};

        s64 move_by = Min(size - end, (s64)var.size - ((s64)varname.size + 1));
        s64 mem_left = size - (end + move_by);

        if (move_by != 0)
        {
            move_memory(str + end, str + end + move_by, mem_left * sizeof(C));

            if (move_by < 0)
                fill_memory((void*)(str + size + move_by), 0, -move_by * sizeof(C));
        }

        mem_left = size - i;
        copy_memory(var.c_str, str + i, Min(mem_left, (s64)var.size) * sizeof(C));

        i += var.size;
    }
}

void resolve_environment_variables(char *str, s64 size, bool aliases)
{
    _resolve_environment_variables(str, size, aliases);
}

void resolve_environment_variables(wchar_t *str, s64 size, bool aliases)
{
    _resolve_environment_variables(str, size, aliases);
}

template<typename C>
static void _resolve_environment_variables_s(string_base<C> *str, bool aliases)
{
    s64 i = 0;

    C *_val = nullptr;
    s64 _val_buf_size = 0;
    sys_char *_sys_char_buf = nullptr;
    s64 _sys_char_buf_size = 0;

    defer {
        if constexpr (!is_same(C, sys_char))
        {
            if (_val != nullptr)
                dealloc_T<C>(_val, _val_buf_size);

            if (_sys_char_buf != nullptr)
                dealloc_T<sys_char>(_sys_char_buf, _sys_char_buf_size);
        }
    };

    while (i < str->size)
    {
        while (i + 1 < str->size && str->data[i] == '\\' && str->data[i + 1] == '$')
        {
            move_memory(str->data + i + 1, str->data + i, (str->size - i) * sizeof(C));
            str->size -= 1;
            str->data[str->size] = '\0';

            i += 1;
        }

        if (i >= str->size)
            break;

        if (str->data[i] != '$')
        {
            i += 1;
            continue;
        }

        // start of variable
        s64 start = i + 1;
        s64 end = start;

        while ((end < str->size) && (is_alphanum(str->data[end]) || (str->data[end] == '_')))
            end += 1;

        const_string_base<C> varname{str->data + start, (s64)(end - start)};
        const_string_base<C> varname_to_resolve = varname;

        if (aliases)
            _alias_environment_variables(&varname_to_resolve);
        
        if (!_get_converted_environment_variable(varname_to_resolve.c_str, varname_to_resolve.size, &_val, &_val_buf_size, &_sys_char_buf, &_sys_char_buf_size))
            return;

        const_string_base<C> var{_val, string_length(_val)};

        s64 move_by = (s64)var.size - ((s64)varname.size + 1);
        s64 mem_left = str->size - end;

        if (move_by != 0)
        {
            if (move_by > 0)
                string_reserve(str, str->size + move_by);

            move_memory(str->data + end, str->data + end + move_by, mem_left * sizeof(C));

            if (move_by < 0)
                fill_memory((void*)(str->data + str->size + move_by), 0, -move_by * sizeof(C));

            str->size += move_by;
        }

        copy_memory(var.c_str, str->data + i, var.size * sizeof(C));

        i += var.size;
    }
}

void resolve_environment_variables(string  *str, bool aliases)
{
    _resolve_environment_variables_s(str, aliases);
}

void resolve_environment_variables(wstring *str, bool aliases)
{
    _resolve_environment_variables_s(str, aliases);
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

template<typename C>
hash_t _hash(const const_string_base<C> *str)
{
    return hash_data(str->c_str, str->size);
}

hash_t hash(const const_string  *str)
{
    return _hash(str);
}

hash_t hash(const const_wstring *str)
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
