
/* string_encoding.hpp

Handle different string encodings.
u32 is used as the type for Unicode codepoints.

REMINDER:

u8"abc"  ->  utf8  string
 u"abc"  ->  utf16 string
 U"abc"  ->  utf32 string... isn't C++ amazing

Types (from char_types.hpp):
    c8  - utf8 unit type
    c16 - utf16 unit type
    c32 - utf32 unit type
    wc_utf_type - either c16 or c32, whichever is the size of wchar_t

Additionaly defines sys_utf_char, which is sys_char if sys_char is one of
c8, c16 or c32, or becomes wc_utf_type if sys_char is wchar_t.
NOTE: We can't make sys_char a c8/c16/c32 because then it wouldn't be a
      "system character" anymore.

Functions:
utf8_decode(c8 *string, u32 *cp, int *error)
    Decodes a single codepoint from the utf8 string and writes it to cp.
    error is nonzero if a UTF decoding error occurred, otherwise error is zero.
    Returns a pointer to the next unit in the utf8 string after the decoded
    codepoint.
    NOTE: string must be at least of size 4 as this function does not check for
          the correctness of the utf8 codepoint.

utf16_decode(c16 *string, u32 *cp, int *error)
    Same as utf8_decode, but UTF 16.
    NOTE: string must be at least of size 2 as this function does not check for
          the correctness of the utf8 codepoint.

utf32_decode(c32 *string, u32 *cp, int *error)
    Same as utf8_decode, but UTF 32.
    
utf8_decode_safe(c8 *string, u32 *cp, int *error)
    Same as utf8_decode, but string does not have to be of at least size 4.

utf16_decode_safe(c16 *string, u32 *cp, int *error)
    Same as utf16_decode, but string does not have to be of at least size 2.

utf32_decode_safe(c32 *string, u32 *cp, int *error)
    Same as utf32_decode.

utf8_decode_string(c8 *string, s64 string_size, u32 *cp_out, s64 out_size)
    Decodes as many (valid) UTF codepoints from string into cp_out as possible
    within string_size string units and out_size codepoints.
    Returns number of codepoints written.
    NOTE: string should be 4 byte padded at the end to not cause access violations.

utf16_decode_string(...)
utf32_decode_string(...) you get the idea

utf8_decode_string_safe(c8 *string, s64 string_size, u32 *cp_out, s64 out_size)
    Same as utf8_decode_string, but string does not have to be padded.

utf16_decode_string_safe(...)
utf32_decode_string_safe(...)

utf8_encode(u32 cp, c8 *out)
    Encodes a UTF codepoint into UTF-8.
    out should have enough space to hold cp.
    Returns number of units written to out.

utf16_encode(u32 cp, c16 *out)
    Same as utf8_encode but UTF-16.

utf32_encode(u32 cp, c16 *out)
    Same as utf8_encode but UTF-32.

utf8_encode_string(u32 *cps, s64 cp_count, c8 *out, s64 out_size)
    Encodes as many UTF codepoints from cps into UTF-8 string out as possible
    within cp_count codepoints and out_size UTF-8 units.
    Returns number of units written.
    
utf16_encode_string(u32 *cps, s64 cp_count, c16 *out, s64 out_size) ...
utf32_encode_string(u32 *cps, s64 cp_count, c32 *out, s64 out_size) ...

utf<A>_to_utf<B>(A_string, A_size, B_out_string, B_out_size)
    UTF conversion functions, e.g. utf8_to_utf16 or utf16_to_utf32.
    Converts between UTF-A to UTF-B, where A and B are either 8, 16 or 32,
    but not the same.
    Returns the number of units written to B_out_string.

utf<A>_units_required_from_utf<B>(B_string, B_size)
    Returns the number of units a UTF-A string would require when converting
    the entire UTF-B string B_string to UTF-A.

utf<A>_bytes_required_from_utf<B>(B_string, B_size)
    Returns the number of _bytes_ a UTF-A string would require when converting
    the entire UTF-B string B_string to UTF-A.

codepoint_utf8_length(u32 cp)
    Returns the number of utf8 units required to represent the UTF codepoint cp.

codepoint_utf16_length(u32 cp)
    Returns the number of utf16 units required to represent the UTF codepoint cp.

codepoint_utf32_length(u32 cp)
    Returns the number of utf32 units required to represent the UTF codepoint cp.

utf8_bytes_required_from_codepoints(u32 *cps, s64 cp_count)
    Returns the number of _bytes_ a UTF-8 string would require when encoding
    the UTF codepoints cps to UTF-8.

utf16_bytes_required_from_codepoints(u32 *cps, s64 cp_count) ...
utf32_bytes_required_from_codepoints(u32 *cps, s64 cp_count) ...

(overloaded function)
string_convert(A_string, A_size, B_out_string, B_size)
    Returns utf<A>_to_utf<B>, depending on the types of A_string and B_out_string.

string_conversion_bytes_required(A_out_string, B_string, B_size)
    Returns utf<A>_bytes_required_from_utf<B>, depending on the types of
    A_out_string and B_out_string.

char_cast(wchar_t ...)
    Casts the parameter to either c16 or c32, depending on which is the same
    size as wchar_t. No copy is performed.

utf_codepoint_length(u8  *str)
    Returns the number of units the codepoint pointed to by str requires in UTF-8.
utf_codepoint_length(u16 *str) ditto
utf_codepoint_length(u32 *str) ditto

utf_decode(c8 *str)
utf_decode(c16 *str)
utf_decode(c32 *str)
    Returns the decoded codepoint pointed to by str.

utf_decode(c8  *str, u32 *cp)
utf_decode(c16 *str, u32 *cp)
utf_decode(c32 *str, u32 *cp)
    Sets cp to the decoded codepoint pointed to by str and
    returns a pointer to the next codepoint in str.

utf_decode(c8  *str, u32 *cp, int *error)
utf_decode(c16 *str, u32 *cp, int *error)
utf_decode(c32 *str, u32 *cp, int *error)
    Sets cp to the decoded codepoint pointed to by str and
    returns a pointer to the next codepoint in str
    and sets error to any UTF decoding errors.


*/

#pragma once

#include "shl/number_types.hpp"
#include "shl/char_types.hpp"
#include "shl/platform.hpp"

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

using sys_utf_char = if_type(is_same(sys_char, wchar_t), wc_utf_type, sys_char);
#define SYS_UTF_CHAR(x) ((const sys_utf_char*)SYS_CHAR(x))

extern "C"
{
const c8  *utf8_decode (const c8  *str, u32 *cp, int *error);
const c16 *utf16_decode(const c16 *str, u32 *cp, int *error);
const c32 *utf32_decode(const c32 *str, u32 *cp, int *error);
const c8  *utf8_decode_safe (const c8  *str, u32 *cp, int *error);
const c16 *utf16_decode_safe(const c16 *str, u32 *cp, int *error);
const c32 *utf32_decode_safe(const c32 *str, u32 *cp, int *error);

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

// used internally for for_utf_string
template<typename C>
static inline C *_for_utf_decode(C *str, u32 *out_cp, s64 *out_cp_size)
{
    int size = 0;
    utf_decode(str, out_cp);
    size = utf_codepoint_length(str);

    *out_cp_size = size;
    return str;
}

template<typename C>
static inline C *_for_utf_decode(C *str, s64 *out_cp_size)
{
    int size = utf_codepoint_length(str);

    *out_cp_size = size;
    return str;
}
