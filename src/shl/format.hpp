
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

s64 to_string(string  *s, u8 x);
s64 to_string(string  *s, u8 x, u64 offset);
// pad_size: how many zeroes to pad with, not including prefix
s64 to_string(string  *s, u8 x, u64 offset, int base);
s64 to_string(string  *s, u8 x, u64 offset, int base, bool include_prefix, int pad_length, char pad_char);
