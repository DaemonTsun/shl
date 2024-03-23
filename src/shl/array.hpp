
#pragma once

/* array.hpp

Contiguous dynamic memory structure.

    template<typename T>
    struct array
    {
        T *data;
        s64 size;
        s64 reserved_size;
        allocator allocator;
    }

Example usage:

    array<int> arr{};
    init(&arr, 3);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    add_at_end(&arr, 4);

    int x = 5;
    add_at_end(&arr, &x);

    int *ptr = add_at_end(&arr);
    *ptr = 6;

    for_array(i, val, &arr)
        printf("%lu: %d\n", i, *val);

    free(&arr);

Functions:

init(*arr)    Initializes an empty array with no elements. arr.data will be nullptr.
init(*arr, N) Initializes an array with N elements. the elements will be uninitialized.

add_at_start(*arr) Returns a pointer to a new element at the start of the array.
                   May allocate more memory if necessary.
add_at_start(*arr, V) Adds the value V at the start of the array, allocating more memory
                      if necessary.
                      Returns a pointer to the inserted value, which is always start.

add_at_end(*arr) Returns a pointer to a new element at the end of the array.
                 May allocate more memory if necessary.
add_at_end(*arr, V) Adds the value V at the end of the array, allocating more memory
                    if necessary.
                    Returns a pointer to the inserted value.

add_elements(*arr, N)
    Adds N elements to the array at the end and returns a pointer
    to the start of the new elements.
    May allocate more memory than needed to store N elements,
    or allocate nothing if the array has enough reserved memory.

insert_elements(*arr, pos, N)
    Inserts N elements at position pos in the array.
    If pos == arr.size, behaves like add_elements(arr, N).
    If pos > arr.size, does nothing and returns nullptr.
    Otherwise, adds N elements using add_elements,
    moves elements from [pos, arr.size[ to pos + N,
    then returns a pointer to where the elements
    were inserted.

add_range(*arr, *Elems, N) Adds N elements from Elems at the end of arr.
                           If Elems is nullptr, returns nullptr.
                           If N is 0, returns nullptr,
                           otherwise, returns pointer to the inserted elements
                           within arr.

add_range(*arr, *arr2) overload of add_range that adds all elements of arr2 to the
                       end of arr.

add_range(*arr, *arr2, Start, Count) add_range(arr, arr2->data + Start, Count)

insert_range(*arr, pos, *Elems, N)
    Adds N elements from Elems into arr at position pos.
    If pos == arr.size, behaves like add_range(arr, Elems, N).
    If pos > arr.size, does nothing and returns nullptr.
    If Elems is nullptr, returns nullptr.
    If N is 0, returns nullptr,
    otherwise, returns pointer to the inserted elements
    within arr.

insert_range(*arr, pos, *arr2) Overload of insert_range that inserts all elements of arr2
                               into arr at position pos.

insert_range(*arr, pos, *arr2, Start, Count) insert_range(arr, pos, arr2->data + Start, Count)

remove_from_start(*arr) Removes the first element from the start of the array,
                        shifting all others back. probably a heavy operation if
                        the array contains many elements. does nothing if the
                        array is empty.

remove_from_end(*arr) Reduces the size of the array by 1 (does not deallocate anything).
                      does nothing if the array is empty.

remove_elements(*arr, pos, N)
    Removes N elements starting at position pos from the array.
    Does nothing if pos >= arr.size.
    If pos + n >= arr.size, simply changes the size of
    the array and keeps the reserved memory.
    Use shrink_to_fit to remove the excess memory.

reserve(*arr, N) If number of allocated elements in arr is smaller than N,
                 allocates enough elements for arr to store N total elements
                 and sets arr.reserved_size accordingly.
                 arr.size is untouched.
                 Cannot make arr smaller.

reserve_exp2(*arr, N) Same as reserve, but if it allocates, it allocates
                      up to the next power of 2 to store at least N elements.

resize(*arr, N) Sets the size of the array to contain exactly N elements.

shrink_to_fit(*arr) Reallocates to only use as much memory as required
                    to store all the elements in the array if
                    more memory is being used.
                    Does nothing if arr.size == arr.reserved_size.

at(*arr, N) Returns a pointer to the Nth element in the array.

clear(*arr) Simply sets arr.size to 0, no memory is deallocated and
            reserved memory is kept.
            Use free(*arr) to deallocate memory.

array_data(*arr) Returns arr.data, pointer to the first element.
array_size(*arr) Returns arr.size.
    array_data and array_size are used in for_array to loop through an array or
    array-like object.

free_values(*arr) Calls free(*v) on each element in the array, but does
                  not deallocate memory of the array.

free(*arr) Frees memory of the array and sets arr.size and arr.reserved_size to 0.
           You may call init(*arr, size) after calling free(*arr).
           The difference between free and free_values is that free frees the
           memory of the _array_, whereas free_values frees the memory that
           the individual entries may have allocated.

other functions:

search(*arr, *key, eq) Returns a pointer to an element where eq(elem, key)
                       returns true to, otherwise returns nullptr if key
                       was not found. Does not assume anything about the
                       array and will do a full scan in the worst case.

index_of(*arr, *key, eq) Returns the index of an element where eq(elem, key)
                         returns true to, otherwise returns -1 if key
                         was not found. Does not assume anything about the
                         array and will do a full scan in the worst case.

contains(*arr, *key, eq) Returns true if key is in the array, false
                         otherwise. Does not assume anything about the
                         array and will do a full scan in the worst case.

hash(*arr) Returns the default hash of the _memory_ of the elements
           of the array.
           See shl/hash.hpp for details.

supports index operator: arr[0] == arr.data[0].

for_array(v, *arr) Iterate an array. v will be a pointer to an element in the array.
                   Example, setting all values to 5:

                   array<int> arr;
                   init(&arr, 3)
                   
                   for_array(v, &arr)
                   {
                       *v = 5;
                   }

for_array(i, v, *arr) Iterate an array. i will be the index of an element and
                      v will be a pointer to an element in the array.
 */

#include "shl/assert.hpp"
#include "shl/macros.hpp"
#include "shl/compare.hpp"
#include "shl/type_functions.hpp"
#include "shl/number_types.hpp"
#include "shl/allocator.hpp"
#include "shl/memory.hpp"
#include "shl/hash.hpp"
#include "shl/bits.hpp"

template<typename T>
struct array
{
    typedef T value_type;

    T *data;
    s64 size;
    s64 reserved_size;
    ::allocator allocator;

          T &operator[](s64 index)       { return data[index]; }
    const T &operator[](s64 index) const { return data[index]; }
};

template<typename T>
bool operator==(const array<T> &lhs, const array<T> &rhs)
{
    if (lhs.size != rhs.size)
        return false;

    for (s64 i = 0; i < lhs.size; ++i)
        if (!(lhs.data[i] == rhs.data[i]))
            return false;

    return true;
}

template<typename T>
void init(array<T> *arr, allocator a = default_allocator)
{
    assert(arr != nullptr);

    arr->data = nullptr;
    arr->size = 0;
    arr->reserved_size = 0;
    arr->allocator = a;
}

template<typename T>
void init(array<T> *arr, s64 n_elements, allocator a = default_allocator)
{
    assert(arr != nullptr);

    if (n_elements < 0)
        n_elements = 0;

    arr->data = allocator_alloc_T(a, T, n_elements);
    arr->size = n_elements;
    arr->reserved_size = n_elements;
    arr->allocator = a;
}

template<typename T>
T *add_elements(array<T> *arr, s64 n_elements)
{
    assert(arr != nullptr);

    if (n_elements <= 0)
        return nullptr;

    s64 nsize = arr->size + n_elements; 

    if (nsize < arr->reserved_size)
    {
        T *ret = arr->data + arr->size;
        arr->size = nsize;
        return ret;
    }

    s64 new_reserved_size = ceil_exp2(arr->reserved_size + n_elements);

    if (arr->allocator.alloc == nullptr)
        arr->allocator = default_allocator;

    T *n = allocator_realloc_T(arr->allocator, arr->data, T, arr->reserved_size, new_reserved_size);

    if (n == nullptr)
        return nullptr;

    arr->data = n;

    T *ret = arr->data;

    if (ret != nullptr)
        ret += arr->size;

    arr->size = nsize;
    arr->reserved_size = new_reserved_size;

    return ret;
}

template<typename T>
T *insert_elements(array<T> *arr, s64 index, s64 n_elements)
{
    assert(arr != nullptr);

    if (index < 0)
        return nullptr;
    else if (index == arr->size)
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

    s64 prev_size = arr->size;
    T *n = add_elements(arr, n_elements);

    if (n == nullptr)
        return nullptr;

    void *start = reinterpret_cast<void*>(arr->data + index);
    void *new_start = reinterpret_cast<void*>(arr->data + index + n_elements);
    s64 elem_count = prev_size - index;

    ::move_memory(start, new_start, elem_count * sizeof(T));

    return arr->data + index;
}

template<typename T>
T *add_range(array<T> *arr, const T *elements, s64 n_elements)
{
    assert(arr != nullptr);

    if (elements == nullptr || n_elements == 0)
        return nullptr;

    T *ret = add_elements(arr, n_elements);

    ::copy_memory((const void*)elements, (void*)ret, n_elements * sizeof(T));

    return ret;
}

template<typename T>
T *add_range(array<T> *arr, const array<T> *other)
{
    assert(arr != nullptr);
    assert(other != nullptr);
    return add_range(arr, other->data, other->size);
}

template<typename T>
T *add_range(array<T> *arr, const array<T> *other, s64 other_start, s64 count)
{
    assert(arr != nullptr);
    assert(other != nullptr);

    assert(other_start <= other->size);
    assert(count <= (other->size - other_start));
    return add_range(arr, other->data + other_start, count);
}

template<typename T>
T *insert_range(array<T> *arr, s64 index, const T *elements, s64 n_elements)
{
    assert(arr != nullptr);

    if (elements == nullptr || n_elements <= 0 || index < 0)
        return nullptr;

    T *ret = insert_elements(arr, index, n_elements);

    if (ret == nullptr)
        return ret;

    ::copy_memory((const void*)elements, (void*)ret, n_elements * sizeof(T));

    return ret;
}

template<typename T>
T *insert_range(array<T> *arr, s64 index, const array<T> *other)
{
    assert(arr != nullptr);
    assert(other != nullptr);

    return insert_range(arr, index, other->data, other->size);
}

template<typename T>
T *insert_range(array<T> *arr, s64 index, const array<T> *other, s64 other_start, s64 count)
{
    assert(arr != nullptr);
    assert(other != nullptr);

    assert(other_start <= other->size);
    assert(count <= (other->size - other_start));
    return insert_range(arr, index, other->data + other_start, count);
}

template<typename T>
inline T *add_at_start(array<T> *arr)
{
    return insert_elements(arr, 0, 1);
}

template<typename T>
inline T *add_at_start(array<T> *arr, T val)
{
    T *ret = add_at_start(arr);
    *ret = val;
    return ret;
}

template<typename T>
inline T *add_at_start(array<T> *arr, const T *val)
{
    T *ret = add_at_start(arr);
    *ret = *val;
    return ret;
}

template<typename T>
inline T *add_at_end(array<T> *arr)
{
    return add_elements(arr, 1);
}

template<typename T>
inline T *add_at_end(array<T> *arr, T val)
{
    T *ret = add_at_end(arr);
    *ret = val;
    return ret;
}

template<typename T>
inline T *add_at_end(array<T> *arr, const T *val)
{
    T *ret = add_at_end(arr);
    *ret = *val;
    return ret;
}

template<bool FreeValues = false, typename T>
void remove_elements(array<T> *arr, s64 index, s64 n_elements)
{
    assert(arr != nullptr);

    if (n_elements <= 0)
        return;

    if (index < 0 || index >= arr->size)
        return;

    if constexpr (FreeValues)
    {
        s64 max = Min(index + n_elements, arr->size);
        
        for (s64 i = index; i < max; ++i)
            free(arr->data + i);
    }

    if (index + n_elements >= arr->size)
    {
        arr->size = index;
        return;
    }

    void *before = reinterpret_cast<void*>(arr->data + index);
    void *after = reinterpret_cast<void*>(arr->data + index + n_elements);

    s64 num_items_after = arr->size - (index + n_elements);
    ::move_memory(after, before, num_items_after * sizeof(T));

    arr->size = arr->size - n_elements;
}

template<bool FreeValues = false, typename T>
inline void remove_from_start(array<T> *arr)
{
    remove_elements<FreeValues>(arr, 0, 1);
}

template<bool FreeValues = false, typename T>
inline void remove_from_end(array<T> *arr)
{
    remove_elements<FreeValues>(arr, arr->size - 1, 1);
}

template<typename T>
bool reserve(array<T> *arr, s64 size)
{
    assert(arr != nullptr);

    if (arr->reserved_size >= size)
        return true;

    if (arr->allocator.alloc == nullptr)
        arr->allocator = default_allocator;

    T *n = allocator_realloc_T(arr->allocator, arr->data, T, arr->reserved_size, size);

    if (n == nullptr)
        return false;

    arr->data = n;
    arr->reserved_size = size;

    return true;
}

template<typename T>
bool reserve_exp2(array<T> *arr, s64 size)
{
    return reserve(arr, ceil_exp2(size));
}

// if size makes array smaller and FreeValues is true, call free() on all
// removed values before reallocating memory.
template<bool FreeValues = false, typename T>
bool resize(array<T> *arr, s64 size)
{
    assert(arr != nullptr);

    if (arr->reserved_size == size)
    {
        arr->size = size;
        return true;
    }

    if constexpr (FreeValues)
    {
        if (arr->size > size)
        for (s64 i = size; i < arr->size; ++i)
            free(arr->data + i);
    }

    if (arr->allocator.alloc == nullptr)
        arr->allocator = default_allocator;

    T *n = allocator_realloc_T(arr->allocator, arr->data, T, arr->reserved_size, size);

    if (n == nullptr && size > 0)
        return false;

    arr->data = n;
    arr->size = size;
    arr->reserved_size = size;

    return true;
}

template<typename T>
bool shrink_to_fit(array<T> *arr)
{
    assert(arr != nullptr);

    if (arr->size == arr->reserved_size)
        return true;

    if (arr->allocator.alloc == nullptr)
        arr->allocator = default_allocator;

    T *n = allocator_realloc_T(arr->allocator, arr->data, T, arr->reserved_size, arr->size);

    if (n == nullptr)
        return false;

    arr->data = n;
    arr->reserved_size = arr->size;

    return true;
}

template<typename T>
T *begin(array<T> *arr)
{
    assert(arr != nullptr);

    return arr->data;
}

// past-the-end pointer
template<typename T>
T *end(array<T> *arr)
{
    assert(arr != nullptr);

    return arr->data + arr->size;
}

template<typename T>
T *at(array<T> *arr, s64 index)
{
    assert(arr != nullptr);
    assert(index < arr->size);

    return arr->data + index;
}

template<typename T>
const T *at(const array<T> *arr, s64 index)
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
T *array_data(array<T> *arr)
{
    assert(arr != nullptr);

    return arr->data;
}

template<typename T>
const T *array_data(const array<T> *arr)
{
    assert(arr != nullptr);

    return arr->data;
}

template<typename T>
s64 array_size(const array<T> *arr)
{
    assert(arr != nullptr);

    return arr->size;
}

#include "shl/impl/for_array.hpp"

template<typename T>
void free_values(array<T> *arr)
{
    assert(arr != nullptr);
    
    for_array(v, arr)
        free(v);
}

template<bool FreeValues = false, typename T>
void free(array<T> *arr)
{
    assert(arr != nullptr);

    if constexpr (FreeValues) free_values(arr);

    if (arr->data != nullptr)
    {
        if (arr->allocator.alloc == nullptr)
            arr->allocator = default_allocator;

        arr->data = allocator_dealloc_T(arr->allocator, arr->data, T, arr->reserved_size);
    }

    arr->size = 0;
    arr->reserved_size = 0;
}

template<typename T>
T *search(array<T> *arr, T key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_array(v, arr)
        if (eq(*v, key))
            return v;

    return nullptr;
}

template<typename T>
T *search(array<T> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_array(v, arr)
        if (eq(v, key))
            return v;

    return nullptr;
}

template<typename T>
s64 index_of(const array<T> *arr, T key, equality_function<T> eq = equals<T>)
{
    assert(arr != nullptr);
    
    for_array(i, v, arr)
        if (eq(*v, key))
            return i;

    return -1;
}

template<typename T>
s64 index_of(const array<T> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    assert(arr != nullptr);
    
    for_array(i, v, arr)
        if (eq(v, key))
            return i;

    return -1;
}

template<typename T>
bool contains(const array<T> *arr, T key, equality_function<T> eq = equals<T>)
{
    return index_of(arr, key, eq) != -1;
}

template<typename T>
bool contains(const array<T> *arr, const T *key, equality_function_p<T> eq = equals_p<T>)
{
    return index_of(arr, key, eq) != -1;
}

// TODO: sort

template<typename T>
hash_t hash(const array<T> *arr)
{
    return hash_data(reinterpret_cast<void*>(arr->data), arr->size * sizeof(T));
}
