
#pragma once

/* array.hpp
 * 
 * contiguous memory structure.
 * 
 * functions:
 *
 * init(*arr) initializes an empty array with no elements. arr.data will be nullptr.
 * init(*arr, N) initializes an array with N elements. the elements will be uninitialized.
 *
 * add_elements(*arr, N) adds N elements to the array at the end and returns a pointer
 *                       to the start of the new elements.
 *                       may allocate more memory than needed to store N elements,
 *                       or allocate nothing if the array has enough reserved memory.
 *
 * insert_elements(*arr, pos, N) inserts N elements at position pos in the array.
 *                               if pos == arr.size, behaves like add_elements(arr, N).
 *                               if pos > arr.size, does nothing and returns nullptr.
 *                               otherwise, adds N elements using add_elements,
 *                               moves elements from [pos, arr.size[ to pos + N,
 *                               then returns a pointer to where the elements
 *                               were inserted.
 *
 * remove_elements(*arr, pos, N) removes N elements starting at position pos from the array.
 *                               does nothing if pos >= arr.size.
 *                               if pos + n >= arr.size, simply changes the size of
 *                               the array and keeps the reserved memory.
 *                               use shrink_to_fit to remove the excess memory.
 *
 * resize(*arr, N) sets the size of the array to contain exactly N elements.
 *
 * shink_to_fit(*arr) reallocates to only use as much memory as required
 *                    to store all the elements in the array if
 *                    more memory is being used.
 *                    does nothing if arr.size == arr.reserved_size.
 *
 * at(*arr, N) returns a pointer to the Nth element in the array.
 *
 * clear(*arr) simply sets arr.size to 0, no memory is deallocated and
 *             reserved memory is kept. use free(*arr) to deallocate memory.
 *
 * free(*arr) frees memory and sets arr.size and arr.reserved_size to 0.
 *            you may call init(*arr, size) after calling free(*arr).
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
#include "shl/memory.hpp"
#include "shl/hash.hpp"

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(...) do {} while (0);
#endif

template<typename T>
struct array
{
    typedef T value_type;

    T *data;
    u64 size;
    u64 reserved_size;

          T &operator[](u64 index)       { return data[index]; }
    const T &operator[](u64 index) const { return data[index]; }
};

template<typename T>
void init(array<T> *arr)
{
    assert(arr != nullptr);

    arr->data = nullptr;
    arr->size = 0;
    arr->reserved_size = 0;
}

template<typename T>
void init(array<T> *arr, u64 n_elements)
{
    assert(arr != nullptr);

    arr->data = allocate_memory<T>(n_elements);
    arr->size = n_elements;
    arr->reserved_size = n_elements;
}

template<typename T>
T *add_elements(array<T> *arr, u64 n_elements)
{
    assert(arr != nullptr);

    if (n_elements == 0)
        return nullptr;

    u64 nsize = arr->size + n_elements; 

    if (nsize < arr->reserved_size)
    {
        T *ret = arr->data + arr->size;
        arr->size = nsize;
        return ret;
    }

    u64 new_reserved_size = (arr->reserved_size + n_elements) * 2;

    arr->data = reallocate_memory<T>(arr->data, new_reserved_size);

    T *ret = arr->data;

    if (ret != nullptr)
        ret += arr->size;

    arr->size = nsize;
    arr->reserved_size = new_reserved_size;

    return ret;
}

template<typename T>
T *insert_elements(array<T> *arr, u64 index, u64 n_elements)
{
    assert(arr != nullptr);

    if (index == arr->size)
        return add_elements(arr, n_elements);
    else if (index > arr->size)
    {
        /* refusing to allocate beyond end.
         * we could add_elements(arr, n_elements + index - arr->size) but like
         * that seems unintuitive and can lead to accidental allocations.
         */
        return nullptr; 
    }

    if (n_elements == 0)
        return arr->data + index;

    u64 prev_size = arr->size;
    add_elements(arr, n_elements);

    void *start = reinterpret_cast<void*>(arr->data + index);
    void *new_start = reinterpret_cast<void*>(arr->data + index + n_elements);

    ::move_memory(start, new_start, n_elements * sizeof(T));

    return arr->data + index;
}

template<typename T>
void remove_elements(array<T> *arr, u64 index, u64 n_elements)
{
    assert(arr != nullptr);

    if (n_elements == 0)
        return;

    if (index >= arr->size)
        return;

    if (index + n_elements >= arr->size)
    {
        arr->size = index;
        return;
    }

    void *before = reinterpret_cast<void*>(arr->data + index);
    void *after = reinterpret_cast<void*>(arr->data + index + n_elements);

    u64 num_items_after = arr->size - (index + n_elements);
    ::move_memory(after, before, num_items_after * sizeof(T));

    arr->size = arr->size - n_elements;
}

template<typename T>
void resize(array<T> *arr, u64 size)
{
    assert(arr != nullptr);

    if (arr->reserved_size == size)
    {
        arr->size = size;
        return;
    }

    arr->data = reallocate_memory<T>(arr->data, size);
    arr->size = size;
    arr->reserved_size = size;
}

template<typename T>
void shrink_to_fit(array<T> *arr)
{
    assert(arr != nullptr);

    if (arr->size == arr->reserved_size)
        return;

    arr->data = reallocate_memory<T>(arr->data, arr->size);
    arr->reserved_size = arr->size;
}

template<typename T>
T *begin(array<T> *arr)
{
    assert(arr != nullptr);

    return arr->data;
}

template<typename T>
T *end(array<T> *arr)
{
    assert(arr != nullptr);

    return arr->data + arr->size;
}

template<typename T>
T *at(array<T> *arr, u64 index)
{
    assert(arr != nullptr);
    assert(index < arr->size);

    return arr->data + index;
}

template<typename T>
const T *at(const array<T> *arr, u64 index)
{
    assert(arr != nullptr);
    assert(index < arr->size);

    return arr->data + index;
}

template<typename T>
void clear(array<T> *arr)
{
    assert(arr != nullptr);

    arr->size = 0;
}

template<typename T>
void free(array<T> *arr)
{
    assert(arr != nullptr);

    if (arr->data != nullptr)
        free_memory<T>(arr->data);

    arr->data = nullptr;
    arr->size = 0;
    arr->reserved_size = 0;
}

#define _for_array_vars(I_Var, V_Var, LIST)\
    u64 I_Var = 0;\
    typename remove_pointer_t<decltype(LIST)>::value_type *V_Var = (LIST)->data;

#define for_array_V(V_Var, LIST)\
    _for_array_vars(V_Var##_index, V_Var, LIST)\
    for (; V_Var##_index < (LIST)->size; ++V_Var##_index, ++V_Var)

#define for_array_IV(I_Var, V_Var, LIST)\
    _for_array_vars(I_Var, V_Var, LIST)\
    for (; I_Var < (LIST)->size; ++I_Var, ++V_Var)

#define for_array(...) GET_MACRO2(__VA_ARGS__, for_array_IV, for_array_V)(__VA_ARGS__)

template<typename T>
T *search(array<T> *arr, const T *key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_array(v, arr)
        if (eq(v, key))
            return v;

    return nullptr;
}

template<typename T>
u64 index_of(const array<T> *arr, const T *key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    const T *ptr = search(arr, key, eq);

    if (ptr == nullptr)
        return -1ull;

    return ptr - arr->data;
}

template<typename T>
bool contains(const array<T> *arr, const T *key, equality_function<T> eq = equals<T>)
{
    return search(arr, key, eq) != nullptr;
}

// TODO: sort

template<typename T>
hash_t hash(const array<T> *arr)
{
    return hash_data(reinterpret_cast<void*>(arr->data), arr->size * sizeof(T));
}
