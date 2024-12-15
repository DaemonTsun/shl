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

Also defines define_explicit_number_type which may be used to define an enum number type
with arithmetic operators.
*/

#include "shl/architecture.hpp"

#if Wordsize == 64 && !defined(_WIN32)
#  define S64_LIT(c) c ## L
#  define U64_LIT(c) c ## UL
#else
#  define S64_LIT(c) c ## LL
#  define U64_LIT(c) c ## ULL
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

    typedef unsigned char       u8;
    typedef unsigned short      u16;
    typedef unsigned int        u32;
    typedef   signed char       s8;
    typedef   signed short      s16;
    typedef   signed int        s32;

#  if defined(_WIN32) || defined(_WIN64)
    typedef unsigned long long int u64;
    typedef   signed long long int s64;
#  else // else linux
#    if Wordsize == 64
        typedef unsigned long int   u64;
        typedef   signed long int   s64;
#    else
        typedef unsigned long long int u64;
        typedef   signed long long int s64;
#    endif
#endif // end if windows
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

// TODO: read up on float, IEEE and if these values are even accurate / stable.
// These values come from my machine.
#ifndef FLOAT_MAX
#  define FLOAT_MAX (340282346638528859811704183484516925440.00000f)
#  define FLOAT_MIN (-FLOAT_MAX)
#  define FLOAT_POSITIVE_MIN (0.0000000000000000000000000000000000000117549435082228750796873653722224567781866555677208752150875170627841725945472717285156250f)
#  define DOUBLE_MAX (179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000)
#  define DOUBLE_MIN (-DOUBLE_MAX)
#  define DOUBLE_POSITIVE_MIN (0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022250738585072013830902327173324040642192159804623318305533274168872044348139181958542831590125110205640673397310358110051524341615534601088560123853777188211307779935320023304796101474425836360719215650469425037342083752508066506166581589487204911799685916396485006359087701183048747997808877537499494515804516050509153998565824708186451135379358049921159810857660519924333521143523901487956996095912888916029926415110634663133936634775865130293717620473256317814856643508721228286376420448468114076139114770628016898532441100241614474216185671661505401542850847167529019031613227788967297073731233340869889831750678388469260927739779728586596549410913690954061364675687023986783152906809846172109246253967285156250)
// TODO: infinity, NaN
#endif

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
define_max_value(float,  FLOAT_MAX);
define_max_value(double, DOUBLE_MAX);

define_min_value(u8,  0);
define_min_value(u16, 0);
define_min_value(u32, 0);
define_min_value(u64, 0);
define_min_value(s8,  S8_MIN);
define_min_value(s16, S16_MIN);
define_min_value(s32, S32_MIN);
define_min_value(s64, S64_MIN);
define_min_value(float,  FLOAT_MIN);
define_min_value(double, DOUBLE_MIN);

#ifndef offset_of
#define offset_of(st, m) ((s64)&(((st *)0)->m))
#endif

// custom types
#define define_explicit_number_type(T, Base)\
    enum T : Base;\
    constexpr inline Base value(T x)            { return (Base)x; }\
    constexpr inline T operator+ (T x)          { return (T)(+(Base)x); }\
    constexpr inline T operator- (T x)          { return (T)(-(Base)x); }\
    constexpr inline T operator~ (T x)          { return (T)(~(Base)x); }\
    constexpr inline bool operator!(T x)        { return (T)(!(Base)x); }\
    constexpr inline T operator+ (T l, T r)     { return (T)((Base)l +  (Base)r); }\
    constexpr inline T operator- (T l, T r)     { return (T)((Base)l -  (Base)r); }\
    constexpr inline T operator* (T l, T r)     { return (T)((Base)l *  (Base)r); }\
    constexpr inline T operator/ (T l, T r)     { return (T)((Base)l /  (Base)r); }\
    constexpr inline T operator% (T l, T r)     { return (T)((Base)l %  (Base)r); }\
    constexpr inline T operator& (T l, T r)     { return (T)((Base)l &  (Base)r); }\
    constexpr inline T operator| (T l, T r)     { return (T)((Base)l |  (Base)r); }\
    constexpr inline T operator^ (T l, T r)     { return (T)((Base)l ^  (Base)r); }\
    constexpr inline T operator<<(T l, T r)     { return (T)((Base)l << (Base)r); }\
    constexpr inline T operator>>(T l, T r)     { return (T)((Base)l >> (Base)r); }\
    constexpr inline T &operator+= (T &l, T r)  { return l = l +  r; }\
    constexpr inline T &operator-= (T &l, T r)  { return l = l -  r; }\
    constexpr inline T &operator*= (T &l, T r)  { return l = l *  r; }\
    constexpr inline T &operator/= (T &l, T r)  { return l = l /  r; }\
    constexpr inline T &operator%= (T &l, T r)  { return l = l %  r; }\
    constexpr inline T &operator&= (T &l, T r)  { return l = l &  r; }\
    constexpr inline T &operator|= (T &l, T r)  { return l = l |  r; }\
    constexpr inline T &operator^= (T &l, T r)  { return l = l ^  r; }\
    constexpr inline T &operator<<=(T &l, T r)  { return l = l << r; }\
    constexpr inline T &operator>>=(T &l, T r)  { return l = l >> r; }\
    constexpr inline T &operator++(T &x)        { return x += (T)1; }\
    constexpr inline T &operator--(T &x)        { return x -= (T)1; }\
    constexpr inline T operator++(T &x, int)    { T tmp = x; ++x; return tmp; }\
    constexpr inline T operator--(T &x, int)    { T tmp = x; --x; return tmp; }
