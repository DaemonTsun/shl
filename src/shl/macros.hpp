
#pragma once

/* macros.hpp

JOIN        function variant of x##y
JOIN3       x##y##z
GET_MACROn  Used internally for variable argument macros
offset_of   The offset_of operator
*/

#ifndef JOIN
#define JOIN(X, Y) X##Y
#endif

#ifndef JOIN3
#define JOIN3(X, Y, Z) X##Y##Z
#endif

#ifndef GET_MACRO2
#define GET_MACRO2(_1, _2, _3, NAME, ...) NAME
#endif

#ifndef GET_MACRO3
#define GET_MACRO3(_1, _2, _3, _4, NAME, ...) NAME
#endif

#ifndef GET_MACRO4
#define GET_MACRO4(_1, _2, _3, _4, _5, NAME, ...) NAME
#endif

#ifndef GET_MACRO5
#define GET_MACRO5(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#endif

#ifndef GET_MACRO6
#define GET_MACRO6(_1, _2, _3, _4, _5, _6, _7, NAME, ...) NAME
#endif

#ifndef offset_of
#  if defined(__GNUC__)
#    define offset_of(type, member) __builtin_offsetof(type, member)
#  elif defined(_MSC_VER) && defined _CRT_USE_BUILTIN_OFFSETOF
#    define offset_of(type, member) __builtin_offsetof(type, member)
#  else
// fallback definition (UB)
#    define offset_of(type, member) (long unsigned int)(&((type *)0)->member)
#  endif
#endif
