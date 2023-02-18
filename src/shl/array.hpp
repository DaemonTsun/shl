
#pragma once

/* array.hpp
 * 
 * contiguous memory structure.
 * initialize with init(*arr, size), free with free(*arr).
 *
 * resize(*arr, size) sets the size of the array to the exact size size.
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
 *                               does nothing if pos >= arr.pos.
 *                               if pos + n >= arr.pos, simply changes the size of
 *                               the array and keeps the reserved memory.
 *                               use shrink_to_fit to remove the excess memory.
 *
 * shink_to_fit(*arr) reallocates to only use as much memory as required
 *                    to store all the elements in the array if
 *                    more memory is being used.
 *                    does nothing if arr.size == arr.reserved_size.
 *
 * supports index operator: arr[0] == arr.data[0].
 */

#include "shl/number_types.hpp"
#include "shl/memory.hpp"

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

    T &operator[](u64 index) { return data[index]; }
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
void init(array<T> *arr, u64 size)
{
    assert(arr != nullptr);

    arr->data = reinterpret_cast<T*>(allocate_memory(sizeof(T) * size));
    arr->size = size;
    arr->reserved_size = size;
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

    arr->data = reinterpret_cast<T*>(reallocate_memory(arr->data, sizeof(T) * new_reserved_size));

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

    if (arr->size == 0)
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

    arr->data = reinterpret_cast<T*>(reallocate_memory(arr->data, sizeof(T) * size));
    arr->size = size;
    arr->reserved_size = size;
}

template<typename T>
void shrink_to_fit(array<T> *arr)
{
    assert(arr != nullptr);

    if (arr->size == arr->reserved_size)
        return;

    arr->data = reinterpret_cast<T*>(reallocate_memory(arr->data, sizeof(T) * arr->size));
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
void free(array<T> *arr)
{
    assert(arr != nullptr);

    if (arr->data == nullptr)
        return;

    free_memory(reinterpret_cast<void*>(arr->data));

    arr->data = nullptr;
}
