
#pragma once

/* linked_list.hpp
 * 
 * non-contiguous linked memory structure.
 * contains nodes, each node contains one element, access via node.value.
 * each node has a previous and next pointer to the previous and next node respectively.
 *
 * initialize a list with init(*list, size), free with free(*list).
 *
 * add_elements(*list, N) adds N elements to the list at the end and returns a pointer
 *                        to the first node inserted.
 *
 * insert_elements(*list, pos, N) inserts N elements at position pos in the list.
 *                                if pos == list.size, behaves like add_elements(list, N).
 *                                if pos > list.size, does nothing and returns nullptr.
 *                                otherwise, inserts nodes at position pos and chains
 *                                them correctly to the neighboring nodes.
 *                                if pos == 0, sets list.first accordingly.
 *                                if pos == list.size, sets list.last accordingly.
 *                                returns a pointer to the first node inserted.
 *
 * remove_elements(*list, pos, N) removes N nodes starting at position pos from the list.
 *                                does nothing if pos >= list.size.
 *                                if pos + n >= list.size, only removes nodes up to list.last.
 *                                if pos == 0, sets list.first accordingly.
 *                                if pos + n >= list.size, sets list.last accordingly.
 *
 * resize(*list, N) sets the size of the list to contain exactly N elements.
 *                  allocates new nodes if size is greater than list.size.
 *                  removes and deallocates nodes starting from the back
 *                  if size is smaller than list.size.
 *                  does nothing if size == list.size.
 *
 * nth_node(*list, N) returns a pointer to the Nth node (not not element) in the list.
 *
 * at(*list, N) returns a pointer to the Nth element (not node) in the list.
 *
 * clear(*list) frees all nodes and sets list.first, list.last to nullptr,
 *              and sets list.size to 0.
 *
 * free(*list) identical to clear(*list).
 *
 * supports index operator: list[0] == nth_node(&list, 0)->value.
 *
 * other functions:
 *
 * search_node(*list, *key, eq) returns a pointer to the node that eq(node.value, key)
 *                              returns true to, otherwise returns nullptr if key
 *                              was not found. does not assume anything about the
 *                              list and will do a full scan in the worst case.
 *
 * search(*list, *key, eq) returns a pointer to an element that eq(elem, key)
 *                         returns true to, otherwise returns nullptr if key
 *                         was not found. does not assume anything about the
 *                         list and will do a full scan in the worst case.
 *
 * index_of(*list, *key, eq) returns the index of an element that eq(elem, key)
 *                           returns true to, otherwise returns -1 if key
 *                           was not found. does not assume anything about the
 *                           list and will do a full scan in the worst case.
 *
 * contains(*list, *key, eq) returns true if key is in the list, false
 *                           otherwise. does not assume anything about the
 *                           list and will do a full scan in the worst case.
 *
 * hash(*list) returns the default hash of the _memory_ of the _elements_
 *             stored in the list.
 *
 * for_list(v, *list) iterate a list. v will be a pointer to an element in the list.
 *                    example, setting all values to 5:
 *
 *                    linked_list<int> list;
 *                    init(&list, 3)
 *                    
 *                    for_list(v, &list)
 *                    {
 *                        *v = 5;
 *                    }
 *
 * for_list(i, v, *list) iterate a list. i will be the index of an element and
 *                       v will be a pointer to an element in the list.
 *
 * for_list(i, v, n, *list) iterate a list. i will be the index of an element and
 *                          v will be a pointer to an element and
 *                          n will be a pointer to a node in the list.
 */

#include "shl/compare.hpp"
#include "shl/macros.hpp"
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
struct list_node
{
    typedef T value_type;

    list_node<T> *next;
    list_node<T> *previous;

    T value;
};

template<typename T>
struct linked_list
{
    typedef T value_type;
    typedef list_node<T> node_type;

    list_node<T> *first;
    list_node<T> *last;
    u64 size;

          T &operator[](u64 index)       { return nth_node(this, index)->value; }
    const T &operator[](u64 index) const { return nth_node(this, index)->value; }
};

template<typename T>
void init(linked_list<T> *list)
{
    assert(list != nullptr);

    list->first = nullptr;
    list->last = nullptr;
    list->size = 0;
}

template<typename T>
void init(linked_list<T> *list, u64 n_elements)
{
    assert(list != nullptr);

    init(list);

    if (n_elements == 0)
        return;

    add_elements(list, n_elements);
}

template<typename T>
list_node<T> *nth_node(const linked_list<T> *list, u64 n)
{
    assert(list != nullptr);

    if (n >= list->size)
        return nullptr;

    list_node<T> *ret = nullptr;
    u64 half = list->size / 2;

    if (n < half)
    {
        ret = list->first;
        while (n > 0 && ret != nullptr)
        {
            ret = ret->next;
            n--;
        }
    }
    else
    {
        ret = list->last;
        while (n < list->size - 1 && ret != nullptr)
        {
            ret = ret->previous;
            n++;
        }
    }

    return ret;
}

template<typename T>
list_node<T> *add_elements(linked_list<T> *list, u64 n_elements)
{
    assert(list != nullptr);

    if (n_elements == 0)
        return nullptr;

    list_node<T> *ret = nullptr;
    u64 new_elements_to_add = n_elements;

    if (list->last == nullptr)
    {
        assert(list->first == nullptr);

        list->first = allocate_memory<list_node<T>>();
        list->first->next = nullptr;
        list->first->previous = nullptr;
        list->last = list->first;

        ret = list->last;
        new_elements_to_add--;
    }
    
    list_node<T> *n = list->last;

    for (u64 i = 0; i < new_elements_to_add; ++i)
    {
        list_node<T> *tmp = allocate_memory<list_node<T>>();
        tmp->previous = n;
        tmp->next = nullptr;
        n->next = tmp;
        n = tmp;

        if (ret == nullptr)
            ret = tmp;
    }

    list->last = n;
    list->size = list->size + n_elements;

    return ret;
}

template<typename T>
list_node<T> *insert_elements(linked_list<T> *list, u64 index, u64 n_elements)
{
    assert(list != nullptr);

    if (n_elements == 0)
        return nullptr;

    if (index > list->size)
        return nullptr;

    if (index == list->size)
        return add_elements(list, n_elements);

    list_node<T> *after = nth_node(list, index);
    list_node<T> *n = after->previous;
    list_node<T> *ret = nullptr;

    for (u64 i = 0; i < n_elements; ++i)
    {
        list_node<T> *tmp = allocate_memory<list_node<T>>();
        tmp->previous = n;
        tmp->next = after;

        if (n != nullptr)
            n->next = tmp;
        else
            list->first = tmp;

        if (ret == nullptr)
            ret = tmp;

        n = tmp;
    }

    after->previous = n;
    list->size = list->size + n_elements;

    return ret;
}

template<typename T>
void remove_elements(linked_list<T> *list, u64 index, u64 n_elements)
{
    assert(list != nullptr);

    if (n_elements == 0)
        return;

    if (index >= list->size)
        return;

    list_node<T> *node = nth_node(list, index);
    list_node<T> *before_start = node->previous;

    u64 i = 0;
    while (i < n_elements && node != nullptr)
    {
        list_node<T> *next = node->next;
        free_memory<list_node<T>>(node);
        node = next;

        i++;
    }

    if (before_start == nullptr)
    {
        list->first = node;

        if (list->first != nullptr)
            list->first->previous = nullptr;
    }
    else
        before_start->next = node;

    if (node == nullptr)
    {
        list->last = before_start;

        if (list->last != nullptr)
            list->last->next = nullptr;
    }
    else
        node->previous = before_start;

    list->size -= i;
}

template<typename T>
void resize(linked_list<T> *list, u64 n_elements)
{
    assert(list != nullptr);

    if (n_elements == list->size)
        return;

    if (n_elements > list->size)
    {
        add_elements(list, n_elements - list->size);
        return;
    }

    if (n_elements == 0)
    {
        free(list);
        return;
    }

    remove_elements(list, n_elements, list->size - n_elements);
}

template<typename T>
T *at(linked_list<T> *list, u64 n)
{
    assert(list != nullptr);

    if (n >= list->size)
        return nullptr;

    return &nth_node(list, n)->value;
}

template<typename T>
const T *at(const linked_list<T> *list, u64 n)
{
    assert(list != nullptr);

    if (n >= list->size)
        return nullptr;

    return &nth_node(list, n)->value;
}

template<typename T>
void clear(linked_list<T> *list)
{
    assert(list != nullptr);

    list_node<T> *n = list->last;

    while (n != nullptr)
    {
        list_node<T> *tmp = n->previous;
        free_memory<list_node<T>>(n);
        n = tmp;
    }

    list->first = nullptr;
    list->last = nullptr;
    list->size = 0;
}

template<typename T>
void free(linked_list<T> *list)
{
    clear(list);
}

#define _for_list_vars(V_Var, N_Var, LIST)\
    typename remove_pointer_t<decltype(LIST)>::node_type  *N_Var = (LIST)->first;\
    typename remove_pointer_t<decltype(LIST)>::value_type *V_Var = N_Var ? &(N_Var->value) : nullptr;

#define for_list_V(V_Var, LIST)\
    _for_list_vars(V_Var, V_Var##_node, LIST)\
    for (; V_Var##_node != nullptr; V_Var##_node = V_Var##_node->next, V_Var = &V_Var##_node->value)

#define for_list_IV(I_Var, V_Var, LIST)\
    u64 I_Var = 0;\
    _for_list_vars(V_Var, I_Var##V_Var##_node, LIST)\
    for (; I_Var##V_Var##_node != nullptr; I_Var##V_Var##_node = I_Var##V_Var##_node->next, ++I_Var, V_Var = &I_Var##V_Var##_node->value)

#define for_list_IVN(I_Var, V_Var, N_Var, LIST)\
    u64 I_Var = 0;\
    _for_list_vars(V_Var, N_Var, LIST)\
    for (; N_Var != nullptr; N_Var = N_Var->next, ++I_Var, V_Var = &N_Var->value)

#define for_list(...) GET_MACRO3(__VA_ARGS__, for_list_IVN, for_list_IV, for_list_V)(__VA_ARGS__)

template<typename T>
list_node<T> *search_node(linked_list<T> *list, const T *key, equality_function<T> eq = equals<T>)
{
    assert(list != nullptr);
    
    for_list(v, list)
        if (eq(v, key))
            return v_node;

    return nullptr;
}

template<typename T>
T *search(linked_list<T> *list, const T *key, equality_function<T> eq = equals<T>)
{
    list_node<T> *n = search_node(list, key, eq);

    if (n == nullptr)
        return nullptr;

    return &n->value;
}

template<typename T>
u64 index_of(const linked_list<T> *list, const T *key, equality_function<T> eq = equals<T>)
{
    assert(list != nullptr);
    
    for_list(i, v, list)
        if (eq(v, key))
            return i;

    return -1ull;
}

template<typename T>
bool contains(const linked_list<T> *list, const T *key, equality_function<T> eq = equals<T>)
{
    return index_of(list, key, eq) != -1ull;
}

template<typename T>
hash_t hash(const linked_list<T> *list)
{
    hash_t ret = 0;

    for_list(v, list)
        ret = hash_data(reinterpret_cast<void*>(v), sizeof(T), ret);

    return ret;
}
