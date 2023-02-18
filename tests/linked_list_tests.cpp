
#include <t1/t1.hpp>

#include "shl/linked_list.hpp"

define_test(init_initializes_linked_list)
{
    linked_list<int> list;

    init(&list);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    free(&list);
}

define_test(init_initializes_array2)
{
    linked_list<int> list;

    init(&list, 1);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_equal(list.first->next, nullptr);
    assert_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.first, list.last);
    assert_equal(list.size, 1);

    free(&list);
}

define_test(init_initializes_array3)
{
    linked_list<int> list;

    init(&list, 5);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);

    free(&list);
}

define_test(nth_node_gets_the_nth_node)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list_node<int> *node = nth_node(&list, 0);
    assert_not_equal(node, nullptr);
    assert_equal(node, list.first);
    assert_equal(node, list.last->previous->previous->previous->previous);

    node = nth_node(&list, 1);
    assert_not_equal(node, nullptr);
    assert_equal(node, list.first->next);
    assert_equal(node, list.last->previous->previous->previous);

    node = nth_node(&list, 2);
    assert_not_equal(node, nullptr);
    assert_equal(node, list.first->next->next);
    assert_equal(node, list.last->previous->previous);

    node = nth_node(&list, 3);
    assert_not_equal(node, nullptr);
    assert_equal(node, list.first->next->next->next);
    assert_equal(node, list.last->previous);

    node = nth_node(&list, 4);
    assert_not_equal(node, nullptr);
    assert_equal(node, list.first->next->next->next->next);
    assert_equal(node, list.last);

    node = nth_node(&list, 5);
    assert_equal(node, nullptr);

    free(&list);
}

define_test(add_elements_does_nothing_when_adding_no_elements)
{
    linked_list<int> list;

    init(&list, 5);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);

    list_node<int> *node = add_elements(&list, 0);

    assert_equal(node, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);

    free(&list);
}

define_test(add_elements_adds_elements)
{
    linked_list<int> list;

    init(&list, 5);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);

    list_node<int> *previous_last = list.last;

    list_node<int> *node = add_elements(&list, 1);

    assert_not_equal(node, nullptr);
    assert_equal(node, previous_last->next);
    assert_equal(node, list.last);
    assert_equal(list.size, 6);

    free(&list);
}

define_test(add_elements_adds_elements2)
{
    linked_list<int> list;

    init(&list, 5);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);

    list_node<int> *previous_last = list.last;

    list_node<int> *node = add_elements(&list, 3);

    assert_not_equal(node, nullptr);
    assert_equal(node, previous_last->next);
    assert_equal(node, list.last->previous->previous);
    assert_equal(list.size, 8);

    free(&list);
}

define_test(add_elements_adds_elements_to_empty_list)
{
    linked_list<int> list;

    init(&list);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    list_node<int> *node = add_elements(&list, 5);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);

    assert_not_equal(node, nullptr);
    assert_equal(node, list.first);

    free(&list);
}

define_default_test_main();
