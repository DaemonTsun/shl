
// TODO: get rid of this
#include <stdlib.h>

#include "shl/assert.hpp"
#include "shl/memory.hpp"
#include "shl/type_functions.hpp"
#include "shl/compare.hpp"
#include "shl/defer.hpp"
#include "shl/compiler.hpp"
#include "shl/platform.hpp"
#include "shl/environment.hpp"
#include "shl/string_encoding.hpp"
#include "shl/string.hpp"

#define as_array_ptr(C, str) (array<C>*)(str)

#if GNU
#define memcmp __builtin_memcmp
#else
int memcmp(const void *s1, const void *s2, size_t n)
{
    const u8 *p1 = (const u8*)s1;
    const u8 *p2 = (const u8*)s2;

    while (n-- > 0)
    {
        if (*p1++ != *p2++)
            return p1[-1] < p2[-1] ? -1 : 1;
    }

    return 0;
}
#endif

// https://stackoverflow.com/a/52989329
static inline void* memmem(const void* haystack, size_t haystack_len,
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

#if Windows
template<typename T>
static inline T *stpncpy(T *dst, const T *src, s64 len)
{
    if (memcpy(dst, src, len * sizeof(T)) == nullptr)
        return nullptr;

    return dst + len;
}

template<typename T>
static inline T *stpcpy(T *dst, const T *src)
{
    return stpncpy(dst, src, string_length(src));
}
#endif

const_string operator ""_cs(const c8 *str, u64 n)
{
    return const_string{str, (s64)n};
}

const_u16string operator ""_cs(const c16 *str, u64 n)
{
    return const_u16string{str, (s64)n};
}

const_u32string operator ""_cs(const c32 *str, u64 n)
{
    return const_u32string{str, (s64)n};
}

template<typename C>
static inline void _init(string_base<C> *str)
{
    assert(str != nullptr);

    init(as_array_ptr(C, str));
}

template<typename C>
static inline void _init(string_base<C> *str, s64 size)
{
    assert(str != nullptr);

    init(as_array_ptr(C, str), size + 1);
    str->size -= 1;
    str->data[size] = (C)'\0';
}

template<typename C>
static inline void _init(string_base<C> *str, const C *c)
{
    init(str, const_string_base<C>{c, string_length(c)});
}

template<typename C>
static inline void _init(string_base<C> *str, const C *c, s64 size)
{
    assert(str != nullptr);

    init(str, const_string_base<C>{c, size});
}

template<typename C>
static inline void _init(string_base<C> *str, const_string_base<C> cs)
{
    assert(str != nullptr);

    init(as_array_ptr(C, str), cs.size + 1);
    str->size -= 1;
    string_copy(cs, str);
    str->data[cs.size] = (C)'\0';
}

void init(string *str)
{
    _init(str);
}

void init(string *str, s64 size)
{
    _init(str, size);
}

void init(string *str, const c8 *c)
{
    _init(str, c);
}

void init(string *str, const c8 *c, s64 size)
{
    _init(str, c, size);
}

void init(string *str, const_string s)
{
    _init(str, s);
}

void init(u16string *str)
{
    _init(str);
}

void init(u16string *str, s64 size)
{
    _init(str, size);
}

void init(u16string *str, const c16 *c)
{
    _init(str, c);
}

void init(u16string *str, const c16 *c, s64 size)
{
    _init(str, c, size);
}

void init(u16string *str, const_u16string s)
{
    _init(str, s);
}

void init(u32string *str)
{
    _init(str);
}

void init(u32string *str, s64 size)
{
    _init(str, size);
}

void init(u32string *str, const c32 *c)
{
    _init(str, c);
}

void init(u32string *str, const c32 *c, s64 size)
{
    _init(str, c, size);
}

void init(u32string *str, const_u32string s)
{
    _init(str, s);
}

void free(string  *str)
{
    assert(str != nullptr);

    free(as_array_ptr(string::value_type, str));
}

void free(u16string *str)
{
    assert(str != nullptr);

    free(as_array_ptr(u16string::value_type, str));
}

void free(u32string *str)
{
    assert(str != nullptr);

    free(as_array_ptr(u32string::value_type, str));
}

string  operator ""_s(const c8 *str, u64 n)
{
    string ret;
    init(&ret, str, (s64)n);
    return ret;
}

u16string  operator ""_s(const c16 *str, u64 n)
{
    u16string ret;
    init(&ret, str, (s64)n);
    return ret;
}

u32string  operator ""_s(const c32 *str, u64 n)
{
    u32string ret;
    init(&ret, str, (s64)n);
    return ret;
}

template<typename C>
static inline bool _string_reserve(string_base<C> *s, s64 size)
{
    assert(s != nullptr);
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

bool string_reserve(u16string *s, s64 size)
{
    return _string_reserve(s, size);
}

bool string_reserve(u32string *s, s64 size)
{
    return _string_reserve(s, size);
}

template<typename C>
static inline void _string_clear(string_base<C> *str)
{
    assert(str != nullptr);

    clear(as_array_ptr(C, str));

    if (str->data != nullptr)
        str->data[0] = (C)'\0';
}

void clear(string  *str)
{
    _string_clear(str);
}

void clear(u16string  *str)
{
    _string_clear(str);
}

void clear(u32string  *str)
{
    _string_clear(str);
}

// string / char functions
// TODO: these can probably be optimized, I hear something of a "trie" data structure
// or something, but as long as we don't run into performance hits, I don't really care.
bool is_space(u32 codepoint)
{
    if (codepoint > 0x00003000u)
        return false;

    if (codepoint >= 0x00000009u && codepoint <= 0x0000000Du)
        return true;

    if (codepoint == 0x00000020u) return true;
    if (codepoint == 0x00000085u) return true;
    if (codepoint == 0x000000A0u) return true;
    if (codepoint == 0x00001680u) return true;

    if (codepoint >= 0x00002000u && codepoint <= 0x0000200Au)
        return true;

    if (codepoint == 0x00002028u) return true;
    if (codepoint == 0x00002029u) return true;
    if (codepoint == 0x0000202Fu) return true;
    if (codepoint == 0x0000205Fu) return true;
    if (codepoint == 0x00003000u) return true;

    return false;
}

bool is_newline(u32 codepoint)
{
    if (codepoint > 0x00002029u)
        return false;

    if (codepoint >= 0x0000000Au && codepoint <= 0x0000000Du)
        return true;

    if (codepoint == 0x00000085u) return true;
    if (codepoint == 0x00002028u) return true;
    if (codepoint == 0x00002029u) return true;

    return false;
}

bool is_alpha(u32 codepoint)
{
    if (codepoint >= (u32)'A' && codepoint <= (u32)'Z') return true;
    if (codepoint >= (u32)'a' && codepoint <= (u32)'z') return true;
    // TODO: all others...
    return false;
}

bool is_digit(u32 codepoint)
{
    if (codepoint >= (u32)'0' && codepoint <= (u32)'9') return true;
    // TODO: all other numbers.......
    return false;
}

bool is_bin_digit(u32 codepoint)
{
    if (codepoint == (u32)'0' || codepoint == (u32)'1') return true;
    // TODO: all 0 and 1 variants...
    return false;
}

bool is_oct_digit(u32 codepoint)
{
    if (codepoint >= (u32)'0' && codepoint <= (u32)'7') return true;
    // TODO: all other number variants...
    return false;
}

bool is_hex_digit(u32 codepoint)
{
    if (codepoint >= (u32)'0' && codepoint <= (u32)'9') return true;
    if (codepoint >= (u32)'a' && codepoint <= (u32)'f') return true;
    if (codepoint >= (u32)'A' && codepoint <= (u32)'F') return true;
    // TODO: all other number and a-f variants...
    return false;
}

bool is_alphanum(u32 codepoint)
{
    return is_digit(codepoint) || is_alpha(codepoint);
}

bool is_upper(u32 codepoint)
{
    if (codepoint >= (u32)'A' && codepoint <= (u32)'Z') return true;
    // TODO: all alpha variants...
    return false;
}

bool is_lower(u32 codepoint)
{
    if (codepoint >= (u32)'a' && codepoint <= (u32)'z') return true;
    // TODO: all alpha variants...
    return false;
}

template<typename C>
static inline bool _string_is_null_or_empty_cs(const_string_base<C> s)
{
    return (s.c_str == nullptr) || (s.size == 0);
}

bool _string_is_null_or_empty(const_string    s) { return _string_is_null_or_empty_cs(s); }
bool _string_is_null_or_empty(const_u16string s) { return _string_is_null_or_empty_cs(s); }
bool _string_is_null_or_empty(const_u32string s) { return _string_is_null_or_empty_cs(s); }

template<typename C>
static inline bool _string_is_blank_cs(const_string_base<C> s)
{
    for_utf_string(cp, _, s)
        if (!is_space(cp) && cp != 0u)
            return false;

    return true;
}

bool _string_is_blank(const_string    s) { return _string_is_blank_cs(s); }
bool _string_is_blank(const_u16string s) { return _string_is_blank_cs(s); }
bool _string_is_blank(const_u32string s) { return _string_is_blank_cs(s); }

template<typename C>
s64 _string_length(const C *s)
{
    if (s == nullptr)
        return 0;

    s64 ret = 0;

    while (*s++ != (C)'\0')
        ret++;

    return ret;
}

s64 string_length(const c8  *s) { return _string_length(s); }
s64 string_length(const c16 *s) { return _string_length(s); }
s64 string_length(const c32 *s) { return _string_length(s); }
s64 string_length(const wchar_t *s) { return _string_length(char_cast(s)); }

s64 string_length(const_string s)    { return s.size; }
s64 string_length(const_u16string s) { return s.size; }
s64 string_length(const_u32string s) { return s.size; }

s64 string_length(const string *s)    { return s == nullptr ? 0 : s->size; }
s64 string_length(const u16string *s) { return s == nullptr ? 0 : s->size; }
s64 string_length(const u32string *s) { return s == nullptr ? 0 : s->size; }

s64 string_utf_length(const c8  *s) { return string_utf_length(to_const_string(s)); }
s64 string_utf_length(const c16 *s) { return string_utf_length(to_const_string(s)); }
s64 string_utf_length(const c32 *s) { return string_utf_length(to_const_string(s)); }

s64 string_utf_length(const_string s)    { return utf32_units_required_from_utf8 (s.c_str, s.size); }
s64 string_utf_length(const_u16string s) { return utf32_units_required_from_utf16(s.c_str, s.size); }
s64 string_utf_length(const_u32string s) { return s.size; }

s64 string_utf_length(const string *s)    { return s == nullptr ? 0 : string_utf_length(to_const_string(s)); }
s64 string_utf_length(const u16string *s) { return s == nullptr ? 0 : string_utf_length(to_const_string(s)); }
s64 string_utf_length(const u32string *s) { return s == nullptr ? 0 : string_utf_length(to_const_string(s)); }

template<typename C>
static inline int _string_compare_cs(const_string_base<C> s1, const_string_base<C> s2, s64 n)
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

int _string_compare(const_string s1, const_string s2, s64 n)
{
    return _string_compare_cs(s1, s2, n);
}

int _string_compare(const_u16string s1, const_u16string s2, s64 n)
{
    return _string_compare_cs(s1, s2, n);
}

int _string_compare(const_u32string s1, const_u32string s2, s64 n)
{
    return _string_compare_cs(s1, s2, n);
}

template<> bool equals(string s1, string s2)
{
    return string_compare(s1, s2) == 0;
}

template<> bool equals(u16string s1, u16string s2)
{
    return string_compare(s1, s2) == 0;
}

template<> bool equals(u32string s1, u32string s2)
{
    return string_compare(s1, s2) == 0;
}

template<> bool equals_p(const string *s1, const string *s2)
{
    return string_compare(s1, s2) == 0;
}

template<> bool equals_p(const u16string *s1, const u16string *s2)
{
    return string_compare(s1, s2) == 0;
}

template<> bool equals_p(const u32string *s1, const u32string *s2)
{
    return string_compare(s1, s2) == 0;
}

template<typename C>
static inline bool _string_begins_with_cs(const_string_base<C> s, const_string_base<C> prefix)
{
    return string_compare(prefix, s, string_length(prefix)) == 0;
}

bool _string_begins_with(const_string s, const_string prefix)
{
    return _string_begins_with_cs(s, prefix);
}

bool _string_begins_with(const_u16string s, const_u16string prefix)
{
    return _string_begins_with_cs(s, prefix);
}

bool _string_begins_with(const_u32string s, const_u32string prefix)
{
    return _string_begins_with_cs(s, prefix);
}

template<typename C>
static inline bool _string_ends_with_cs(const_string_base<C> s, const_string_base<C> suffix)
{
    s64 str_length = string_length(s);
    s64 suffix_length = string_length(suffix);

    if (str_length < suffix_length)
        return false;

    const_string_base<C> s2{s.c_str + str_length - suffix_length, suffix_length};

    return string_compare(s2, suffix, suffix_length) == 0;
}

bool _string_ends_with(const_string s, const_string prefix)
{
    return _string_ends_with_cs(s, prefix);
}

bool _string_ends_with(const_u16string s, const_u16string prefix)
{
    return _string_ends_with_cs(s, prefix);
}

bool _string_ends_with(const_u32string s, const_u32string prefix)
{
    return _string_ends_with_cs(s, prefix);
}

// TODO: u8/u16/u32/u64 functions instead
#define DEFINE_INTEGER_BODY(T, NAME, FUNC) \
    T NAME(const c8     *s, c8 **pos, int base) { return FUNC(s, pos, base); }\
    T NAME(const_string  s, c8 **pos, int base) { return FUNC(s.c_str, pos, base); }\
    T NAME(const string *s, c8 **pos, int base) { assert(s != nullptr); return FUNC(s->data, pos, base); }

#define DEFINE_DECIMAL_BODY(T, NAME, FUNC) \
    T NAME(const c8     *s, c8 **pos) { return FUNC(s, pos); }\
    T NAME(const_string  s, c8 **pos) { return FUNC(s.c_str, pos); }\
    T NAME(const string *s, c8 **pos) { assert(s != nullptr); return FUNC(s->data, pos); }

DEFINE_INTEGER_BODY(int, to_int, strtol);
DEFINE_INTEGER_BODY(long, to_long, strtol);
DEFINE_INTEGER_BODY(long long, to_long_long, strtoll);
DEFINE_INTEGER_BODY(unsigned int, to_unsigned_int, strtoul);
DEFINE_INTEGER_BODY(unsigned long, to_unsigned_long, strtoul);
DEFINE_INTEGER_BODY(unsigned long long, to_unsigned_long_long, strtoull);
DEFINE_DECIMAL_BODY(float, to_float, strtof);
DEFINE_DECIMAL_BODY(double, to_double, strtod);
DEFINE_DECIMAL_BODY(long double, to_long_double, strtold);

// manip
template<typename C>
static inline void _string_set_same_char(string_base<C> *dst, const_string_base<C> src)
{
    assert(dst != nullptr);

    if (dst->reserved_size < src.size + 1)
        string_reserve(dst, src.size);

    dst->size = src.size;

    copy_memory(src.c_str, dst->data, sizeof(C) * src.size);

    dst->data[dst->size] = '\0';
}

template<typename CTo, typename CFrom>
static inline void _string_set_different_char(string_base<CTo> *dst, const_string_base<CFrom> src)
{
    assert(dst != nullptr);

    s64 required_size = string_conversion_bytes_required(dst->data, src.c_str, src.size) / sizeof(CTo);

    if (dst->reserved_size < required_size + 1)
        string_reserve(dst, required_size + 1);

    dst->size = string_convert(src.c_str, src.size, dst->data, dst->reserved_size);
    dst->data[dst->size] = '\0';
}

void _string_set(string    *dst, const_string src)    { _string_set_same_char(dst, src); }
void _string_set(string    *dst, const_u16string src) { _string_set_different_char(dst, src); }
void _string_set(string    *dst, const_u32string src) { _string_set_different_char(dst, src); }
void _string_set(u16string *dst, const_string src)    { _string_set_different_char(dst, src); }
void _string_set(u16string *dst, const_u16string src) { _string_set_same_char(dst, src); }
void _string_set(u16string *dst, const_u32string src) { _string_set_different_char(dst, src); }
void _string_set(u32string *dst, const_string src)    { _string_set_different_char(dst, src); }
void _string_set(u32string *dst, const_u16string src) { _string_set_different_char(dst, src); }
void _string_set(u32string *dst, const_u32string src) { _string_set_same_char(dst, src); }

template<typename C>
static inline C *_string_copy(const C *src, C *dst, s64 n)
{
    assert(dst != nullptr);

    if (n < 0 || src == nullptr)
        return nullptr;

    copy_memory((const void*)src, (void*)dst, n * sizeof(C));

    return dst;
}

c8  *string_copy(const c8  *src, c8  *dst)        { return _string_copy(src, dst, string_length(src)); }
c8  *string_copy(const c8  *src, c8  *dst, s64 n) { return _string_copy(src, dst, n); }
c16 *string_copy(const c16 *src, c16 *dst)        { return _string_copy(src, dst, string_length(src)); }
c16 *string_copy(const c16 *src, c16 *dst, s64 n) { return _string_copy(src, dst, n); }
c32 *string_copy(const c32 *src, c32 *dst)        { return _string_copy(src, dst, string_length(src)); }
c32 *string_copy(const c32 *src, c32 *dst, s64 n) { return _string_copy(src, dst, n); }

template<typename C>
static inline void _string_copy_cs_s(const_string_base<C> src, string_base<C> *dst, s64 n, s64 dst_offset)
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

    copy_memory((const void*)src.c_str, (void*)(dst->data + dst_offset), sizeof(C) * n);

    if (append_null)
        dst->data[dst->size] = '\0';
}

void _string_copy(const_string src, string *dst, s64 n, s64 dst_offset)
{
    _string_copy_cs_s(src, dst, n, dst_offset);
}

void _string_copy(const_u16string src, u16string *dst, s64 n, s64 dst_offset)
{
    _string_copy_cs_s(src, dst, n, dst_offset);
}

void _string_copy(const_u32string src, u32string *dst, s64 n, s64 dst_offset)
{
    _string_copy_cs_s(src, dst, n, dst_offset);
}

template<typename C>
static inline string_base<C> _copy_new_string(const_string_base<C> src, s64 n)
{
    string_base<C> ret{};
    _string_copy(src, &ret, n, 0);

    return ret;
}

string _string_copy(const_string src, s64 n)
{
    return _copy_new_string(src, n);
}

u16string _string_copy(const_u16string src, s64 n)
{
    return _copy_new_string(src, n);
}

u32string _string_copy(const_u32string src, s64 n)
{
    return _copy_new_string(src, n);
}

template<typename C>
static inline void _string_append_cs(string_base<C> *dst, const_string_base<C> other)
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

    copy_memory((const void*)other.c_str, (void*)(dst->data + dst->size), sizeof(C) * other.size);

    dst->size += other.size;
    dst->data[dst->size] = '\0';
}

void _string_append(string    *dst, const_string    other)
{
    _string_append_cs(dst, other);
}

void _string_append(u16string *dst, const_u16string other)
{
    _string_append_cs(dst, other);
}

void _string_append(u32string *dst, const_u32string other)
{
    _string_append_cs(dst, other);
}

template<typename C>
static inline void _string_prepend_cs(string_base<C> *dst, const_string_base<C> other)
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

    move_memory((const void*)dst->data, (void*)(dst->data + other.size), sizeof(C) * dst->size);
    copy_memory((const void*)other.c_str, (void*)dst->data, sizeof(C) * other.size);

    dst->size += other.size;
    dst->data[dst->size] = '\0';
}

void _string_prepend(string    *dst, const_string    other)
{
    _string_prepend_cs(dst, other);
}

void _string_prepend(u16string *dst, const_u16string other)
{
    _string_prepend_cs(dst, other);
}

void _string_prepend(u32string *dst, const_u32string other)
{
    _string_prepend_cs(dst, other);
}

template<typename C>
static inline s64 _string_index_of_c(const_string_base<C> haystack, C needle, s64 offset)
{
    if (offset < 0)
        return -1;

    if (offset >= haystack.size)
        return -1;

    for (s64 i = offset; i < haystack.size; ++i)
        if (haystack.c_str[i] == needle)
            return i;

    return -1;
}

template<typename C>
static inline s64 _string_index_of_s(const_string_base<C> str, const_string_base<C> needle, s64 offset)
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

s64 _string_index_of(const_string    haystack, c8              needle, s64 offset)
{
    return _string_index_of_c(haystack, needle, offset);
}

s64 _string_index_of(const_string    haystack, const_string    needle, s64 offset)
{
    return _string_index_of_s(haystack, needle, offset);
}

s64 _string_index_of(const_u16string haystack, c16             needle, s64 offset)
{
    return _string_index_of_c(haystack, needle, offset);
}

s64 _string_index_of(const_u16string haystack, const_u16string needle, s64 offset)
{
    return _string_index_of_s(haystack, needle, offset);
}

s64 _string_index_of(const_u32string haystack, c32             needle, s64 offset)
{
    return _string_index_of_c(haystack, needle, offset);
}

s64 _string_index_of(const_u32string haystack, const_u32string needle, s64 offset)
{
    return _string_index_of_s(haystack, needle, offset);
}

template<typename C>
static inline s64 _string_last_index_of_c(const_string_base<C> haystack, C needle, s64 offset)
{
    // TODO: UTF
    if (offset < 0)
        return -1;

    if (offset + 1 >= haystack.size)
        offset = haystack.size - 1;

    for (s64 i = offset; i >= 0; --i)
        if (haystack.c_str[i] == needle)
            return i;

    return -1;
}

template<typename C>
static inline s64 _string_last_index_of_s(const_string_base<C> str, const_string_base<C> needle, s64 offset)
{
    // TODO: UTF
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

s64 _string_last_index_of(const_string    haystack, c8              needle, s64 offset)
{
    return _string_last_index_of_c(haystack, needle, offset);
}

s64 _string_last_index_of(const_string    haystack, const_string    needle, s64 offset)
{
    return _string_last_index_of_s(haystack, needle, offset);
}

s64 _string_last_index_of(const_u16string haystack, c16             needle, s64 offset)
{
    return _string_last_index_of_c(haystack, needle, offset);
}

s64 _string_last_index_of(const_u16string haystack, const_u16string needle, s64 offset)
{
    return _string_last_index_of_s(haystack, needle, offset);
}

s64 _string_last_index_of(const_u32string haystack, c32             needle, s64 offset)
{
    return _string_last_index_of_c(haystack, needle, offset);
}

s64 _string_last_index_of(const_u32string haystack, const_u32string needle, s64 offset)
{
    return _string_last_index_of_s(haystack, needle, offset);
}

template<typename C>
static inline bool _string_trim_left(string_base<C> *s)
{
    assert(s != nullptr);

    s64 i = 0;
    s64 len = string_length(s);

    // TODO: change to UTF
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
        return true;
    }

    return false;
}

bool string_trim_left(string    *s) { return _string_trim_left(s); }
bool string_trim_left(u16string *s) { return _string_trim_left(s); }
bool string_trim_left(u32string *s) { return _string_trim_left(s); }

template<typename C>
static inline bool _string_trim_right(string_base<C> *s)
{
    assert(s != nullptr);

    s64 len = string_length(s);
    s64 i = len;
    i -= 1;

    // TODO: change to UTF
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
        return true;
    }

    return false;
}

bool string_trim_right(string    *s) { return _string_trim_right(s); }
bool string_trim_right(u16string *s) { return _string_trim_right(s); }
bool string_trim_right(u32string *s) { return _string_trim_right(s); }

template<typename C>
static inline bool _string_trim(string_base<C> *s)
{
    bool ret = _string_trim_left(s);
    ret     |= _string_trim_right(s);
    return ret;
}

bool string_trim(string    *s) { return _string_trim(s); }
bool string_trim(u16string *s) { return _string_trim(s); }
bool string_trim(u32string *s) { return _string_trim(s); }

template<typename C>
static inline C _char_to_upper(C c)
{
    if (c >= (C)'a' && c <= (C)'z')
        return (c - (C)'a') + (C)'A';

    return c;
}

c8  char_to_upper(c8  c) { return _char_to_upper(c); }
c16 char_to_upper(c16 c) { return _char_to_upper(c); }
c32 char_to_upper(c32 c) { return _char_to_upper(c); }

void utf_codepoint_to_upper(c8  *codepoint)
{
    // TODO: implement
    (void)codepoint;
}

void utf_codepoint_to_upper(c16 *codepoint)
{
    // TODO: implement
    (void)codepoint;
}

void utf_codepoint_to_upper(c32 *codepoint)
{
    // TODO: implement
    (void)codepoint;
}

void utf_to_upper(c8  *s, s64 size)
{
    // TODO: implement
    (void)s;
    (void)size;
    // for_utf_codepoints(cp, {s, size})
    //     utf_codepoint_to_upper(cp);
}

void utf_to_upper(c16 *s, s64 size)
{
    // TODO: implement
    (void)s;
    (void)size;
    // for_utf_codepoints(cp, {s, size})
    //     utf_codepoint_to_upper(cp);
}
void utf_to_upper(c32 *s, s64 size)
{
    // TODO: implement
    (void)s;
    (void)size;
    // for_utf_codepoints(cp, {s, size})
    //     utf_codepoint_to_upper(cp);
}
void utf_to_upper(string    *s)
{
    // TODO: implement
    (void)s;
    // for_utf_codepoints(cp, s)
    //     utf_codepoint_to_upper(cp);
}
void utf_to_upper(u16string *s)
{
    // TODO: implement
    (void)s;
    // for_utf_codepoints(cp, s)
    //     utf_codepoint_to_upper(cp);
}
void utf_to_upper(u32string *s)
{
    // TODO: implement
    (void)s;
    // for_utf_codepoints(cp, s)
    //     utf_codepoint_to_upper(cp);
}


template<typename C>
static inline C _char_to_lower(C c)
{
    if (c >= (C)'A' && c <= (C)'Z')
        return (c - (C)'A') + (C)'a';

    return c;
}

c8  char_to_lower(c8  c) { return _char_to_lower(c); }
c16 char_to_lower(c16 c) { return _char_to_lower(c); }
c32 char_to_lower(c32 c) { return _char_to_lower(c); }

void utf_codepoint_to_lower(c8  *codepoint)
{
    // TODO: implement
    (void)codepoint;
}

void utf_codepoint_to_lower(c16 *codepoint)
{
    // TODO: implement
    (void)codepoint;
}

void utf_codepoint_to_lower(c32 *codepoint)
{
    // TODO: implement
    (void)codepoint;
}

void utf_to_lower(c8  *s, s64 size)
{
    // TODO: implement
    (void)s;
    (void)size;
    // for_utf_codepoints(cp, {s, size})
    //     utf_codepoint_to_lower(cp);
}

void utf_to_lower(c16 *s, s64 size)
{
    // TODO: implement
    (void)s;
    (void)size;
    // for_utf_codepoints(cp, {s, size})
    //     utf_codepoint_to_lower(cp);
}
void utf_to_lower(c32 *s, s64 size)
{
    // TODO: implement
    (void)s;
    (void)size;
    // for_utf_codepoints(cp, {s, size})
    //     utf_codepoint_to_lower(cp);
}
void utf_to_lower(string    *s)
{
    // TODO: implement
    (void)s;
    // for_utf_codepoints(cp, s)
    //     utf_codepoint_to_lower(cp);
}
void utf_to_lower(u16string *s)
{
    // TODO: implement
    (void)s;
    // for_utf_codepoints(cp, s)
    //     utf_codepoint_to_lower(cp);
}
void utf_to_lower(u32string *s)
{
    // TODO: implement
    (void)s;
    // for_utf_codepoints(cp, s)
    //     utf_codepoint_to_lower(cp);
}

template<typename C>
static inline const_string_base<C> _substring_nocopy(const_string_base<C> str, s64 start, s64 length)
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

const_string _substring(const_string s, s64 start, s64 length)
{
    return _substring_nocopy(s, start, length);
}

const_u16string _substring(const_u16string s, s64 start, s64 length)
{
    return _substring_nocopy(s, start, length);
}

const_u32string _substring(const_u32string s, s64 start, s64 length)
{
    return _substring_nocopy(s, start, length);
}

template<typename C>
static inline void _substring_c_str(const C *s, s64 start, s64 length, C *out, s64 out_offset)
{
    string_copy(s + start, out + out_offset, length);
}

void substring(const c8 *s, s64 start, s64 length, c8 *out, s64 out_offset)
{
    _substring_c_str(s, start, length, out, out_offset);
}

void substring(const c16 *s, s64 start, s64 length, c16 *out, s64 out_offset)
{
    _substring_c_str(s, start, length, out, out_offset);
}

void substring(const c32 *s, s64 start, s64 length, c32 *out, s64 out_offset)
{
    _substring_c_str(s, start, length, out, out_offset);
}

template<typename C>
static inline void _substring_copy(const_string_base<C> s, s64 start, s64 length, string_base<C> *out, s64 out_offset)
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

void _substring(const_string s, s64 start, s64 length, string *out,  s64 out_offset)
{
    _substring_copy(s, start, length, out, out_offset);
}

void _substring(const_u16string s, s64 start, s64 length, u16string *out, s64 out_offset)
{
    _substring_copy(s, start, length, out, out_offset);
}

void _substring(const_u32string s, s64 start, s64 length, u32string *out, s64 out_offset)
{
    _substring_copy(s, start, length, out, out_offset);
}

template<typename C>
static inline const_string_base<C> _utf_substring_nocopy(const_string_base<C> str, s64 start, s64 length)
{
    (void)str;
    (void)start;
    (void)length;
    // TODO: implement
    return const_string_base<C>{};
}

const_string _utf_substring(const_string s, s64 start, s64 length)
{
    return _utf_substring_nocopy(s, start, length);
}

const_u16string _utf_substring(const_u16string s, s64 start, s64 length)
{
    return _utf_substring_nocopy(s, start, length);
}

const_u32string _utf_substring(const_u32string s, s64 start, s64 length)
{
    return _utf_substring_nocopy(s, start, length);
}

template<typename C>
static inline void _utf_substring_c_str(const C *s, s64 start, s64 length, C *out, s64 out_offset)
{
    (void)s;
    (void)start;
    (void)length;
    (void)out;
    (void)out_offset;
    // TODO: implement
}

void utf_substring(const c8 *s, s64 start, s64 length, c8 *out, s64 out_offset)
{
    _utf_substring_c_str(s, start, length, out, out_offset);
}

void utf_substring(const c16 *s, s64 start, s64 length, c16 *out, s64 out_offset)
{
    _utf_substring_c_str(s, start, length, out, out_offset);
}

void utf_substring(const c32 *s, s64 start, s64 length, c32 *out, s64 out_offset)
{
    _utf_substring_c_str(s, start, length, out, out_offset);
}

template<typename C>
static inline void _utf_substring_copy(const_string_base<C> s, s64 start, s64 length, string_base<C> *out, s64 out_offset)
{
    // TODO: implement
    (void)s;
    (void)start;
    (void)length;
    (void)out;
    (void)out_offset;
}

void _utf_substring(const_string s, s64 start, s64 length, string *out,  s64 out_offset)
{
    _utf_substring_copy(s, start, length, out, out_offset);
}

void _utf_substring(const_u16string s, s64 start, s64 length, u16string *out, s64 out_offset)
{
    _utf_substring_copy(s, start, length, out, out_offset);
}

void _utf_substring(const_u32string s, s64 start, s64 length, u32string *out, s64 out_offset)
{
    _utf_substring_copy(s, start, length, out, out_offset);
}

template<typename C>
static inline void _replace_s(string_base<C> *s, const_string_base<C> needle, const_string_base<C> replacement, s64 offset)
{
    // TODO: UTF replace
    if (needle.size == 0)
        return;

    s64 idx = string_index_of(s, needle, offset);

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

    string_copy(replacement, s, max_value(s64), idx);
}

void _string_replace(string  *s, const_string  needle, const_string  replacement, s64 offset)
{
    _replace_s(s, needle, replacement, offset);
}

void _string_replace(u16string *s, const_u16string needle, const_u16string replacement, s64 offset)
{
    _replace_s(s, needle, replacement, offset);
}

void _string_replace(u32string *s, const_u32string needle, const_u32string replacement, s64 offset)
{
    _replace_s(s, needle, replacement, offset);
}

template<typename C>
static inline void _string_replace_all_s(string_base<C> *s, const_string_base<C> needle, const_string_base<C> replacement, s64 offset)
{
    // TODO: UTF replace
    if (needle.size == 0)
        return;

    s64 idx = string_index_of(s, needle, offset);

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

        string_copy(replacement, s, max_value(s64), idx);

        idx = string_index_of(s, needle, idx + 1);
    }
}

void _string_replace_all(string  *s, const_string  needle, const_string  replacement, s64 offset)
{
    _string_replace_all_s(s, needle, replacement, offset);
}

void _string_replace_all(u16string *s, const_u16string needle, const_u16string replacement, s64 offset)
{
    _string_replace_all_s(s, needle, replacement, offset);
}

void _string_replace_all(u32string *s, const_u32string needle, const_u32string replacement, s64 offset)
{
    _string_replace_all_s(s, needle, replacement, offset);
}

template<typename C>
static inline s64 _split_s(const_string_base<C> s, const_string_base<C> delim, array<const_string_base<C>> *out)
{
    // TODO: UTF split
    if (delim.size == 0)
        return 0;

    s64 split_count = 0;
    out->size = 0;

    s64 begin = 0;
    s64 end = string_index_of(s, delim);
    s64 diff = 0;

    const_string_base<C> to_add;

    while (end >= 0)
    {
        split_count++;
        diff = end - begin;

        if (diff <= 0)
        {
            to_add.c_str = string_literal(C, "");
            to_add.size = 0;
        }
        else
        {
            to_add.c_str = s.c_str + begin;
            to_add.size = end - begin;
        }

        add_at_end(out, to_add);

        begin = end + delim.size;
        end = string_index_of(s, delim, end + delim.size);
    }

    end = s.size;
    diff = end - begin;

    if (diff <= 0)
    {
        to_add.c_str = string_literal(C, "");
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


s64 _string_split(const_string    s, const_string    delim, array<const_string>    *out)
{
    return _split_s(s, delim, out);
}

s64 _string_split(const_u16string s, const_u16string delim, array<const_u16string> *out)
{
    return _split_s(s, delim, out);
}

s64 _string_split(const_u32string s, const_u32string delim, array<const_u32string> *out)
{
    return _split_s(s, delim, out);
}

template<typename C> auto _string_length_x(const C **str) { return string_length(*str); }
template<typename C> auto _string_length_x(const_string_base<C> *str) { return string_length(*str); }
template<typename C> auto _string_length_x(const string_base<C> *str) { return string_length(to_const_string(str)); }

template<typename Str, typename C>
static inline void _string_join_s(Str *strings, s64 count, const_string_base<C> delim, string_base<C> *out)
{
    // TODO: UTF join... wait, isn't this already compatible with UTF?
    if (count == 0)
        return;

    s64 total_size = 0;
    s64 i = 0;

    total_size += (count - 1) * delim.size;

    for (; i < count ; ++i)
        total_size += _string_length_x(strings+i);

    string_reserve(out, total_size);

    s64 offset = 0;
    i = 0;

    const_string_base<C> s = to_const_string(strings[i]);
    
    if (s.size > 0)
        copy_memory(s.c_str, out->data + offset, sizeof(C) * s.size);

    offset += s.size;
    i++;

    while (i < count)
    {
        if (delim.size > 0)
            copy_memory(delim.c_str, out->data + offset, sizeof(C) * delim.size);

        offset += delim.size;
        s = to_const_string(strings[i]);

        if (s.size > 0)
            copy_memory(s.c_str, out->data + offset, sizeof(C) * s.size);

        offset += s.size;
        i++;
    }

    out->size = total_size;
    out->data[total_size] = 0;
}

void _string_join(const c8     **strings, s64 count, const_string delim, string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const_string  *strings, s64 count, const_string delim, string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const string  *strings, s64 count, const_string delim, string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const c16       **strings, s64 count, const_u16string delim, u16string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const_u16string  *strings, s64 count, const_u16string delim, u16string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const u16string  *strings, s64 count, const_u16string delim, u16string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const c32       **strings, s64 count, const_u32string delim, u32string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const_u32string  *strings, s64 count, const_u32string delim, u32string *out)
{
    _string_join_s(strings, count, delim, out);
}

void _string_join(const u32string  *strings, s64 count, const_u32string delim, u32string *out)
{
    _string_join_s(strings, count, delim, out);
}

template<typename C>
static inline bool _get_converted_environment_variable(const C *name, s64 namesize, C **outbuf, s64 *outsize, sys_char **sysbuf, s64 *syssize)
{
    if constexpr (sizeof(C) == sizeof(sys_char))
    {
        *outbuf = const_cast<C*>(get_environment_variable(name, namesize));
    }
    else
    {
        s64 char_count = string_conversion_bytes_required((sys_char*)nullptr, name, namesize) / sizeof(sys_char);

        if (char_count < 0)
            return false;

        if (*syssize < Max((s64)64, char_count + 1))
        {
            s64 old_size = *syssize;
            *syssize = Max((s64)64, char_count + 1);
            *sysbuf = realloc_T<sys_char>(*sysbuf, old_size, *syssize);
            fill_memory((void*)*sysbuf, 0, (*syssize) * sizeof(sys_char));
        }

        string_convert(name, namesize, *sysbuf, *syssize);

        const sys_char *envvar = get_environment_variable(*sysbuf, namesize);
        s64 envvar_size = string_length(envvar);

        if (envvar != nullptr)
        {
            char_count = string_conversion_bytes_required((C*)nullptr, envvar, envvar_size) / sizeof(C);

            if (char_count < 0)
                return false;

            if (*outsize < Max((s64)64, char_count + 1))
            {
                s64 old_size = *outsize;
                *outsize = Max((s64)64, char_count + 1);
                *outbuf = realloc_T<C>(*outbuf, old_size, *outsize);
                fill_memory((void*)*outbuf, 0, *outsize * sizeof(C));
            }

            string_convert(envvar, envvar_size, *outbuf, *outsize);
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
static inline void _alias_environment_variables([[maybe_unused]] const_string_base<C> *str)
{
#if Windows
    if (*str == string_literal(C, "HOME"_cs))
    {
        *str = string_literal(C, "USERPROFILE"_cs);
    }
#endif
}

template<typename C>
static inline void _resolve_environment_variables(C *str, s64 _size, bool aliases)
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
        if constexpr (sizeof(C) != sizeof(sys_char))
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

void resolve_environment_variables(c8 *str, s64 size, bool aliases)
{
    _resolve_environment_variables(str, size, aliases);
}

void resolve_environment_variables(c16 *str, s64 size, bool aliases)
{
    _resolve_environment_variables(str, size, aliases);
}

void resolve_environment_variables(c32 *str, s64 size, bool aliases)
{
    _resolve_environment_variables(str, size, aliases);
}

template<typename C>
static inline void _resolve_environment_variables_s(string_base<C> *str, bool aliases)
{
    s64 i = 0;

    C *_val = nullptr;
    s64 _val_buf_size = 0;
    sys_char *_sys_char_buf = nullptr;
    s64 _sys_char_buf_size = 0;

    defer {
        if constexpr (sizeof(C) != sizeof(sys_char))
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

void resolve_environment_variables(u16string  *str, bool aliases)
{
    _resolve_environment_variables_s(str, aliases);
}

void resolve_environment_variables(u32string  *str, bool aliases)
{
    _resolve_environment_variables_s(str, aliases);
}

template<typename C>
static inline hash_t _hash(const_string_base<C> str)
{
    return hash_data(str.c_str, str.size);
}

hash_t hash(const_string    str) { return _hash(str); }
hash_t hash(const_u16string str) { return _hash(str); }
hash_t hash(const_u32string str) { return _hash(str); }
hash_t hash(const const_string    *str) { return _hash(*str); }
hash_t hash(const const_u16string *str) { return _hash(*str); }
hash_t hash(const const_u32string *str) { return _hash(*str); }
hash_t hash(const string    *str) { return _hash(to_const_string(str)); }
hash_t hash(const u16string *str) { return _hash(to_const_string(str)); }
hash_t hash(const u32string *str) { return _hash(to_const_string(str)); }
