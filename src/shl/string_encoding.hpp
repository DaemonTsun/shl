
/* string_encoding.hpp

Handle different string encodings.
u32 is used as the type for Unicode codepoints.
u16* is used for UTF-16 strings, since wchar_t is trash that can have variable
sizes.
*/

#pragma once

#include "shl/number_types.hpp"

#define UNICODE_MAX 0x10ffff

#define UTF8_1_MAX  0x7f
#define UTF8_2_MAX  0x7ff
#define UTF8_3_MAX  0xffff
#define UTF8_4_MAX  0x10ffff
#define UTF16_MAX   0xffff

static constexpr char utf8_lengths[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
};

// strings must be zero padded to four bytes
const char *utf8_decode (const char *str, u32 *cp, int *error)
{
    // https://nullprogram.com/blog/2017/10/06/
    static constexpr const s32 masks[]  = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static constexpr const u32 mins[]   = {4194304, 0, 128, 2048, 65536};
    static constexpr const s32 shiftc[] = {0, 18, 12, 6, 0};
    static constexpr const s32 shifte[] = {0, 6, 4, 2, 0};

    const u8 *s = (const u8 *)str;
    int len = utf8_lengths[s[0] >> 3];
    const u8 *next = s + len + !len;

    /* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
    *cp  = (u32)(s[0] & masks[len]) << 18;
    *cp |= (u32)(s[1] & 0x3f) << 12;
    *cp |= (u32)(s[2] & 0x3f) <<  6;
    *cp |= (u32)(s[3] & 0x3f) <<  0;
    *cp >>= shiftc[len];

    /* Accumulate the various error conditions. */
    *error  = (*cp < mins[len]) << 6; // non-canonical encoding
    *error |= ((*cp >> 11) == 0x1b) << 7;  // surrogate half?
    *error |= (*cp > 0x10FFFF) << 8;  // out of range?
    *error |= (s[1] & 0xc0) >> 2;
    *error |= (s[2] & 0xc0) >> 4;
    *error |= (s[3]       ) >> 6;
    *error ^= 0x2a; // top two bits of each tail byte correct?
    *error >>= shifte[len];

    return (const char*)next;
}

/*
static inline const u16  *utf16_decode(const u16  *str, u32 *cp)
{
    // TODO: implement
    (void)cp;
    return str;
}
*/

// length of a single unicode codepoint if it were encoded as utf8/utf16
static inline s32 codepoint_utf8_length (u32 cp)
{
    if      (cp <= UTF8_1_MAX)  return 1;
    else if (cp <= UTF8_2_MAX)  return 2;
    else if (cp <= UTF8_3_MAX)  return 3;
    else                        return 4;
}

static inline s32 codepoint_utf16_length(u32 cp)
{
    if (cp <= UTF16_MAX)    return 1;
    else                    return 2;
}
