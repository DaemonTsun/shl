
#pragma once

/* chunk_array.hpp

A dynamic data structure of fixed-size chunks that doesn't move memory.

Elements must be added using add_element before they may be accessed in
the chunk_array.
The chunks are stored in chunk_array->all_chunks, the number of elements
in the array are in chunk_array->size.
Items are accessed not by a single index but by a combination of
chunk index and slot index within the chunk.
Alternatively, the chunk_array defines the index operator to access
the Nth chunk, and chunks define the index operator to access the Nth
slot within the chunk.

Example:

    chunk_array<int, 4> arr{};
    add_element(&arr, 5);

    printf("%d\n", arr[0][0]);

    free(&arr);

Chunks are not guaranteed to be next to each other in memory, but a
chunks elements are contiguous.

Functions:

init(*arr): Initializes the chunk_array.

add_element(*arr, Val): Inserts the value Val into the array at the first
                        free slot in a nonfull chunk.
                        The nonfull chunk may not be the first nonfull chunk in the
                        array.
                        Returns a pointer to the inserted element as well as the index
                        to the element in the array.

remove_element(*arr, index): Removes an element from the array by item index.
                             Chunk_item_index is a struct containing the chunk and
                             slot index.
                             Example:
                             remove_element(&arr, {.chunk_index = 0, .slot_index = 0});

at(*arr, index): Returns a pointer to an element in the array by item index.
                 If the index is out of range (chunk or slot), return nullptr.
                 If the element at the given index is not used (i.e. not added
                 with add_element), also returns nullptr.
                 
clear(*arr): Marks all chunks as empty and resets size, but does not deallocate
             any memory.

free_values(*arr): Calls free() on every used element in the array.

free(*arr): Frees the memory of the chunks and the array and resets the data structure.
free<True>(*arr): Frees the memory of the elements, chunks and the array and resets
                  the data structure. Use when storing e.g. strings.

search/index_of/contains/hash: As usual, index_of yields an item index.

for_chunk_array(*value, *arr)
for_chunk_array(index, *value, *arr)
for_chunk_array(index, *value, *chunk, *arr)
    Iterates the used elements of a chunk_array. example:

    chunk_array<int, 4> arr{};
    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);
    add_element(&arr, 4);
    add_element(&arr, 5);

    // there's 8 allocated values, but only the 5 used will be
    // iterated.
    for_chunk_array(i, v, &arr)
        printf("[%d:%d]: %d\n", i.chunk_index, i.slot_index, *v);

    free(&arr);
 */

#include "shl/array.hpp"

#define Default_chunk_size 32

struct chunk_item_index
{
    s32 chunk_index;
    s32 slot_index;
};

template<typename T, s64 N = Default_chunk_size>
struct chunk
{
    s32 index;
    s32 used_count;

    bool used[N];
    T data[N];

    T &operator[](s64 i)       { return data[i]; }
};

template<typename T, s64 N = Default_chunk_size>
struct chunk_array
{
    typedef T value_type;
    typedef chunk<T, N> chunk_type;

    static constexpr s64 chunk_size = N;

    array<chunk_type*> all_chunks;
    array<chunk_type*> nonfull_chunks;

    // number of items in the array
    s64 size;

    ::allocator allocator;

    chunk_type &operator[](s64 index)     { return *all_chunks[index]; }
    T &operator[](chunk_item_index index) { return all_chunks[index.chunk_index]->data[index.slot_index]; }
};

template<typename T, s64 N>
void init(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    init(&arr->all_chunks);
    init(&arr->nonfull_chunks);

    arr->size = 0;
    arr->allocator = get_context_pointer()->allocator;
}

template<typename T, s64 N>
chunk<T, N> *add_chunk(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    chunk<T, N> **ptr = add_at_end(&arr->all_chunks);

    if (ptr == nullptr)
        return nullptr;

    _set_allocator_if_not_set(arr);

    *ptr = (chunk<T, N>*)allocator_alloc(arr->allocator, sizeof(chunk<T, N>));
    chunk<T, N> *ret = *ptr;

    add_at_end(&arr->nonfull_chunks, ret);

    ret->index = (u32)(arr->all_chunks.size - 1);
    ret->used_count = 0;

    fill_memory(ret->used, 0x00, N);

    return ret;
}

template<typename T>
struct chunk_slot_locator
{
    T *ptr;
    chunk_item_index index;
};

// allocates more if there's no more space
template<typename T, s64 N>
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

template<typename T, s64 N>
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

template<typename T, s64 N>
chunk_slot_locator<T> add_element(chunk_array<T, N> *arr, T val)
{
    assert(arr != nullptr);

    return add_element(arr, &val);
}

template<typename T, s64 N>
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

template<typename T, s64 N>
chunk<T, N> **begin(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->all_chunks.data;
}

template<typename T, s64 N>
chunk<T, N> **end(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->all_chunks.data + arr->all_chunks.size;
}

template<typename T, s64 N>
T *at(chunk_array<T, N> *arr, chunk_item_index index)
{
    assert(arr != nullptr);

    if (index.slot_index >= N)
        return nullptr;

    if (index.chunk_index >= arr->all_chunks.size)
        return nullptr;

    chunk<T, N> *c = arr->all_chunks[index.chunk_index];

    if (c->used[index.slot_index] != true)
        return nullptr;

    return c->data + index.slot_index;
}

template<typename T, s64 N>
void clear(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    clear(&arr->nonfull_chunks);

    for_array(chnk_, &arr->all_chunks)
    {
        chunk<T, N> *chnk = *chnk_;

        for_array(used, &chnk->used)
            *used = false;

        chnk->used_count = 0;
        add_at_end(&arr->nonfull_chunks, chnk);
    }

    arr->size = 0;
}

// number of elements
template<typename T, s64 N>
s64 array_size(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->size;
}

template<typename T, s64 N>
s64 chunk_count(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    return arr->all_chunks.size;
}

#define for_chunk_array_IVC(I_Var, V_Var, Chunk_Var, ARRAY)\
    if constexpr (chunk_item_index I_Var{}; true)\
    if constexpr (auto *Chunk_Var = array_data(&((ARRAY)->all_chunks)); true)\
    if constexpr (auto *V_Var = array_data(&(*Chunk_Var)->data); true)\
    for (I_Var.chunk_index = 0;\
        I_Var.chunk_index < chunk_count(ARRAY);\
        ++I_Var.chunk_index, ++Chunk_Var, V_Var = (I_Var.chunk_index < chunk_count(ARRAY) ? (*Chunk_Var)->data : nullptr))\
    for (I_Var.slot_index = 0;\
         I_Var.slot_index < array_size(&(*Chunk_Var)->data);\
         ++I_Var.slot_index, V_Var = (*Chunk_Var)->data + I_Var.slot_index)\
    if ((*Chunk_Var)->used[I_Var.slot_index])


#define for_chunk_array_IV(I_Var, V_Var, ARRAY)\
    for_chunk_array_IVC(I_Var, V_Var, V_Var##_chunk, ARRAY)

#define for_chunk_array_V(V_Var, ARRAY)\
    for_chunk_array_IVC(V_Var##_index, V_Var, V_Var##_chunk, ARRAY)

#define for_chunk_array(...) GET_MACRO3(__VA_ARGS__, for_chunk_array_IVC, for_chunk_array_IV, for_chunk_array_V)(__VA_ARGS__)

template<typename T, s64 N>
void free_values(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);
    
    for_chunk_array(v, arr)
        free(v);
}

template<bool FreeValues = false, typename T, s64 N>
void free(chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    if constexpr (FreeValues) free_values(arr);

    for_array(chnk, &arr->all_chunks)
        allocator_dealloc(arr->allocator, *chnk, sizeof(chunk<T, N>));

    free(&arr->all_chunks);
    free(&arr->nonfull_chunks);
    arr->size = 0;
}

template<typename T, s64 N>
T *search(chunk_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(v, arr)
        if (eq(*v, key))
            return v;

    return nullptr;
}

template<typename T, s64 N>
T *search(chunk_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(v, arr)
        if (eq(v, key))
            return v;

    return nullptr;
}

template<typename T, s64 N>
chunk_item_index index_of(const chunk_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_chunk_array(i, v, arr)
        if (eq(v, key))
            return i;

    return {.chunk_index = (s32)-1, .slot_index = (s32)-1};
}

template<typename T, s64 N>
chunk_item_index index_of(const chunk_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    return index_of(arr, &key, eq);
}

template<typename T, s64 N>
bool contains(const chunk_array<T, N> *arr, T key, equality_function<T> eq = equals<T>)
{
    return index_of(arr, key, eq) != -1;
}

template<typename T, s64 N>
bool contains(const chunk_array<T, N> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    return index_of(arr, key, eq) != -1;
}

template<typename T, s64 N>
hash_t hash(const chunk_array<T, N> *arr)
{
    assert(arr != nullptr);

    u32 ret = 0;

    for_chunk_array(v, arr)
        ret ^= hash_data(reinterpret_cast<void*>(v), sizeof(T));

    return ret;
}
