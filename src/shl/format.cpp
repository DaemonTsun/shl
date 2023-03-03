
#include "shl/bits.hpp"
#include "shl/format.hpp"

template <bool Cond, typename T1, typename T2>
decltype(auto) _constexpr_if_eval(T1 &&a, T2 &&b)
{
    if constexpr (Cond)
        return forward<T1>(a);
    else
        return forward<T2>(b);
}

#define LIT(C, Literal)\
    _constexpr_if_eval<is_same(C, char)>(Literal##_cs, L##Literal##_cs)

template<typename C>
bool string_reserve(string_base<C> *s, u64 total_size)
{
    // +1 for \0
    if (s->data.reserved_size < total_size + 1)
    {
        reserve(&s->data, total_size + 1);
        return true;
    }

    return false;
}

template<typename C>
u64 string_pad(string_base<C> *s, C chr, s64 count, u64 offset = 0)
{
    string_reserve(s, count + offset);

    for (s64 i = 0; i < count; ++i)
        s->data.data[i + offset] = chr;

    return count + offset;
}

template<typename C>
s64 _bool_to_string(string_base<C> *s, bool x, u64 offset, bool as_text)
{
    u64 sz = as_text ? 5 : 1;
    sz += offset;

    string_reserve(s, sz);

    if (as_text)
    {
        if (x)
        {
            copy_string(LIT(C, "true"), s, 4, offset);

            return 4;
        }
        else
        {
            copy_string(LIT(C, "false"), s, 5, offset);

            return 5;
        }
    }
    else
    {
        s->data.data[offset] = x ? '1' : '0';
        return 1;
    }
}

// this basically just calls F and sets the size and null terminating
// character of String accordingly.
#define to_string_body(F, String, Value, Offset, ...)\
{\
    u64 sz = String->data.size;\
    u64 n = F(String, Value, Offset __VA_OPT__(,) __VA_ARGS__);\
\
    if (n + Offset >= sz)\
    {\
        String->data.size = n + Offset;\
        String->data.data[String->data.size] = '\0';\
    }\
\
    return n;\
}

s64 to_string(string *s, bool x)
    to_string_body(_bool_to_string, s, x, 0, false)

s64 to_string(string *s, bool x, u64 offset)
    to_string_body(_bool_to_string, s, x, offset, false)

s64 to_string(string *s, bool x, u64 offset, bool as_text)
    to_string_body(_bool_to_string, s, x, offset, as_text)

s64 to_string(wstring *s, bool x)
    to_string_body(_bool_to_string, s, x, 0, false)

s64 to_string(wstring *s, bool x, u64 offset)
    to_string_body(_bool_to_string, s, x, offset, false)

s64 to_string(wstring *s, bool x, u64 offset, bool as_text)
    to_string_body(_bool_to_string, s, x, offset, as_text)

template<typename C>
s64 _char_to_string(string_base<C> *s, C x, u64 offset)
{
    u64 sz = 1;
    sz += offset;

    string_reserve(s, sz);
    s->data.data[offset] = x;

    return 1;
}

s64 to_string(string  *s, char x)
    to_string_body(_char_to_string, s, x, 0)

s64 to_string(string  *s, char x, u64 offset)
    to_string_body(_char_to_string, s, x, offset)

s64 to_string(wstring *s, wchar_t x)
    to_string_body(_char_to_string, s, x, 0)

s64 to_string(wstring *s, wchar_t x, u64 offset)
    to_string_body(_char_to_string, s, x, offset)

template<u8 Pow, typename C, typename N>
s64 _c_unsigned_pow2_to_string(C *s, N x)
{
    static_assert(Pow <= 16);

    static const char *hex_letters = "0123456789abcdef";

    int i = 0;

    if (x == 0)
    {
        s[i++] = '0';
        return i;
    }

    constexpr u64 Shift = log2(Pow);
    constexpr u64 mask = bitmask<u64>(0, Shift - 1);

    s64 places = 0;

    while (bitmask<u64>(0, (Shift * places) - 1) < x)
        places++;

    places--;

    while (places >= 0)
    {
        s[i++] = hex_letters[(x >> (places * Shift)) & mask];
        places--;
    }

    return i;
}

template<typename C, typename N>
s64 _c_unsigned_decimal_to_string(C *s, N x)
{
    int i = 0;

    if (x == 0)
    {
        s[i++] = '0';
        return i;
    }

    while (x > 0)
    {
        s[i++] = '0' + x % 10;
        x = x / 10;
    }

    return i;
}

template<typename C, typename N>
s64 _unsigned_number_to_string(string_base<C> *s, N x, u64 offset, int base, bool include_prefix, int pad_length, C pad_char)
{
    C buf[64];
    s64 buf_size = -1;

    switch (base)
    {
    case 2:  buf_size = _c_unsigned_pow2_to_string<2>(buf, x); break;
    case 8:  buf_size = _c_unsigned_pow2_to_string<8>(buf, x); break;
    case 10: buf_size = _c_unsigned_decimal_to_string(buf, x); break;
    case 16: buf_size = _c_unsigned_pow2_to_string<16>(buf, x); break;
    }

    if (buf_size < 0)
        return -1;

    if (buf_size == 0)
        return 0;

    u64 sz = buf_size + offset;

    string_reserve(s, sz);

    copy_string(buf, s->data.data + offset, buf_size);

    return buf_size;
}

s64 to_string(string  *s, u8 x)
    to_string_body(_unsigned_number_to_string, s, x, 0, 10, false, 0, '0');
s64 to_string(string  *s, u8 x, u64 offset)
    to_string_body(_unsigned_number_to_string, s, x, offset, 10, false, 0, '0');
s64 to_string(string  *s, u8 x, u64 offset, int base)
    to_string_body(_unsigned_number_to_string, s, x, offset, base, false, 0, '0');
s64 to_string(string  *s, u8 x, u64 offset, int base, bool include_prefix, int pad_length, char pad_char)
    to_string_body(_unsigned_number_to_string, s, x, offset, base, include_prefix, pad_length, pad_char);
