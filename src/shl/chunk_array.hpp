
#pragma once

/* chunk_array.hpp

TODO: docs

for_chunk_array(*value, *arr)
for_chunk_array(index, *value, *arr)
for_chunk_array(index, *value, *chunk, *arr)
    iterates the chunk array arr. example:

    chunk_array<int, 32> arr;
    init(&arr);

    for_chunk_array(i, v, &arr)
        *v = i;

    free(&arr);
 */

#include "shl/array.hpp"

#define Default_chunk_size 32

struct chunk_item_index
{
    u32 chunk_index;
    u32 slot_index;
};

template<typename T, u64 N = Default_chunk_size>
struct chunk
{
    u32 index;
    u32 used_count;

    bool used[N];
    T data[N];
};

template<typename T, u64 N = Default_chunk_size>
struct chunk_array
{
    typedef T value_type;
    typedef chunk<T, N> chunk_type;

    static constexpr u64 chunk_size = N;

    array<chunk_type*> all_chunks;
    array<chunk_type*> nonfull_chunks;

    // number of items in the array
    u64 size;

    T &operator[](chunk_item_index index)       { return *at(this, index); }
    // const T &operator[](chunk_item_index index) const { return *at(this, index); }
};

template<typename T, u64 N>
void init(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    init(&arr->all_chunks);
    init(&arr->nonfull_chunks);

    arr->size = 0;
}

template<typename T, u64 N>
chunk<T, N> *add_chunk(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    chunk<T, N> **ptr = add_at_end(&arr->all_chunks);

    if (ptr == nullptr)
        return nullptr;

    *ptr = allocate_memory<chunk<T, N>>();
    chunk<T, N> *ret = *ptr;

    add_at_end(&arr->nonfull_chunks, ret);

    ret->index = arr->all_chunks.size - 1;
    ret->used_count = 0;

    fill_memory(ret->used, N, 0x00);

    return ret;
}

template<typename T>
struct chunk_slot_locator
{
    T *ptr;
    chunk_item_index index;
};

// allocates more if there's no more space
template<typename T, u64 N>
chunk_slot_locator<T> get_first_free_slot(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    if (arr->nonfull_chunks.size == 0)
        add_chunk(arr);

    chunk<T, N> *c = arr->nonfull_chunks[0];

    s32 index = -1;

    // maybe keep track of where the first free index is
    for (s32 i = 0; i < N; ++i)
        if (!c->used[i])
        {
            index = i;
            break;
        }

    c->used[index] = true;
    c->used_count += 1;

    if (c->used_count == N)
    {
        assert(arr->nonfull_chunks[0] == c);
        remove_from_start(&arr->nonfull_chunks);
    }

    arr->size += 1;

    chunk_slot_locator<T> res;
    res.ptr = c->data + index;
    res.index.chunk_index = c->index;
    res.index.slot_index = index;

    return res;
}

template<typename T, u64 N>
chunk_slot_locator<T> add_element(chunk_array<T, N> *arr, const T *val)
{
    assert(arr != nullptr);
    assert(val != nullptr);

    chunk_slot_locator loc = get_first_free_slot(arr);

    if (loc.ptr == nullptr)
        return loc;

    *loc.ptr = *val;

    return loc;
}

template<typename T, u64 N>
chunk_slot_locator<T> add_element(chunk_array<T, N> *arr, T val)
{
    assert(arr != nullptr);

    return add_element(arr, &val);
}

template<typename T, u64 N>
void remove_element(chunk_array<T, N> *arr, chunk_item_index index)
{
    assert(arr != nullptr);

    if (index.chunk_index >= arr->all_chunks.size)
        return;

    if (index.slot_index >= N)
        return;

    chunk<T, N> *c = arr->all_chunks[index.chunk_index];
    bool was_full = c->used_count == N;

    c->used[index.slot_index] = false;
    c->used_count -= 1;
    arr->size -= 1;

    if (was_full)
        add_at_end(&arr->nonfull_chunks, c);
}

template<typename T, u64 N>
chunk<T, N> **begin(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->all_chunks.data;
}

template<typename T, u64 N>
chunk<T, N> **end(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->all_chunks.data + arr->all_chunks.size;
}

template<typename T, u64 N>
T *at(chunk_array<T, N> *arr, chunk_item_index index)
{
    assert(arr != nullptr);

    chunk<T, N> *c = arr->all_chunks[index.chunk_index];

    assert(c->used[index.slot_index] == true);

    return c->data + index.slot_index;
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

    return arr->size;
}

template<typename T, u64 N>
u64 chunk_count(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->all_chunks.size;
}

#define for_chunk_array_IVC(I_Var, V_Var, Chunk_Var, ARRAY)\
    u64 I_Var = 0;\
    auto *Chunk_Var = array_data(&((ARRAY)->all_chunks));\
    auto *V_Var = array_data((*Chunk_Var)->data);\
    for (u64 Chunk_Var##_index = 0;\
        Chunk_Var##_index < chunk_count(ARRAY);\
        ++Chunk_Var##_index, ++Chunk_Var, V_Var = (Chunk_Var##_index < chunk_count(ARRAY) ? (*Chunk_Var)->data : nullptr))\
    for (u64 I_Var##_in_chunk = 0;\
         I_Var##_in_chunk < array_size((*Chunk_Var)->data);\
         ++I_Var##_in_chunk, V_Var = (*Chunk_Var)->data + I_Var##_in_chunk, ++I_Var)\
    if ((*Chunk_Var)->used[I_Var##_in_chunk])


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

    for_array(chnk, &arr->all_chunks)
        free_memory<chunk<T, N>>(*chnk);

    free(&arr->all_chunks);
    free(&arr->nonfull_chunks);
    arr->size = 0;
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

    for_chunk_array(v, arr)
        ret ^= hash_data(reinterpret_cast<void*>(v), sizeof(T));

    return ret;
}
