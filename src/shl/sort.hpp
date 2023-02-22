
#pragma once

/* sort.hpp
 *
 * provides sort(ptr, count, element_size, comparer), same interface as C qsort,
 * as well as sort<T>(ptr, count, comparer<T>) for typed sorting.
 *
 * default comparer is compare_ascending<T>(a, b) which just compares a and b using
 * < and > operators.
 *
 * also provides search(key, ptr, count, element_size, comparer), same interface as bsearch,
 * as well as search<T>(key, ptr, count, comparer<T>) for typed searching.
 * search returns a pointer within the range [ptr, ptr + count[ where key is, if key
 * is within the range, or nullptr otherwise.
 *
 * also provides index_of(key, ptr, count, comparer<T>), which returns the index in the
 * search range where key is, or -1 otherwise.
 *
 */

#include "shl/compare.hpp"
#include "shl/number_types.hpp"

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(...) do {} while (0);
#endif

void sort(void *ptr, u64 count, u64 size, compare_function<void> comp);

template<typename T>
void sort(T *ptr, u64 size, compare_function<T> comp = compare_ascending<T>)
{
    assert(ptr != nullptr);

    sort(reinterpret_cast<void*>(ptr), size, sizeof(T), (compare_function<void>)(comp));
}

// TODO: quicksort

void *search(const void *value, void *ptr, u64 count, u64 size, compare_function<void> comp);

template<typename T>
T *search(const T *value, T *ptr, u64 size, compare_function<T> comp = compare_descending<T>)
{
    assert(value != nullptr);
    assert(ptr != nullptr);

    return reinterpret_cast<T*>(search(reinterpret_cast<const void*>(value),
                                       reinterpret_cast<void*>(ptr),
                                       size,
                                       sizeof(T),
                                       (compare_function<void>)(comp)));
}

// TODO: binary_search

template<typename T>
bool contains(const T *value, T *ptr, u64 size, compare_function<T> comp = compare_descending<T>)
{
    return search(value, ptr, size, comp) != nullptr;
}

template<typename T>
u64 index_of(const T *value, T *ptr, u64 size, compare_function<T> comp = compare_descending<T>)
{
    assert(value != nullptr);
    assert(ptr != nullptr);

    T *res = search(value, ptr, size, comp);

    if (res == nullptr)
        return -1ull;

    return res - ptr;
}
