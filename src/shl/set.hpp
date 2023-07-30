
#pragma once

/* set.hpp

contiguous, sorted dynamic memory structure with unique elements.

Use set_data(*set) or array_data(*set) to get a pointer to the elements of the set.
Use set_size(*set) or array_size(*set) to get the size of the set.

functions:

init(*st, comp) initializes an empty set with no elements. st.data will be nullptr.
                comp is the compare function to use when arranging the elements of the set.
                comp can be ascending or descending.
init(*st, N, comp) initializes an set with N reserved elements.
                   comp is the compare function to use when arranging the elements of the set.

nearest_index_of(*st, Val) returns the nearest index of the given value Val and
                           the last binary search comparison. if the comparison is
                           0, Val is inside the set and the index is the index inside
                           the set. If the comparison is anything but 0, index is the
                           index where Val would be if Val was inserted into the set.

insert_element(*st, Val) inserts value Val into the set at the correct, sorted place.
                         Uses st->compare to determine the order. If Val is already
                         inside the set, Val is _not_ inserted again.
                         Returns a pointer to an element in the set which has the
                         value Val.

remove_element(*st, Val) removes an element X from the set if
                         st->compare(Val, X) == 0.
                         Does nothing if Val is not in the set.

remove_elements_at_index(*st, pos, N)
                         removes N elements starting at position pos from the set.
                         does nothing if pos >= st.data.size.
                         if pos + n >= st.data.size, simply changes the size of
                         the set and keeps the reserved memory.
                         use shrink_to_fit to remove the excess memory.

reserve(*st, N) if number of allocated elements in st is smaller than N,
                 allocates enough elements for st to store N total elements
                 and sets st.reserved_size accordingly.
                 st.data.size is untouched.
                 cannot make st smaller.

reserve_exp2(*st, N) same as reserve, but if it allocates, it allocates
                      up to the next power of 2 to store at least N elements.

resize(*st, N) sets the size of the set to contain exactly N elements.
               truncates from the right if N < st.data.size.
               ps: I can't think of a reason to use resize with N > st.data.size;
               uninitialized elements in a set cause huge issues because you can't
               really compare them, so only use this when absolutely necessary.

shrink_to_fit(*st) reallocates to only use as much memory as required
                   to store all the elements in the set if
                   more memory is being used.

at(*st, N) returns a pointer to the Nth element in the set.

clear(*st) simply sets the size of the set to 0, no memory is deallocated and
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

search(*st, Val) does a binary search on the set and returns the element X
                 for which st->compare(Val, X) == 0, or nullptr if Val
                 is not found in the set.

index_of(*st, Val) does a binary search on the set and returns the index of
                 an element X for which st->compare(Val, X) == 0,
                 or nullptr if Val is not found in the set.

contains(*st, Val) returns true if Val is inside the set, false if not.

hash(*st) returns the default hash of the _memory_ of the elements
          of the set.

supports index operator: st[0] == st.data[0].

for_array works on sets, too. examples from array.hpp:
for_array(v, *st) iterate a set. v will be a pointer to an element in the set.
                  example, setting all values to 5:

                  set<int> st;
                  init(&st, 3)
                  
                  for_array(v, &st)
                  {
                      *v = 5;
                  }

for_array(i, v, *st) iterate a set. i will be the index of an element and
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

// binary search the value, then return the index where it
// WOULD be inserted at / is at.
// also returns the last comparison result, if result is 0
// then the value was found and the index is exactly where it
// is in the set.
template<typename T1, typename T2>
binary_search_result nearest_index_of(const set<T1> *st, const T2 *val, compare_function_p<T2, T1> comp)
{
    assert(st != nullptr);
    assert(val != nullptr);

    binary_search_result res = binary_search(st->data.data, array_size(st), val, comp); 

    if (res.last_comparison > 0)
        res.index++;

    return res;
}

template<typename T1, typename T2>
binary_search_result nearest_index_of(const set<T1> *st, T2 val, compare_function_p<T2, T1> comp)
{
    return nearest_index_of(st, &val, comp);
}

template<typename T>
binary_search_result nearest_index_of(const set<T> *st, const T *val)
{
    assert(st != nullptr);
    assert(val != nullptr);

    return nearest_index_of(st, val, st->compare);
}

template<typename T>
binary_search_result nearest_index_of(const set<T> *st, T val)
{
    assert(st != nullptr);
    assert(val != nullptr);

    return nearest_index_of(st, &val, st->compare);
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

    binary_search_result result = nearest_index_of(st, val);
 
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

template<bool FreeValues = false, typename T1, typename T2>
void remove_element(set<T1> *st, const T2 *val, compare_function_p<T2, T1> comp)
{
    assert(st != nullptr);

    binary_search_result result = nearest_index_of(st, val, comp);

    if (result.index < 0 || result.last_comparison != 0)
        return;

    remove_elements<FreeValues>(&st->data, result.index, 1);
}

template<bool FreeValues = false, typename T1, typename T2>
void remove_element(set<T1> *st, T2 val, compare_function_p<T2, T1> comp)
{
    remove_element<FreeValues>(st, &val, st->compare);
}

template<bool FreeValues = false, typename T>
void remove_element(set<T> *st, const T *val)
{
    remove_element<FreeValues>(st, val, st->compare);
}

template<bool FreeValues = false, typename T>
void remove_element(set<T> *st, T val)
{
    remove_element<FreeValues>(st, &val, st->compare);
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

template<typename T1, typename T2>
T1 *search(const set<T1> *st, const T2 *key, compare_function_p<T2, T1> comp)
{
    assert(st != nullptr);
    assert(key != nullptr);
    
    binary_search_result result = nearest_index_of(st, key, comp);

    if (result.last_comparison == 0)
        return st->data.data + result.index;

    return nullptr;
}

template<typename T1, typename T2>
T1 *search(const set<T1> *st, T2 key, compare_function_p<T2, T1> comp)
{
    return search(st, &key, comp);
}

template<typename T>
T *search(const set<T> *st, const T *key)
{
    return search(st, key, st->compare);
}

template<typename T>
T *search(const set<T> *st, T key)
{
    return search(st, &key, st->compare);
}

template<typename T1, typename T2>
s64 index_of(const set<T1> *st, const T2 *key, compare_function_p<T2, T1> comp)
{
    assert(st != nullptr);

    binary_search_result result = nearest_index_of(st, key, comp);

    if (result.last_comparison == 0)
        return result.index;

    return -1;
}

template<typename T1, typename T2>
s64 index_of(const set<T1> *st, T2 key, compare_function_p<T2, T1> comp)
{
    return index_of(st, &key, comp);
}

template<typename T>
s64 index_of(const set<T> *st, const T *key)
{
    return index_of(st, key, st->compare);
}

template<typename T>
s64 index_of(const set<T> *st, T key)
{
    return index_of(st, &key, st->compare);
}

template<typename T1, typename T2>
bool contains(const set<T1> *st, const T2 *key, compare_function_p<T2, T1> comp)
{
    return search(st, key, comp) != nullptr;
}

template<typename T1, typename T2>
bool contains(const set<T1> *st, T2 key, compare_function_p<T2, T1> comp)
{
    return search(st, key, comp) != nullptr;
}

template<typename T>
bool contains(const set<T> *st, T key)
{
    return contains(st, key, st->compare);
}

template<typename T>
bool contains(const set<T> *st, const T *key)
{
    return contains(st, key, st->compare);
}

template<typename T>
hash_t hash(const set<T> *st)
{
    return hash(&st->data);
}
