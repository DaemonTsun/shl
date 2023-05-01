
#pragma once

/* chunk_array.hpp

TODO: write
 */

#include "shl/array.hpp"

#define Default_chunk_size 32

// pointer to an array of Ts of size N...
template<typename T, u64 N = Default_chunk_size>
using array_chunk = T (*)[N];

template<typename T, u64 N = Default_chunk_size>
struct chunk_array
{
    typedef T value_type;
    typedef array_chunk<T, N> chunk_type;
    typedef array<array_chunk<T, N>> container_type;

    static constexpr u64 chunk_size = N;

    array<array_chunk<T, N>> chunks;

          T &operator[](u64 index)       { return *at(this, index); }
    const T &operator[](u64 index) const { return *at(this, index); }
};

template<typename T, u64 N>
void init(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    init(&arr->chunks);
}

template<typename T, u64 N>
void init(chunk_array<T, N> *arr, u64 n_elements)
{
    assert(arr != nullptr);

    init(arr);

    if (n_elements == 0)
        return;

    add_chunks(arr, n_elements);
}

template<typename T, u64 N>
inline array_chunk<T, N> *add_chunk_at_start(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return insert_chunks(arr, 0, 1);
}

template<typename T, u64 N>
inline array_chunk<T, N> *add_chunk_at_end(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return add_chunks(arr, 1);
}

template<typename T, u64 N>
void _alloc_chunks(array_chunk<T, N> *chunk, u64 n)
{
    for (u64 i = 0; i < n; ++i)
    {
        *chunk = allocate_memory<T[N]>();
        chunk++;
    }
}

template<typename T, u64 N>
void _free_chunks(array_chunk<T, N> *chunk, u64 n)
{
    for (u64 i = 0; i < n; ++i)
    {
        free_memory<T[N]>(*chunk);
        chunk++;
    }
}

template<typename T, u64 N>
array_chunk<T, N> *add_chunks(chunk_array<T, N> *arr, u64 n)
{
    assert(arr != nullptr);

    array_chunk<T, N> *ret = add_elements(&arr->chunks, n);

    if (ret == nullptr)
        return ret;

    _alloc_chunks(ret, n);

    return ret;
}

template<typename T, u64 N>
array_chunk<T, N> *insert_chunks(chunk_array<T, N> *arr, u64 index, u64 n)
{
    assert(arr != nullptr);

    array_chunk<T, N> *ret = insert_elements(&arr->chunks, index, n);

    if (ret == nullptr)
        return ret;

    _alloc_chunks(ret, n);

    return ret;
}

template<bool FreeValues = false, typename T, u64 N>
void remove_chunk_from_start(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    remove_chunks<FreeValues>(arr, 0, 1);
}

template<bool FreeValues = false, typename T, u64 N>
void remove_chunk_from_end(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    remove_chunks<FreeValues>(arr, arr->chunks.size - 1, 1);
}

// if FreeValues == true, calls free on ALL elements in removed chunks.
template<bool FreeValues = false, typename T, u64 N>
void remove_chunks(chunk_array<T, N> *arr, u64 index, u64 n)
{
    assert(arr != nullptr);

    if (n == 0)
        return;

    if (index >= arr->chunks.size)
        return;

    u64 max = Min(index + n, arr->chunks.size);

    if constexpr (FreeValues)
    {
        for (u64 i = index; i < max; ++i)
        {
            auto chunk = arr->chunks.data[i];

            for (u64 j = 0; j < N; ++j)
                free((*chunk) + j);
        }
    }

    auto first_chunk = arr->chunks.data + index;
    _free_chunks(first_chunk, max - index);

    remove_elements(&arr->chunks, index, n);
}

template<typename T, u64 N>
bool reserve(chunk_array<T, N> *arr, u64 n)
{
    assert(arr != nullptr);

    return reserve(&arr->chunks, n);
}

template<bool FreeValues = false, typename T, u64 N>
bool resize(chunk_array<T, N> *arr, u64 n)
{
    assert(arr != nullptr);

    if (arr->chunks.reserved_size == n)
    {
        arr->chunks.size = n;
        return true;
    }

    if constexpr (FreeValues)
    {
        if (arr->chunks.size > n)
        for (u64 i = n; i < arr->chunks.size; ++i)
        {
            auto chunk = arr->chunks.data[i];

            for (u64 j = 0; j < N; ++j)
                free((*chunk) + j);
        }
    }

    if (arr->chunks.size > n)
        _free_chunks(arr->chunks.data + n, arr->chunks.size - n);

    u64 prev_size = arr->chunks.size;
    bool ret = resize(&arr->chunks, n);

    if (ret && prev_size < n)
        _alloc_chunks(arr->chunks.data + prev_size, n - prev_size);

    return ret;
}

template<typename T, u64 N>
bool shrink_to_fit(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return shrink_to_fit(&arr->chunks);
}

template<typename T, u64 N>
array_chunk<T, N> *begin(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->chunks.data;
}

template<typename T, u64 N>
array_chunk<T, N> *end(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->chunks.data + arr->chunks.size;
}

#define _at_body(arr, index) \
{ \
    assert(arr != nullptr); \
    assert(index < arr->chunks.size * N); \
 \
    u64 chunk_index = index / N; \
    u64 index_within_chunk = index % N; \
 \
    return (*arr->chunks[chunk_index]) + index_within_chunk; \
}

// gets the nth element within the entire chunk_array,
// not the nth chunk
template<typename T, u64 N>
T *at(chunk_array<T, N> *arr, u64 index)
    _at_body(arr, index)

template<typename T, u64 N>
const T *at(const chunk_array<T, N> *arr, u64 index)
    _at_body(arr, index)

template<typename T, u64 N>
array_chunk<T, N> *nth_chunk(chunk_array<T, N> *arr, u64 index)
{
    assert(arr != nullptr);
    assert(index < arr->chunks.size);

    return at(&arr->chunks, index);
}

template<typename T, u64 N>
void clear(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    _free_chunks(arr->chunks.data, arr->chunks.size);
    clear(&arr->chunks);
}

// number of elements
template<typename T, u64 N>
u64 array_size(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->chunks.size * N;
}

template<typename T, u64 N>
u64 chunk_count(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->chunks.size;
}

#define for_chunk_array_IVC(I_Var, V_Var, Chunk_Var, ARRAY)\
    u64 I_Var = 0;\
    auto *Chunk_Var = array_data(&((ARRAY)->chunks));\
    auto *V_Var = array_data(*Chunk_Var);\
    for (u64 Chunk_Var##_index = 0;\
        Chunk_Var##_index < chunk_count(ARRAY);\
        ++Chunk_Var##_index, ++Chunk_Var, V_Var = (Chunk_Var##_index < chunk_count(ARRAY) ? *(*Chunk_Var) : nullptr))\
    for (u64 I_Var##_in_chunk = 0;\
         I_Var##_in_chunk < array_size(*Chunk_Var);\
         ++I_Var##_in_chunk, V_Var = *(*Chunk_Var) + I_Var##_in_chunk, ++I_Var)

#define for_chunk_array_IV(I_Var, V_Var, ARRAY)\
    for_chunk_array_IVC(I_Var, V_Var, V_Var##_chunk, ARRAY)

#define for_chunk_array_V(V_Var, ARRAY)\
    for_chunk_array_IVC(V_Var##_index, V_Var, V_Var##_chunk, ARRAY)

#define for_chunk_array(...) GET_MACRO3(__VA_ARGS__, for_chunk_array_IVC, for_chunk_array_IV, for_chunk_array_V)(__VA_ARGS__)

template<typename T, u64 N>
void free_values(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);
    
    for_chunk_array(v, arr)
        free(v);
}

template<bool FreeValues = false, typename T, u64 N>
void free(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    if constexpr (FreeValues) free_values(arr);

    _free_chunks(arr->chunks.data, arr->chunks.size);
    free<false>(&arr->chunks);
}

template<typename T, u64 N>
T *search(chunk_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(v, arr)
        if (eq(*v, key))
            return v;

    return nullptr;
}

template<typename T, u64 N>
T *search(chunk_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(v, arr)
        if (eq(v, key))
            return v;

    return nullptr;
}

template<typename T, u64 N>
s64 index_of(const chunk_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(i, v, arr)
        if (eq(*v, key))
            return i;

    return -1;
}

template<typename T, u64 N>
s64 index_of(const chunk_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(i, v, arr)
        if (eq(v, key))
            return i;

    return -1;
}

template<typename T, u64 N>
bool contains(const chunk_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    return index_of(arr, key, eq) != -1;
}

template<typename T, u64 N>
bool contains(const chunk_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    return index_of(arr, key, eq) != -1;
}

template<typename T, u64 N>
hash_t hash(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    u32 ret = 0;

    for_array(chunk, &arr->chunks)
        ret ^= hash_data(reinterpret_cast<void*>(*chunk), sizeof(T) * N);

    return ret;
}
