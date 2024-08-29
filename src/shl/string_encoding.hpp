
/* string_encoding.hpp

Handle different string encodings.
u32 is used as the type for Unicode codepoints.

REMINDER:

u8"abc"  ->  utf8  string
 u"abc"  ->  utf16 string
 U"abc"  ->  utf32 string... isn't C++ amazing

TODO: docs
*/

#pragma once

#include "shl/number_types.hpp"
#include "shl/type_functions.hpp"

typedef char     c8;
typedef char16_t c16;
typedef char32_t c32;

using wc_utf_type = if_type((sizeof(wchar_t) == sizeof(c16)), c16, c32);

#define UNICODE_MAX 0x10ffff

#define UTF8_1_MAX  0x7f
#define UTF8_2_MAX  0x7ff
#define UTF8_3_MAX  0xffff
#define UTF8_4_MAX  0x10ffff
#define UTF16_1_MAX 0xffff

#define UTF16_SURROGATE_HIGH_LOW_MASK   0xfc00
#define UTF16_SURROGATE_MASK            0xf800
#define UTF16_SURROGATE_HIGH            0xd800
#define UTF16_SURROGATE_LOW             0xdc00
#define UTF16_CODEPOINT_OFFSET          0x10000
#define UTF16_CODEPOINT_MASK            0x03ff

extern "C"
{
const c8  *utf8_decode (const c8  *str, u32 *cp, int *error);
const c16 *utf16_decode(const c16 *str, u32 *cp, int *error);
const c32 *utf32_decode(const c32 *str, u32 *cp, int *error);

// u8str/u16str must be zero padded to a multiple of four bytes.
// str_buf_size is the total number of elements in the string, regardless
// of encoding or code points.
// Returns the number of codepoints (u32) written to out.
s64 utf8_decode_string (const c8  *u8str,  s64 u8str_size,  u32 *out, s64 out_size);
s64 utf16_decode_string(const c16 *u16str, s64 u16str_size, u32 *out, s64 out_size);
s64 utf32_decode_string(const c32 *u32str, s64 u32str_size, u32 *out, s64 out_size);

// Same as above, but u8str/u16str don't have to be zero padded because
// string length is checked.
s64 utf8_decode_string_safe (const c8  *u8str,  s64 u8str_size,  u32 *out, s64 out_size);
s64 utf16_decode_string_safe(const c16 *u16str, s64 u16str_size, u32 *out, s64 out_size);
s64 utf32_decode_string_safe(const c32 *u32str, s64 u32str_size, u32 *out, s64 out_size);

// returns the number of bytes written, or -1 on error.
// out must have space for 4 bytes.
s64 utf8_encode (u32 cp, c8  *out);
s64 utf16_encode(u32 cp, c16 *out);
s64 utf32_encode(u32 cp, c32 *out);

// encode CODEPOINTS to utf8/utf16/utf32
s64 utf8_encode_string (const u32 *cps, s64 cp_count, c8  *out, s64 out_size);
s64 utf16_encode_string(const u32 *cps, s64 cp_count, c16 *out, s64 out_size);
s64 utf32_encode_string(const u32 *cps, s64 cp_count, c32 *out, s64 out_size);

// returns the number of elements written such that
// out + return value = directly after written content.
s64 utf8_to_utf16 (const c8  *u8str,  s64 u8str_size,  c16 *out, s64 out_size);
s64 utf8_to_utf32 (const c8  *u8str,  s64 u8str_size,  c32 *out, s64 out_size);
s64 utf16_to_utf8 (const c16 *u16str, s64 u16str_size, c8  *out, s64 out_size);
s64 utf16_to_utf32(const c16 *u16str, s64 u16str_size, c32 *out, s64 out_size);
s64 utf32_to_utf8 (const c32 *u32str, s64 u32str_size, c8  *out, s64 out_size);
s64 utf32_to_utf16(const c32 *u32str, s64 u32str_size, c16 *out, s64 out_size);

s64 utf8_units_required_from_utf16 (const c16 *u16str, s64 u16str_size);
s64 utf8_units_required_from_utf32 (const c32 *u32str, s64 u32str_size);
s64 utf16_units_required_from_utf8 (const c8  *u8str,  s64 u8str_size);
s64 utf16_units_required_from_utf32(const c32 *u32str, s64 u32str_size);
s64 utf32_units_required_from_utf8 (const c8  *u8str,  s64 u8str_size);
s64 utf32_units_required_from_utf16(const c16 *u16str, s64 u16str_size);

s64 utf8_bytes_required_from_utf16 (const c16 *u16str, s64 u16str_size);
s64 utf8_bytes_required_from_utf32 (const c32 *u32str, s64 u32str_size);
s64 utf16_bytes_required_from_utf8 (const c8  *u8str,  s64 u8str_size);
s64 utf16_bytes_required_from_utf32(const c32 *u32str, s64 u32str_size);
s64 utf32_bytes_required_from_utf8 (const c8  *u8str,  s64 u8str_size);
s64 utf32_bytes_required_from_utf16(const c16 *u16str, s64 u16str_size);

// length (bytes) of a single unicode codepoint if it were encoded as utf8/utf16
static inline s32 codepoint_utf8_length (u32 cp)
{
    if      (cp <= UTF8_1_MAX)  return 1;
    else if (cp <= UTF8_2_MAX)  return 2;
    else if (cp <= UTF8_3_MAX)  return 3;
    else                        return 4;
}

static inline s32 codepoint_utf16_length(u32 cp)
{
    if (cp <= UTF16_1_MAX)  return 1;
    else                    return 2;
}

static inline s32 codepoint_utf32_length(u32 cp)
{
    (void)cp;
    return 1;
}

s64 utf8_bytes_required_from_codepoints (const u32 *cps, s64 cp_count);
s64 utf16_bytes_required_from_codepoints(const u32 *cps, s64 cp_count);
s64 utf32_bytes_required_from_codepoints(const u32 *cps, s64 cp_count);
}

s64 string_convert(const c8  *u8str,  s64 u8str_size,  c16 *u16str, s64 u16str_size);
s64 string_convert(const c8  *u8str,  s64 u8str_size,  c32 *u32str, s64 u32str_size);
s64 string_convert(const c16 *u16str, s64 u16str_size, c8  *u8str,  s64 u8str_size);
s64 string_convert(const c16 *u16str, s64 u16str_size, c32 *u32str, s64 u32str_size);
s64 string_convert(const c32 *u32str, s64 u32str_size, c8  *u8str,  s64 u8str_size);
s64 string_convert(const c32 *u32str, s64 u32str_size, c16 *u16str, s64 u16str_size);

// bytes required for type of first parameter from string of second parameter of size of third parameter...
s64 string_conversion_bytes_required([[maybe_unused]] c8  *, const c16 *u16str, s64 u16str_size);
s64 string_conversion_bytes_required([[maybe_unused]] c8  *, const c32 *u32str, s64 u32str_size);
s64 string_conversion_bytes_required([[maybe_unused]] c16 *, const c8  *u8str,  s64 u8str_size);
s64 string_conversion_bytes_required([[maybe_unused]] c16 *, const c32 *u32str, s64 u32str_size);
s64 string_conversion_bytes_required([[maybe_unused]] c32 *, const c8  *u8str,  s64 u8str_size);
s64 string_conversion_bytes_required([[maybe_unused]] c32 *, const c16 *u16str, s64 u16str_size);

// wchar_t """support"""
// TODO: remove these, use char_cast for everything instead
s64 string_convert(const c8  *u8str,  s64 u8str_size,  wchar_t *wstr, s64 wstr_size);
s64 string_convert(const c16 *u16str, s64 u16str_size, wchar_t *wstr, s64 wstr_size);
s64 string_convert(const c32 *u32str, s64 u32str_size, wchar_t *wstr, s64 wstr_size);
s64 string_convert(const wchar_t *wstr, s64 wstr_size, c8  *u8str,  s64 u8str_size);
s64 string_convert(const wchar_t *wstr, s64 wstr_size, c16 *u16str, s64 u16str_size);
s64 string_convert(const wchar_t *wstr, s64 wstr_size, c32 *u32str, s64 u32str_size);

s64 string_conversion_bytes_required([[maybe_unused]] c8  *, const wchar_t *wstr, s64 wstr_size);
s64 string_conversion_bytes_required([[maybe_unused]] c16 *, const wchar_t *wstr, s64 wstr_size);
s64 string_conversion_bytes_required([[maybe_unused]] c32 *, const wchar_t *wstr, s64 wstr_size);
s64 string_conversion_bytes_required([[maybe_unused]] wchar_t *, const c8  *u8str,  s64 u8str_size);
s64 string_conversion_bytes_required([[maybe_unused]] wchar_t *, const c16 *u16str, s64 u16str_size);
s64 string_conversion_bytes_required([[maybe_unused]] wchar_t *, const c32 *u32str, s64 u32str_size);

#define char_types_need_conversion(CFrom, CTo) (sizeof(CFrom) != sizeof(CTo))

static inline auto char_cast(wchar_t c)          { return (wc_utf_type)c; }
static inline auto char_cast(wchar_t *str)       { return (wc_utf_type*)str; }
static inline auto char_cast(const wchar_t *str) { return (const wc_utf_type*)str; }

// for more "readable" literals, e.g. string_literal(c16, "Hello")
#define string_literal(CharType, Literal)\
    inline_const_if(is_same(C, c8), u8##Literal,\
        inline_const_if(is_same(C, c16), u##Literal, U##Literal))

s32 utf_codepoint_length(const c8  *str);
s32 utf_codepoint_length(const c16 *str);
s32 utf_codepoint_length(const c32 *str);
#define utf_next_codepoint(Str) ((Str) + utf_codepoint_length(Str))

u32 utf_decode(const c8  *str);
u32 utf_decode(const c16 *str);
u32 utf_decode(const c32 *str);
const c8  *utf_decode(const c8  *str, u32 *cp);
const c16 *utf_decode(const c16 *str, u32 *cp);
const c32 *utf_decode(const c32 *str, u32 *cp);
const c8  *utf_decode(const c8  *str, u32 *cp, int *error);
const c16 *utf_decode(const c16 *str, u32 *cp, int *error);
const c32 *utf_decode(const c32 *str, u32 *cp, int *error);

template<typename C>
static inline C *utf_advance(C *str, u32 *out_cp, s64 *out_cp_size)
{
    int size = 0;
    utf_decode(str, out_cp, /*error*/&size);
    size = utf_codepoint_length(str);

    *out_cp_size = size;
    // *out_adv += size;
    return str;
}
