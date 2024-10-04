#pragma once

/* enum_flag.hpp

Defines the macro enum_flag which defines operators and functions:
     value       - returns the underlying value of the enum
     is_flag_set - checks if a given flag is set
     set_flag    - sets a flag
     unset_flag  - unsets a flag

Example:
    enum class Attributes
    {
        None        = 0,
        Floating    = 1,
        Liquid      = 2,
        Blue        = 4
    };

    enum_flag(Attributes);

    void ...()
    {
        Attributes attrs = Attributes::None;

        set_flag(attrs, Attributes::Liquid);
        set_flag(attrs, Attributes::Blue);

        if (is_flag_set(attrs, Attributes::Blue))
            printf("Blue attribute is indeed set.\n");
    }

*/

#include "shl/type_functions.hpp"

#ifndef JOIN
#define JOIN(X, Y) X##Y
#endif

#define DEFINE_ENUM_VALUE(T)\
    constexpr inline underlying_type(T) value(T a) { return static_cast<underlying_type(T)>(a); }

#define DEFINE_ENUM_UNDERLYING_TYPE(T)\
    typedef underlying_type(T) JOIN(T, _ut);\
    DEFINE_ENUM_VALUE(T)

#define ENUM_CLASS_FLAG_OPS(T)\
    DEFINE_ENUM_UNDERLYING_TYPE(T)\
    constexpr inline underlying_type(T) operator~ (T a)                         { return (~static_cast<underlying_type(T)>(a)); }\
    constexpr inline T operator| (T a, T b)                                     { return static_cast<T>(static_cast<underlying_type(T)>(a) | static_cast<underlying_type(T)>(b)); }\
    constexpr inline underlying_type(T) operator| (T a, underlying_type(T) b)   { return static_cast<underlying_type(T)>(a) | b; }\
    constexpr inline underlying_type(T) operator| (underlying_type(T) a, T b)   { return a | static_cast<underlying_type(T)>(b); }\
    constexpr inline T operator& (T a, T b)                                     { return static_cast<T>(static_cast<underlying_type(T)>(a) & static_cast<underlying_type(T)>(b)); }\
    constexpr inline underlying_type(T) operator& (T a, underlying_type(T) b)   { return static_cast<underlying_type(T)>(a) & b; }\
    constexpr inline underlying_type(T) operator& (underlying_type(T) a, T b)   { return a & static_cast<underlying_type(T)>(b); }\
    constexpr inline T operator^ (T a, T b)                                     { return static_cast<T>(static_cast<underlying_type(T)>(a) ^ static_cast<underlying_type(T)>(b)); }\
    constexpr inline underlying_type(T) operator^ (T a, underlying_type(T) b)   { return static_cast<underlying_type(T)>(a) ^ b; }\
    constexpr inline underlying_type(T) operator^ (underlying_type(T) a, T b)   { return a ^ static_cast<underlying_type(T)>(b); }\
    constexpr inline T& operator|=(T& a, T b)                                   { return a = static_cast<T>(static_cast<underlying_type(T)>(a) | static_cast<underlying_type(T)>(b)); }\
    constexpr inline T& operator|=(T& a, underlying_type(T) b)                  { return a = static_cast<T>(static_cast<underlying_type(T)>(a) | b); }\
    constexpr inline T& operator&=(T& a, T b)                                   { return a = static_cast<T>(static_cast<underlying_type(T)>(a) & static_cast<underlying_type(T)>(b)); }\
    constexpr inline T& operator&=(T& a, underlying_type(T) b)                  { return a = static_cast<T>(static_cast<underlying_type(T)>(a) & b); }\
    constexpr inline T& operator^=(T& a, T b)                                   { return a = static_cast<T>(static_cast<underlying_type(T)>(a) ^ static_cast<underlying_type(T)>(b)); }\
    constexpr inline T& operator^=(T& a, underlying_type(T) b)                  { return a = static_cast<T>(static_cast<underlying_type(T)>(a) ^ b); }\
    constexpr inline bool is_flag_set(T a, T b)                                 { return (a & b) == b; }\
    constexpr inline bool is_flag_set(T a, underlying_type(T) b)                { return (a & b) == b; }\
    constexpr inline bool is_flag_set(underlying_type(T) a, T b)                { return static_cast<T>(a & b) == b; }\
    constexpr inline T& set_flag(T& a, T b)                     { return a |=  b; } \
    constexpr inline T& set_flag(T& a, underlying_type(T) b)    { return a |=  b; } \
    constexpr inline T& unset_flag(T& a, T b)                   { return a &= ~b; } \
    constexpr inline T& unset_flag(T& a, underlying_type(T) b)  { return a &= ~b; }

#define enum_flag(T)\
    ENUM_CLASS_FLAG_OPS(T)

