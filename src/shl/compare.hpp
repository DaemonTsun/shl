
#pragma once

/* compare.hpp
 *
 * defines function types for comparing and checking equality of
 * pointers of typed data.
 *
 * compare_function<T> = int(*)(const T a, const T b)
 *      returns -1 if a < b
 *               0 if a == b
 *               1 if a > b
 *
 * equality_function = bool(*)(const T a, const T b)
 *      returns true if a == b
 *              false otherwise
 *
 * _p variants for pointers.
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

#define Min(A, B) ((A) < (B) ? (A) : (B))
#define Max(A, B) ((A) > (B) ? (A) : (B))

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
