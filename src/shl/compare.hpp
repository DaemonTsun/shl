
#pragma once

/* compare.hpp
 *
 * defines function types for comparing and checking equality of
 * pointers of typed data.
 *
 * compare_function<T> = int(*)(const T *a, const T *b)
 *      returns -1 if a < b
 *               0 if a == b
 *               1 if a > b
 *
 * equality_function = bool(*)(const T *a, const T *b)
 *      returns true if a == b
 *              false otherwise
 */

template<typename T>
using compare_function = int (*)(const T*, const T*);

template<typename T>
int compare_ascending(const T *a, const T *b)
{
    T a_val = *a;
    T b_val = *b;

    if (a_val < b_val) return -1;
    if (a_val > b_val) return 1;
    return 0;
}

template<typename T>
int compare_descending(const T *a, const T *b)
{
    return -compare_ascending(a, b);
}

template<typename T>
using equality_function = bool (*)(const T*, const T*);

template<typename T>
bool equals(const T *a, const T *b)
{
    return (a == b) || (*a == *b);
}

template<typename T>
bool safe_equals(const T *a, const T *b)
{
    return (a == b) || ((a != nullptr && b != nullptr) && (*a == *b));
}
