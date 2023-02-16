
#pragma once

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

    T* data;
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

    free_memory(static_cast<void*>(arr->data));

    arr->data = nullptr;
}
