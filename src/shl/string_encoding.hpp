
/* string_encoding.hpp

Handle different string encodings.
u32 is used as the type for Unicode codepoints.
u16* is used for UTF-16 strings, since wchar_t is trash that can have variable
sizes.

TODO: docs
*/

#pragma once

#include "shl/number_types.hpp"

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
const char *utf8_decode (const char *str, u32 *cp, int *error);
const u16  *utf16_decode(const u16  *str, u32 *cp, int *error);

// u8str/u16str must be zero padded to a multiple of four bytes.
// str_buf_size is the total number of elements in the string, regardless
// of encoding or code points.
// Returns the number of codepoints (u32) written to out.
s64 utf8_decode_string (const char *u8str, s64 str_buf_size, u32 *out, s64 out_size);
s64 utf16_decode_string(const u16 *u16str, s64 str_buf_size, u32 *out, s64 out_size);

// Same as above, but u8str/u16str don't have to be zero padded.
s64 utf8_decode_string_safe (const char *u8str, s64 str_buf_size, u32 *out, s64 out_size);
s64 utf16_decode_string_safe(const u16 *u16str, s64 str_buf_size, u32 *out, s64 out_size);

// returns the number of bytes written, or -1 on error.
// out must have space for 4 bytes.
s64 utf8_encode(u32 cp, char *out);
s64 utf16_encode(u32 cp, u16 *out);

s64 utf8_encode_string (const u32 *cps, s64 cp_count, char *out, s64 out_size);
s64 utf16_encode_string(const u32 *cps, s64 cp_count, u16  *out, s64 out_size);

// returns the number of elements written such that
// out + return value = directly after written content.
s64 utf8_to_utf16(const char *u8str, s64 u8str_size,  u16  *out, s64 out_size);
s64 utf16_to_utf8(const u16 *u16str, s64 u16str_size, char *out, s64 out_size);

s64 utf16_bytes_required_from_utf8(const char *u8str, s64 u8str_size);
s64 utf16_bytes_required_from_codepoints(const u32 *cps, s64 cp_count);
s64 utf8_bytes_required_from_utf16(const u16 *u16str, s64 u16str_size);
s64 utf8_bytes_required_from_codepoints(const u32 *cps, s64 cp_count);

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
}

s64 string_convert(const char *u8str, s64 u8str_size, wchar_t *wcstr, s64 wcstr_size);
s64 string_convert(const wchar_t *wcstr, s64 wcstr_size, char *u8str, s64 u8str_size);
s64 string_conversion_chars_required(const char *u8str, s64 u8str_size);
s64 string_conversion_chars_required(const wchar_t *wcstr, s64 wcstr_size);
s64 string_conversion_bytes_required(const char *u8str, s64 u8str_size);
s64 string_conversion_bytes_required(const wchar_t *wcstr, s64 wcstr_size);
