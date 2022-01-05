#pragma once

/* enum_flag.hpp
 * defines the macro ENUM_CLASS_FLAG_OPS which defines
 * operators and functions:
 *      value  - returns the underlying value of the enum
 *      is_set - checks if a given flag is set
 *      set    - sets a flag
 *      unset  - unsets a flag
 */

#include <type_traits>

#define JOIN(X, Y) X##Y

#define DEFINE_ENUM_UNDERLYING_TYPE(T)\
    typedef std::underlying_type_t<T> JOIN(T, _ut);\
    constexpr inline std::underlying_type_t<T> value(T a) { return static_cast<std::underlying_type_t<T>>(a); }

#define ENUM_CLASS_FLAG_OPS(T)\
    DEFINE_ENUM_UNDERLYING_TYPE(T)\
    constexpr inline std::underlying_type_t<T> operator~ (T a)       { return (~static_cast<std::underlying_type_t<T>>(a)); }\
    constexpr inline T operator| (T a, T b)  { return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b)); }\
    constexpr inline std::underlying_type_t<T> operator| (T a, std::underlying_type_t<T> b)  { return static_cast<std::underlying_type_t<T>>(a) | b; }\
    constexpr inline std::underlying_type_t<T> operator| (std::underlying_type_t<T> a, T b)  { return a | static_cast<std::underlying_type_t<T>>(b); }\
    constexpr inline T operator& (T a, T b)  { return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b)); }\
    constexpr inline std::underlying_type_t<T> operator& (T a, std::underlying_type_t<T> b)  { return static_cast<std::underlying_type_t<T>>(a) & b; }\
    constexpr inline std::underlying_type_t<T> operator& (std::underlying_type_t<T> a, T b)  { return a & static_cast<std::underlying_type_t<T>>(b); }\
    constexpr inline T operator^ (T a, T b)  { return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^ static_cast<std::underlying_type_t<T>>(b)); }\
    constexpr inline std::underlying_type_t<T> operator^ (T a, std::underlying_type_t<T> b)  { return static_cast<std::underlying_type_t<T>>(a) ^ b; }\
    constexpr inline std::underlying_type_t<T> operator^ (std::underlying_type_t<T> a, T b)  { return a ^ static_cast<std::underlying_type_t<T>>(b); }\
    constexpr inline T& operator|=(T& a, T b) { return a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b)); }\
    constexpr inline T& operator|=(T& a, std::underlying_type_t<T> b) { return a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | b); }\
    constexpr inline T& operator&=(T& a, T b) { return a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b)); }\
    constexpr inline T& operator&=(T& a, std::underlying_type_t<T> b) { return a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) & b); }\
    constexpr inline T& operator^=(T& a, T b) { return a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^ static_cast<std::underlying_type_t<T>>(b)); }\
    constexpr inline T& operator^=(T& a, std::underlying_type_t<T> b) { return a = static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^ b); }\
    constexpr inline bool is_set(T a, T b) { return (a & b) == b; }\
    constexpr inline bool is_set(T a, std::underlying_type_t<T> b) { return (a & b) == b; }\
    constexpr inline bool is_set(std::underlying_type_t<T> a, T b) { return static_cast<T>(a & b) == b; }\
    constexpr inline T& set(T& a, T b) {return a |= b;} \
    constexpr inline T& set(T& a, std::underlying_type_t<T> b) {return a |= b;} \
    constexpr inline T& unset(T& a, T b) {return a &= ~b;} \
    constexpr inline T& unset(T& a, std::underlying_type_t<T> b) {return a &= ~b; } \
