
#pragma once

/* sort.hpp
 *
 * provides sort(ptr, count, element_size, comparer), same interface as C qsort,
 * as well as sort<T>(ptr, count, comparer<T>) for typed sorting.
 *
 * default comparer is ascending_comparer<T>(a, b) which just compares a and b using
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

#include "shl/number_types.hpp"

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(...) do {} while (0);
#endif

template<typename T>
using comparator_function = int (*)(const T*, const T*);

template<typename T>
int ascending_comparer(const T *a, const T *b)
{
    T a_val = *a;
    T b_val = *b;

    if (a_val < b_val) return -1;
    if (a_val > b_val) return 1;
    return 0;
}

template<typename T>
int descending_comparer(const T *a, const T *b)
{
    return -ascending_comparer(a, b);
}

void sort(void *ptr, u64 count, u64 size, comparator_function<void> comp);

template<typename T>
void sort(T *ptr, u64 size, comparator_function<T> comp = ascending_comparer<T>)
{
    assert(ptr != nullptr);

    sort(reinterpret_cast<void*>(ptr), size, sizeof(T), (comparator_function<void>)(comp));
}

// TODO: quicksort

void *search(const void *value, void *ptr, u64 count, u64 size, comparator_function<void> comp);

template<typename T>
T *search(const T *value, T *ptr, u64 size, comparator_function<T> comp = descending_comparer<T>)
{
    assert(value != nullptr);
    assert(ptr != nullptr);

    return reinterpret_cast<T*>(search(reinterpret_cast<const void*>(value),
                                       reinterpret_cast<void*>(ptr),
                                       size,
                                       sizeof(T),
                                       (comparator_function<void>)(comp)));
}

// TODO: binary_search

template<typename T>
u64 index_of(const T *value, T *ptr, u64 size, comparator_function<T> comp = descending_comparer<T>)
{
    assert(value != nullptr);
    assert(ptr != nullptr);

    T *res = search(value, ptr, size, comp);

    if (res == nullptr)
        return -1ull;

    return res - ptr;
}
