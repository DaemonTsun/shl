
#pragma once

/* set.hpp

contiguous, sorted dynamic memory structure with unique elements.

functions:

init(*st, comp) initializes an empty set with no elements. st.data will be nullptr.
              comp is the compare function to use when arranging the elements of the set.
              comp can be ascending or descending.
init(*st, N, comp) initializes an set with N elements. the elements will be uninitialized.
                 comp is the compare function to use when arranging the elements of the set.

add_elements(*st, N) adds N elements to the set at the end and returns a pointer
                      to the start of the new elements.
                      may allocate more memory than needed to store N elements,
                      or allocate nothing if the set has enough reserved memory.

remove_elements(*st, pos, N) removes N elements starting at position pos from the set.
                              does nothing if pos >= st.size.
                              if pos + n >= st.size, simply changes the size of
                              the set and keeps the reserved memory.
                              use shrink_to_fit to remove the excess memory.

reserve(*st, N) if number of allocated elements in st is smaller than N,
                 allocates enough elements for st to store N total elements
                 and sets st.reserved_size accordingly.
                 st.size is untouched.
                 cannot make st smaller.

reserve_exp2(*st, N) same as reserve, but if it allocates, it allocates
                      up to the next power of 2 to store at least N elements.

resize(*st, N) sets the size of the set to contain exactly N elements.

shrink_to_fit(*st) reallocates to only use as much memory as required
                   to store all the elements in the set if
                   more memory is being used.
                   does nothing if st.size == st.reserved_size.

at(*st, N) returns a pointer to the Nth element in the set.

clear(*st) simply sets st.size to 0, no memory is deallocated and
            reserved memory is kept. use free(*st) to deallocate memory.

set_data(*st) returns st.data, pointer to the first element
set_size(*st) returns st.size
array_data(*st) returns st.data, pointer to the first element
array_size(*st) returns st.size

free_values(*st) calls free(*v) on each element in the set, but does
                  not deallocate memory of the set.

free(*st) frees memory of the set and sets st.size and st.reserved_size to 0.
           the difference between free and free_values is that free frees the
           memory of the _set_, whereas free_values frees the memory that
           the individual entries may have allocated.

other functions:

search(*st, *key) returns a pointer to an element that eq(elem, key)
                       returns true to, otherwise returns nullptr if key
                       was not found. does not assume anything about the
                       set and will do a full scan in the worst case.

index_of(*st, *key) returns the index of an element that eq(elem, key)
                         returns true to, otherwise returns -1 if key
                         was not found. does not assume anything about the
                         set and will do a full scan in the worst case.

contains(*st, *key, eq) returns true if key is in the set, false
                         otherwise. does not assume anything about the
                         set and will do a full scan in the worst case.

hash(*st) returns the default hash of the _memory_ of the elements
           of the set.

supports index operator: st[0] == st.data[0].

for_array(v, *st) iterate an set. v will be a pointer to an element in the set.
                example, setting all values to 5:

                set<int> st;
                init(&st, 3)
                
                for_array(v, &st)
                {
                    *v = 5;
                }

for_array(i, v, *st) iterate an set. i will be the index of an element and
                   v will be a pointer to an element in the set.
 */

#include <assert.h>

#include "shl/macros.hpp"
#include "shl/compare.hpp"
#include "shl/type_functions.hpp"
#include "shl/number_types.hpp"
#include "shl/memory.hpp"
#include "shl/hash.hpp"
#include "shl/array.hpp"
#include "shl/sort.hpp"
#include "shl/bits.hpp"

template<typename T>
struct set
{
    typedef T value_type;

    array<T> data;
    compare_function_p<T> compare;

          T &operator[](u64 index)       { return data[index]; }
    const T &operator[](u64 index) const { return data[index]; }
};

template<typename T>
bool operator==(const set<T> &lhs, const set<T> &rhs)
{
    return lhs.compare == rhs.compare && lhs.data == rhs.data;
}

template<typename T>
void init(set<T> *st, compare_function_p<T> comp = compare_ascending_p<T>)
{
    assert(st != nullptr);

    if (comp == nullptr)
        comp = compare_ascending_p<T>;

    init(&st->data);
    st->compare = comp;
}

template<typename T>
void init(set<T> *st, u64 reserve_size, compare_function_p<T> comp = compare_ascending_p<T>)
{
    assert(st != nullptr);

    init(st, comp);
    reserve(&st->data, reserve_size);
}

struct nearest_search_result
{
    s64 index;
    int last_comparison;
};

// binary search the value, then return the index where it
// WOULD be inserted at / is at.
// also returns the last comparison result, if result is 0
// then the value was found and the index is exactly where it
// is in the set.
template<typename T>
nearest_search_result nearest_index_of(const set<T> *st, const T *val)
{
    assert(st != nullptr);
    assert(val != nullptr);

    s64 l = 0;
    s64 r = array_size(st) - 1;
    s64 m = 0;
    int last_comp = 0;

    while (l <= r)
    {
        m = l + (r - l) / 2;

        last_comp = st->compare(val, st->data.data + m);

        if (last_comp == 0)
            break;
 
        if (last_comp > 0)
            l = m + 1;
        else
        {
            if (m == 0)
                break;

            r = m - 1;
        }
    }

    if (last_comp > 0)
        m++;

    return nearest_search_result{.index = m, .last_comparison = last_comp};
}

template<typename T>
T *insert_element(set<T> *st, const T *val)
{
    assert(st != nullptr);
    assert(val != nullptr);

    if (st->compare == nullptr)
        init(st);

    if (st->data.size == 0)
        return add_at_end(&st->data, val);

    nearest_search_result result = nearest_index_of(st, val);
 
    // if found, just return the one found
    if (result.last_comparison == 0)
        return st->data.data + result.index;

    T *ret = nullptr;

    if (result.index >= st->data.size)
        ret = add_at_end(&st->data);
    else
        ret = insert_elements(&st->data, result.index, 1);

    if (ret != nullptr)
        *ret = *val;

    return ret;
}

template<typename T>
T *insert_element(set<T> *st, T val)
{
    return insert_element(st, &val);
}

template<bool FreeValues = false, typename T>
void remove_element(set<T> *st, const T *val)
{
    assert(st != nullptr);

    nearest_search_result result = nearest_index_of(st, val);

    if (result.index < 0 || result.last_comparison != 0)
        return;

    remove_elements<FreeValues>(&st->data, result.index, 1);
}

template<bool FreeValues = false, typename T>
void remove_element(set<T> *st, T val)
{
    assert(st != nullptr);

    return remove_element<FreeValues>(st, &val);
}

template<bool FreeValues = false, typename T>
void remove_elements_at_index(set<T> *st, u64 index, u64 n_elements)
{
    assert(st != nullptr);

    remove_elements<FreeValues>(&st->data, index, n_elements);
}

template<typename T>
bool reserve(set<T> *st, u64 size)
{
    assert(st != nullptr);

    return reserve(&st->data, size);
}

template<typename T>
bool reserve_exp2(set<T> *st, u64 size)
{
    return reserve_exp2(&st->data, size);
}

// if size makes set smaller and FreeValues is true, call free() on all
// removed values before reallocating memory.
template<bool FreeValues = false, typename T>
bool resize(set<T> *st, u64 size)
{
    assert(st != nullptr);

    return resize<FreeValues>(&st->data, size);
}

template<typename T>
bool shrink_to_fit(set<T> *st)
{
    assert(st != nullptr);

    return shrink_to_fit(&st->data);
}

template<typename T>
T *begin(set<T> *st)
{
    assert(st != nullptr);

    return st->data->data;
}

// past-the-end pointer
template<typename T>
T *end(set<T> *st)
{
    assert(st != nullptr);

    return st->data->data + st->size;
}

template<typename T>
T *at(set<T> *st, u64 index)
{
    assert(st != nullptr);
    assert(index < st->data.size);

    return st->data.data + index;
}

template<typename T>
const T *at(const set<T> *st, u64 index)
{
    assert(st != nullptr);
    assert(index < st->data.size);

    return st->data.data + index;
}

template<typename T>
void clear(set<T> *st)
{
    assert(st != nullptr);

    st->data.size = 0;
}

template<typename T>
T *set_data(set<T> *st)
{
    assert(st != nullptr);

    return st->data.data;
}

template<typename T>
const T *set_data(const set<T> *st)
{
    assert(st != nullptr);

    return st->data.data;
}

template<typename T>
u64 set_size(const set<T> *st)
{
    assert(st != nullptr);

    return st->data.size;
}

template<typename T>
T *array_data(set<T> *st)
{
    assert(st != nullptr);

    return st->data.data;
}

template<typename T>
const T *array_data(const set<T> *st)
{
    assert(st != nullptr);

    return st->data.data;
}

template<typename T>
u64 array_size(const set<T> *st)
{
    assert(st != nullptr);

    return st->data.size;
}

template<typename T>
void free_values(set<T> *st)
{
    assert(st != nullptr);
    
    for_array(v, st)
        free(v);
}

template<bool FreeValues = false, typename T>
void free(set<T> *st)
{
    assert(st != nullptr);

    free<FreeValues>(&st->data);
    st->compare = nullptr;
}

template<typename T>
T *search(const set<T> *st, T key)
{
    assert(st != nullptr);

    nearest_search_result result = nearest_index_of(st, &key);

    if (result.last_comparison == 0)
        return st->data.data + result.index;

    return nullptr;
}

template<typename T>
T *search(const set<T> *st, const T *key)
{
    assert(st != nullptr);
    
    nearest_search_result result = nearest_index_of(st, key);

    if (result.last_comparison == 0)
        return st->data.data + result.index;

    return nullptr;
}

template<typename T>
s64 index_of(const set<T> *st, T key)
{
    assert(st != nullptr);

    nearest_search_result result = nearest_index_of(st, &key);

    if (result.last_comparison == 0)
        return result.index;

    return -1;
}

template<typename T>
s64 index_of(const set<T> *st, const T *key)
{
    assert(st != nullptr);

    nearest_search_result result = nearest_index_of(st, key);
    
    if (result.last_comparison == 0)
        return result.index;

    return -1;
}

template<typename T>
bool contains(const set<T> *st, T key)
{
    return search(st, key) != nullptr;
}

template<typename T>
bool contains(const set<T> *st, const T *key)
{
    return search(st, key) != nullptr;
}

template<typename T>
hash_t hash(const set<T> *st)
{
    return hash(&st->data);
}
