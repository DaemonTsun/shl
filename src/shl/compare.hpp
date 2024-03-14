
#pragma once

/* compare.hpp

Comparison functions and types.

Defines function types for comparing and checking equality of
pointers of typed data.

compare_function<T> = int(*)(const T a, const T b)
     returns -1 if a < b
              0 if a == b
              1 if a > b

equality_function = bool(*)(const T a, const T b)
     returns true if a == b
             false otherwise

_p variants for pointers.

Min(X, Y)        If X < Y, returns X, otherwise returns Y.
Max(X, Y)        If X > Y, returns X, otherwise returns Y.
Clamp(Val, X, Y) If Val < X, returns X; if Val > Y, returns Y; otherwise returns Val.
*/

template<typename T1, typename T2 = T1>
using compare_function = int (*)(T1, T2);

template<typename T1, typename T2 = T1>
using compare_function_p = int (*)(const T1*, const T2*);

template<typename T1, typename T2 = T1>
constexpr int compare_ascending(T1 a, T2 b)
{
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

template<typename T1, typename T2 = T1>
constexpr int compare_descending(T1 a, T2 b)
{
    return -compare_ascending(a, b);
}

template<typename T1, typename T2 = T1>
constexpr int compare_ascending_p(const T1 *a, const T2 *b)
{
    return compare_ascending(*a, *b);
}

template<typename T1, typename T2 = T1>
constexpr int compare_descending_p(const T1 *a, const T2 *b)
{
    return compare_descending(*a, *b);
}

template<typename T, typename TMin = T, typename TMax = T>
constexpr int compare_clamp_ascending(T val, TMin min, TMax max)
{
    if (val >= min && val <= max)
        return 0;
    else if (val < min)
        return -1;
    else
        return 1;
}

template<typename T, typename TMin = T, typename TMax = T>
constexpr int compare_clamp_descending(T val, TMin min, TMax max)
{
    return -compare_clamp_ascending(val, min, max);
}

template<typename T>
constexpr T Min(T lhs, T rhs)
{
    if (lhs < rhs)  return lhs;
    else            return rhs;
}

template<typename T>
constexpr T Max(T lhs, T rhs)
{
    if (lhs > rhs)  return lhs;
    else            return rhs;
}

template<typename T>
constexpr T Clamp(T val, T min, T max)
{
    return Min(Max(val, min), max);
}

template<typename T1, typename T2 = T1>
using equality_function = bool (*)(T1, T2);

template<typename T1, typename T2 = T1>
using equality_function_p = bool (*)(const T1*, const T2*);

template<typename T1, typename T2 = T1>
constexpr bool equals(T1 a, T2 b)
{
    return a == b;
}

template<typename T1, typename T2 = T1>
constexpr bool equals_p(const T1 *a, const T2 *b)
{
    return (a == b) || equals(*a, *b);
}

template<typename T1, typename T2 = T1>
constexpr bool safe_equals_p(const T1 *a, const T2 *b)
{
    return (a == b) || ((a != nullptr && b != nullptr) && equals(*a, *b));
}
