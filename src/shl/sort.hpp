
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

#include <assert.h>
#include "shl/compare.hpp"
#include "shl/number_types.hpp"

void sort(void *ptr, u64 count, u64 size, compare_function_p<void> comp);

template<typename T>
void sort(T *ptr, u64 size, compare_function_p<T> comp = compare_ascending_p<T>)
{
    assert(ptr != nullptr);

    sort(reinterpret_cast<void*>(ptr), size, sizeof(T), (compare_function_p<void>)(comp));
}

// TODO: quicksort

void *sorted_search(const void *value, void *ptr, u64 count, u64 size, compare_function_p<void> comp);

template<typename T>
T *sorted_search(const T *value, T *ptr, u64 size, compare_function_p<T> comp = compare_ascending_p<T>)
{
    assert(value != nullptr);
    assert(ptr != nullptr);

    return reinterpret_cast<T*>(sorted_search(reinterpret_cast<const void*>(value),
                                              reinterpret_cast<void*>(ptr),
                                              size,
                                              sizeof(T),
                                              (compare_function_p<void>)(comp)));
}

// TODO: binary_search

template<typename T>
bool sorted_contains(const T *value, T *ptr, u64 size, compare_function_p<T> comp = compare_ascending_p<T>)
{
    return sorted_search(value, ptr, size, comp) != nullptr;
}

template<typename T>
s64 sorted_index_of(const T *value, T *ptr, u64 size, compare_function_p<T> comp = compare_ascending_p<T>)
{
    assert(value != nullptr);
    assert(ptr != nullptr);

    T *res = sorted_search(value, ptr, size, comp);

    if (res == nullptr)
        return -1;

    return res - ptr;
}
