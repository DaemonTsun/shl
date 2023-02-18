
#pragma once

#include "shl/number_types.hpp"
#include "shl/memory.hpp"

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

    list_node<T> *first;
    list_node<T> *last;
    u64 size;

    // TODO: T &operator[](u64 index) { return ???? }
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
list_node<T> *nth_node(linked_list<T> *list, u64 n)
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
