
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

    if (n_elements == 0)
    {
        init(list);
        return;
    }

    list->size = n_elements;

    list->first = allocate_memory<list_node<T>>();
    list->first->next = nullptr;
    list->first->previous = nullptr;

    list_node<T> *n = list->first;

    for (u64 i = 1; i < n_elements; ++i)
    {
        list_node<T> *tmp = allocate_memory<list_node<T>>();
        tmp->previous = n;
        tmp->next = nullptr;
        n->next = tmp;
        n = tmp;
    }

    list->last = n;
}

template<typename T>
void free(linked_list<T> *list)
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
