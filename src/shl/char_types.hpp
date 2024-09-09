
/* char_types.hpp

Mostly for UTF "char" types.

    c8  - utf8 unit type
    c16 - utf16 unit type
    c32 - utf32 unit type
    wc_utf_type - either c16 or c32, whichever is the size of wchar_t
 */

#include "shl/type_functions.hpp"

typedef char     c8;
typedef char16_t c16;
typedef char32_t c32;

#define is_char_type(T) (is_same(T, c8) || is_same(T, c16) || is_same(T, c32))

using wc_utf_type = if_type((sizeof(wchar_t) == sizeof(c16)), c16, c32);
