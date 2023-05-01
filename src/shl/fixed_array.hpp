
#pragma once

/* fixed_array.hpp
 * 
 * contiguous fixed memory structure.
 * 
 * functions:
 *
 * at(*arr, N) returns a pointer to the Nth element in the array.
 *
 * array_data(*arr) returns a pointer to the first element in the array
 * array_size(*arr) returns the size of the array
 *
 * other functions:
 *
 * search(*arr, *key, eq) returns a pointer to an element that eq(elem, key)
 *                        returns true to, otherwise returns nullptr if key
 *                        was not found. does not assume anything about the
 *                        array and will do a full scan in the worst case.
 *
 * index_of(*arr, *key, eq) returns the index of an element that eq(elem, key)
 *                          returns true to, otherwise returns -1 if key
 *                          was not found. does not assume anything about the
 *                          array and will do a full scan in the worst case.
 *
 * contains(*arr, *key, eq) returns true if key is in the array, false
 *                          otherwise. does not assume anything about the
 *                          array and will do a full scan in the worst case.
 *
 * hash(*arr) returns the default hash of the _memory_ of the elements
 *            of the array.
 *
 * supports index operator: arr[0] == arr.data[0].
 *
 * for_array(v, *arr) iterate an array. v will be a pointer to an element in the array.
 *                    example, setting all values to 5:
 *
 *                    array<int> arr;
 *                    init(&arr, 3)
 *                    
 *                    for_array(v, &arr)
 *                    {
 *                        *v = 5;
 *                    }
 *
 * for_array(i, v, *arr) iterate an array. i will be the index of an element and
 *                       v will be a pointer to an element in the array.
 */

#include "shl/macros.hpp"
#include "shl/compare.hpp"
#include "shl/type_functions.hpp"
#include "shl/number_types.hpp"
#include "shl/hash.hpp"
#include "shl/bits.hpp"

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(...) do {} while (0);
#endif

template<typename T, u64 N>
struct fixed_array
{
    typedef T value_type;
    static constexpr u64 size = N;

    T data[N];

    constexpr T &operator[](u64 index) { return data[index]; }
    constexpr const T &operator[](u64 index) const { return data[index]; }
};

template<typename T, typename... Ts>
fixed_array(T, Ts...) -> fixed_array<T, 1 + sizeof...(Ts)>;

template<typename T, u64 N>
constexpr T *at(fixed_array<T, N> *arr, u64 index)
{
    assert(arr != nullptr);
    assert(index < arr->size);

    return arr->data + index;
}

template<typename T, u64 N>
constexpr const T *at(const fixed_array<T, N> *arr, u64 index)
{
    assert(arr != nullptr);
    assert(index < arr->size);

    return arr->data + index;
}

template<typename T, u64 N>
constexpr T *array_data(fixed_array<T, N> *arr)
{
    return arr->data;
}

template<typename T, u64 N>
constexpr const T *array_data(const fixed_array<T, N> *arr)
{
    return arr->data;
}

template<typename T, u64 N>
constexpr u64 array_size(const fixed_array<T, N> *)
{
    return N;
}

#include "shl/impl/for_array.hpp"

#define search_body() \
{\
    assert(arr != nullptr);\
    \
    for_array(v, arr)\
        if (eq(*v, key))\
            return v;\
\
    return nullptr;\
}

#define search_body_p() \
{\
    assert(arr != nullptr);\
    \
    for_array(v, arr)\
        if (eq(v, key))\
            return v;\
\
    return nullptr;\
}

template<typename T, u64 N>
constexpr T *search(fixed_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
    search_body()

template<typename T, u64 N>
constexpr T *search(fixed_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
    search_body_p()

template<typename T, u64 N>
constexpr const T *search(const fixed_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
    search_body()

template<typename T, u64 N>
constexpr const T *search(const fixed_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
    search_body_p()

template<typename T, u64 N>
constexpr s64 index_of(const fixed_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_array(i, v, arr)
        if (eq(*v, key))
            return i;

    return -1;
}

template<typename T, u64 N>
constexpr s64 index_of(const fixed_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_array(i, v, arr)
        if (eq(v, key))
            return i;

    return -1;
}

template<typename T, u64 N>
constexpr bool contains(const fixed_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    return index_of(arr, key, eq) != -1;
}

template<typename T, u64 N>
constexpr bool contains(const fixed_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    return index_of(arr, key, eq) != -1;
}

// TODO: sort

template<typename T, u64 N>
constexpr hash_t hash(const fixed_array<T, N> *arr)
{
    return hash_data(reinterpret_cast<const void*>(arr->data), arr->size * sizeof(T));
}
