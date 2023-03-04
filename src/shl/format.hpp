
#pragma once

#include "shl/type_functions.hpp"
#include "shl/string.hpp"

s64 to_string(string  *s, bool x);
s64 to_string(string  *s, bool x, u64 offset);
s64 to_string(string  *s, bool x, u64 offset, bool as_text);
s64 to_string(wstring *s, bool x);
s64 to_string(wstring *s, bool x, u64 offset);
s64 to_string(wstring *s, bool x, u64 offset, bool as_text);

s64 to_string(string  *s, char x);
s64 to_string(string  *s, char x, u64 offset);
s64 to_string(wstring *s, wchar_t x);
s64 to_string(wstring *s, wchar_t x, u64 offset);

template<typename C = char>
struct integer_format_options
{
    int base;               // 2, 8, 10 or 16
    bool include_prefix;    // 0b for base 2, 0 for base 8, 0x for base 16
    int pad_length;         // left padding length EXCLUDING PREFIX OR SIGN
    C pad_char;             // padding character, defaults to 0
    bool force_sign;        // always include + or -
    bool caps_letters;      // only for hex, whether to use ABCDEF
    bool caps_prefix;       // if include_prefix is on and base is 2 or 16, make the
                            // prefix letter caps
};

template<typename C = char>
inline constexpr integer_format_options default_integer_options = 
{
    .base = 10,
    .include_prefix = false,
    .pad_length = 0,
    .pad_char = '0',
    .force_sign = false,
    .caps_letters = false,
    .caps_prefix = false
};

s64 to_string(string  *s, u8 x);
s64 to_string(string  *s, u8 x, u64 offset);
s64 to_string(string  *s, u8 x, u64 offset, integer_format_options<char> options);
s64 to_string(string  *s, u16 x);
s64 to_string(string  *s, u16 x, u64 offset);
s64 to_string(string  *s, u16 x, u64 offset, integer_format_options<char> options);
s64 to_string(string  *s, u32 x);
s64 to_string(string  *s, u32 x, u64 offset);
s64 to_string(string  *s, u32 x, u64 offset, integer_format_options<char> options);
s64 to_string(string  *s, u64 x);
s64 to_string(string  *s, u64 x, u64 offset);
s64 to_string(string  *s, u64 x, u64 offset, integer_format_options<char> options);

s64 to_string(string  *s, s8 x);
s64 to_string(string  *s, s8 x, u64 offset);
s64 to_string(string  *s, s8 x, u64 offset, integer_format_options<char> options);
s64 to_string(string  *s, s16 x);
s64 to_string(string  *s, s16 x, u64 offset);
s64 to_string(string  *s, s16 x, u64 offset, integer_format_options<char> options);
s64 to_string(string  *s, s32 x);
s64 to_string(string  *s, s32 x, u64 offset);
s64 to_string(string  *s, s32 x, u64 offset, integer_format_options<char> options);
s64 to_string(string  *s, s64 x);
s64 to_string(string  *s, s64 x, u64 offset);
s64 to_string(string  *s, s64 x, u64 offset, integer_format_options<char> options);


template<typename C = char>
struct float_format_options
{
    int precision;                  // how many remainder digits
    int pad_length;                 // left padding length EXCLUDING SIGN
    C pad_char;                     // padding character, defaults to 0
    bool force_sign;                // always include + or -
    bool ignore_trailing_zeroes;    // if true, e.g. 0.1000 becomes 0.1
};

#define FLOAT_MAX_PRECISION 19

template<typename C = char>
inline constexpr float_format_options default_float_options = 
{
    .precision = 6,
    .pad_length = 0,
    .pad_char = '0',
    .force_sign = false,
    .ignore_trailing_zeroes = true
};


s64 to_string(string  *s, float x);
s64 to_string(string  *s, float x, u64 offset);
s64 to_string(string  *s, float x, u64 offset, float_format_options<char> options);
s64 to_string(string  *s, double x);
s64 to_string(string  *s, double x, u64 offset);
s64 to_string(string  *s, double x, u64 offset, float_format_options<char> options);
