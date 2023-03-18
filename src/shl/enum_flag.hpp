#pragma once

/* enum_flag.hpp
 * v1.0
 *
 * defines the macro enum_flag which defines
 * operators and functions:
 *      value  - returns the underlying value of the enum
 *      is_set - checks if a given flag is set
 *      set    - sets a flag
 *      unset  - unsets a flag
 */

#include "shl/type_functions.hpp"

#define JOIN(X, Y) X##Y

#define DEFINE_ENUM_VALUE(T)\
    constexpr inline underlying_type(T) value(T a) { return static_cast<underlying_type(T)>(a); }

#define DEFINE_ENUM_UNDERLYING_TYPE(T)\
    typedef underlying_type(T) JOIN(T, _ut);\
    DEFINE_ENUM_VALUE(T)

#define ENUM_CLASS_FLAG_OPS(T)\
    DEFINE_ENUM_UNDERLYING_TYPE(T)\
    constexpr inline underlying_type(T) operator~ (T a)       { return (~static_cast<underlying_type(T)>(a)); }\
    constexpr inline T operator| (T a, T b)  { return static_cast<T>(static_cast<underlying_type(T)>(a) | static_cast<underlying_type(T)>(b)); }\
    constexpr inline underlying_type(T) operator| (T a, underlying_type(T) b)  { return static_cast<underlying_type(T)>(a) | b; }\
    constexpr inline underlying_type(T) operator| (underlying_type(T) a, T b)  { return a | static_cast<underlying_type(T)>(b); }\
    constexpr inline T operator& (T a, T b)  { return static_cast<T>(static_cast<underlying_type(T)>(a) & static_cast<underlying_type(T)>(b)); }\
    constexpr inline underlying_type(T) operator& (T a, underlying_type(T) b)  { return static_cast<underlying_type(T)>(a) & b; }\
    constexpr inline underlying_type(T) operator& (underlying_type(T) a, T b)  { return a & static_cast<underlying_type(T)>(b); }\
    constexpr inline T operator^ (T a, T b)  { return static_cast<T>(static_cast<underlying_type(T)>(a) ^ static_cast<underlying_type(T)>(b)); }\
    constexpr inline underlying_type(T) operator^ (T a, underlying_type(T) b)  { return static_cast<underlying_type(T)>(a) ^ b; }\
    constexpr inline underlying_type(T) operator^ (underlying_type(T) a, T b)  { return a ^ static_cast<underlying_type(T)>(b); }\
    constexpr inline T& operator|=(T& a, T b) { return a = static_cast<T>(static_cast<underlying_type(T)>(a) | static_cast<underlying_type(T)>(b)); }\
    constexpr inline T& operator|=(T& a, underlying_type(T) b) { return a = static_cast<T>(static_cast<underlying_type(T)>(a) | b); }\
    constexpr inline T& operator&=(T& a, T b) { return a = static_cast<T>(static_cast<underlying_type(T)>(a) & static_cast<underlying_type(T)>(b)); }\
    constexpr inline T& operator&=(T& a, underlying_type(T) b) { return a = static_cast<T>(static_cast<underlying_type(T)>(a) & b); }\
    constexpr inline T& operator^=(T& a, T b) { return a = static_cast<T>(static_cast<underlying_type(T)>(a) ^ static_cast<underlying_type(T)>(b)); }\
    constexpr inline T& operator^=(T& a, underlying_type(T) b) { return a = static_cast<T>(static_cast<underlying_type(T)>(a) ^ b); }\
    constexpr inline bool is_set(T a, T b) { return (a & b) == b; }\
    constexpr inline bool is_set(T a, underlying_type(T) b) { return (a & b) == b; }\
    constexpr inline bool is_set(underlying_type(T) a, T b) { return static_cast<T>(a & b) == b; }\
    constexpr inline T& set(T& a, T b) { return a |= b; } \
    constexpr inline T& set(T& a, underlying_type(T) b) { return a |= b; } \
    constexpr inline T& unset(T& a, T b) { return a &= ~b; } \
    constexpr inline T& unset(T& a, underlying_type(T) b) { return a &= ~b; }

#define enum_flag(T)\
    ENUM_CLASS_FLAG_OPS(T)

