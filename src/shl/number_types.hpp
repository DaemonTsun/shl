#pragma once

/* number_types.hpp
v1.2
got rid of stdint.h
v1.1
max / min values

Defines common number types, macros and compile-time functions for number types.

The number types are:

    u8
    u16
    u32
    u64
    s8
    s16
    s32
    s64

with 'u' meaning unsigned, 's' meaning signed.
Each type has a T_MAX macro constant (where T is the type, capitalized, e.g. s8 -> S8_MAX)
with the largest possible value of that type, and signed types have T_MIN values.

Templated min and max values are available with min_value(T) and max_value(T) macros,
e.g.
    if constexpr (123 < max_value(u8)) { ... }
works fine.

Also mostly just used in syscalls, this header defines the 'sys_int' and 'sys_uint'
types which are of the size of the architecture, expected in syscalls.

*/

#include "shl/architecture.hpp"

#if Wordsize == 64 && !defined(_MSC_VER)
#define S64_LIT(c) c ## L
#define U64_LIT(c) c ## UL
#else
#define S64_LIT(c) c ## LL
#define U64_LIT(c) c ## ULL
#endif

#if defined(_MSC_VER)
#  if (_MSC_VER >= 1300)
typedef unsigned __int8     u8;
typedef unsigned __int16    u16;
typedef unsigned __int32    u32;
typedef   signed __int8     s8;
typedef   signed __int16    s16;
typedef   signed __int32    s32;
#  else // old MSVC
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef   signed char       s8;
typedef   signed short      s16;
typedef   signed int        s32;
#  endif

typedef unsigned __int64    u64;
typedef   signed __int64    s64;
#else // not MSVC

// Linux
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef   signed char       s8;
typedef   signed short      s16;
typedef   signed int        s32;

#  if Wordsize == 64
typedef unsigned long int   u64;
typedef   signed long int   s64;
#  else
typedef unsigned long long int u64;
typedef   signed long long int s64;
#  endif
#endif

// sysint
#if Wordsize == 64
typedef s64 sys_int;
typedef u64 sys_uint;
#else
typedef s32 sys_int;
typedef u32 sys_uint;
#endif

// max & min values
#define  S8_MIN (-128)
#define S16_MIN (-32768)
#define S32_MIN (-2147483647 - 1)
#define S64_MIN (-S64_LIT(9223372036854775807) - 1)

#define  S8_MAX (127)
#define S16_MAX (32767)
#define S32_MAX (2147483647)
#define S64_MAX (S64_LIT(9223372036854775807))

#define  U8_MAX (255)
#define U16_MAX (65535)
#define U32_MAX (4294967295u)
#define U64_MAX (U64_LIT(18446744073709551615))

template<typename T> struct _max_value { };
#define define_max_value(T, Val) template<> struct _max_value<T>     { static constexpr T value = Val; };
#define max_value(T) _max_value<T>::value

template<typename T> struct _min_value { };
#define define_min_value(T, Val) template<> struct _min_value<T>     { static constexpr T value = Val; };
#define min_value(T) _min_value<T>::value

define_max_value(u8,  U8_MAX);
define_max_value(u16, U16_MAX);
define_max_value(u32, U32_MAX);
define_max_value(u64, U64_MAX);
define_max_value(s8,  S8_MAX);
define_max_value(s16, S16_MAX);
define_max_value(s32, S32_MAX);
define_max_value(s64, S64_MAX);

define_min_value(u8,  0);
define_min_value(u16, 0);
define_min_value(u32, 0);
define_min_value(u64, 0);
define_min_value(s8,  S8_MIN);
define_min_value(s16, S16_MIN);
define_min_value(s32, S32_MIN);
define_min_value(s64, S64_MIN);

#ifndef offsetof
#define offsetof(st, m) ((s64)&(((st *)0)->m))
#endif
