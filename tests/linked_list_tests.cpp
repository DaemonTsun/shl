
#include <t1/t1.hpp>

#include "shl/linked_list.hpp"

template<typename T>
void assert_list_integrity(const linked_list<T> *list)
{
    for_list(i, v, node, list)
    {
        if (node->previous != nullptr)
        {
            assert_equal(node->previous->next, node);
        }
        else
            assert_equal(node, list->first);

        if (node->next != nullptr)
        {
            assert_equal(node->next->previous, node);
        }
        else
            assert_equal(node, list->last);
    }
}

define_test(init_initializes_linked_list)
{
    linked_list<int> list;

    init(&list);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    free(&list);
}

define_test(init_initializes_list2)
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

define_test(init_initializes_list3)
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
    assert_list_integrity(&list);

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
    assert_list_integrity(&list);

    list_node<int> *node = add_elements(&list, 0);

    assert_equal(node, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

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
    assert_list_integrity(&list);

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
    assert_list_integrity(&list);

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
    assert_list_integrity(&list);

    free(&list);
}

define_test(remove_elements_does_nothing_when_removing_no_elements)
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

    remove_elements(&list, 0, 0);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

    free(&list);
}

define_test(remove_elements_does_nothing_when_removing_elements_outside_of_list_size)
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

    remove_elements(&list, 8, 0);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

    free(&list);
}

define_test(remove_elements_does_nothing_when_removing_from_empty_list)
{
    linked_list<int> list;

    init(&list);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    remove_elements(&list, 0, 0);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    free(&list);
}

define_test(remove_elements_removes_elements)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 1, 1);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 4);
    assert_list_integrity(&list);

    assert_equal(list[0], 1);
    assert_equal(list[1], 3);
    assert_equal(list[2], 4);
    assert_equal(list[3], 5);

    free(&list);
}

define_test(remove_elements_removes_elements2)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 0, 1);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 4);
    assert_list_integrity(&list);

    assert_equal(list[0], 2);
    assert_equal(list[1], 3);
    assert_equal(list[2], 4);
    assert_equal(list[3], 5);

    free(&list);
}

define_test(remove_elements_removes_elements3)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 4, 1);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 4);
    assert_list_integrity(&list);

    assert_equal(list[0], 1);
    assert_equal(list[1], 2);
    assert_equal(list[2], 3);
    assert_equal(list[3], 4);

    free(&list);
}

define_test(remove_elements_removes_elements4)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 4, 10);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 4);
    assert_list_integrity(&list);

    assert_equal(list[0], 1);
    assert_equal(list[1], 2);
    assert_equal(list[2], 3);
    assert_equal(list[3], 4);

    free(&list);
}

define_test(remove_elements_removes_elements5)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 3, 10);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    assert_equal(list[0], 1);
    assert_equal(list[1], 2);
    assert_equal(list[2], 3);

    free(&list);
}

define_test(remove_elements_removes_elements6)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 0, 5);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);
    assert_list_integrity(&list);

    free(&list);
}

define_test(remove_elements_removes_elements7)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;
    list[3] = 4;
    list[4] = 5;

    remove_elements(&list, 3, 1);

    assert_equal(list[0], 1);
    assert_equal(list[1], 2);
    assert_equal(list[2], 3);
    assert_equal(list[3], 5);
    assert_equal(list.first->value, 1);
    assert_equal(list.last->value, 5);

    remove_elements(&list, 1, 1);

    assert_equal(list[0], 1);
    assert_equal(list[1], 3);
    assert_equal(list[2], 5);
    assert_equal(list.first->value, 1);
    assert_equal(list.last->value, 5);

    remove_elements(&list, 2, 1);

    assert_equal(list[0], 1);
    assert_equal(list[1], 3);
    assert_equal(list.first->value, 1);
    assert_equal(list.last->value, 3);

    remove_elements(&list, 0, 1);

    assert_equal(list[0], 3);
    assert_equal(list.first->value, 3);
    assert_equal(list.last->value, 3);

    remove_elements(&list, 0, 1);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);
    assert_list_integrity(&list);

    free(&list);
}

define_test(insert_elements_inserts_into_empty_list)
{
    linked_list<int> list;

    init(&list);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    list_node<int> *node = insert_elements(&list, 0, 3);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    assert_equal(node, list.first);

    free(&list);
}

define_test(insert_elements_does_nothing_when_inserting_no_elements)
{
    linked_list<int> list;

    init(&list, 3);

    assert_equal(list.size, 3);

    list_node<int> *node = insert_elements(&list, 0, 0);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);

    assert_equal(node, nullptr);

    free(&list);
}

define_test(insert_elements_does_nothing_when_inserting_outside_list_bounds)
{
    linked_list<int> list;

    init(&list, 3);

    assert_equal(list.size, 3);

    list_node<int> *node = insert_elements(&list, 8, 2);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);

    assert_equal(node, nullptr);

    free(&list);
}

define_test(insert_elements_inserts_elements_at_the_start)
{
    linked_list<int> list;

    init(&list, 3);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;

    assert_equal(list.size, 3);

    list_node<int> *node = insert_elements(&list, 0, 2);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

    assert_equal(node, list.first);
    assert_equal(list[2], 1);
    assert_equal(list[3], 2);
    assert_equal(list[4], 3);

    free(&list);
}

define_test(insert_elements_inserts_elements_at_the_end)
{
    linked_list<int> list;

    init(&list, 3);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;

    assert_equal(list.size, 3);

    list_node<int> *node = insert_elements(&list, 3, 2);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

    assert_equal(node, list.last->previous);
    assert_equal(list[0], 1);
    assert_equal(list[1], 2);
    assert_equal(list[2], 3);

    free(&list);
}

define_test(insert_elements_inserts_elements_in_between)
{
    linked_list<int> list;

    init(&list, 3);

    list[0] = 1;
    list[1] = 2;
    list[2] = 3;

    assert_equal(list.size, 3);

    list_node<int> *node = insert_elements(&list, 1, 2);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

    assert_equal(node, list.first->next);
    assert_equal(list[0], 1);
    assert_equal(list[3], 2);
    assert_equal(list[4], 3);

    free(&list);
}

define_test(resize_creates_list_on_empty_list)
{
    linked_list<int> list;

    init(&list);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    resize(&list, 3);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    free(&list);
}

define_test(resize_deallocates_entire_list_when_resizing_to_zero_elements)
{
    linked_list<int> list;

    init(&list, 3);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    resize(&list, 0);

    assert_equal(list.first, nullptr);
    assert_equal(list.last, nullptr);
    assert_equal(list.size, 0);

    free(&list);
}

define_test(resize_does_nothing_when_resizing_to_same_size_as_list)
{
    linked_list<int> list;

    init(&list, 3);

    list_node<int> *previous_first = list.first;
    list_node<int> *previous_last = list.last;

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    resize(&list, 3);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    assert_equal(list.first, previous_first);
    assert_equal(list.last, previous_last);

    free(&list);
}

define_test(resize_allocates_nodes_when_resizing_to_larger_size)
{
    linked_list<int> list;

    init(&list, 3);

    list_node<int> *previous_last = list.last;

    assert_equal(list.size, 3);

    resize(&list, 5);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 5);
    assert_list_integrity(&list);

    assert_equal(previous_last, list.last->previous->previous);

    free(&list);
}

define_test(resize_deallocates_nodes_when_resizing_to_smaller_size)
{
    linked_list<int> list;

    init(&list, 5);

    assert_equal(list.size, 5);

    list_node<int> *third_node = nth_node(&list, 2);

    resize(&list, 3);

    assert_not_equal(list.first, nullptr);
    assert_not_equal(list.last, nullptr);
    assert_not_equal(list.first->next, nullptr);
    assert_not_equal(list.last->previous, nullptr);
    assert_equal(list.first->previous, nullptr);
    assert_equal(list.last->next, nullptr);
    assert_equal(list.size, 3);
    assert_list_integrity(&list);

    assert_equal(list.last, third_node);

    free(&list);
}

define_test(for_list_iterates_values)
{
    linked_list<int> list1;
    linked_list<int> list2;

    init(&list1, 3);
    init(&list2);

    list1[0] = 1;
    list1[1] = 2;
    list1[2] = 3;

    for_list(v, &list1)
    {
        list_node<int> *n = add_elements(&list2, 1);
        n->value = *v;
    }

    assert_equal(list1[0], list2[0]);
    assert_equal(list1[1], list2[1]);
    assert_equal(list1[2], list2[2]);

    free(&list1);
    free(&list2);
}

define_test(for_list_iterates_indices_and_values)
{
    linked_list<int> list1;
    linked_list<int> list2;

    init(&list1, 3);
    init(&list2, 3);

    list1[0] = 1;
    list1[1] = 2;
    list1[2] = 3;

    for_list(i, v, &list1)
        list2[i] = *v;

    assert_equal(list1[0], list2[0]);
    assert_equal(list1[1], list2[1]);
    assert_equal(list1[2], list2[2]);

    free(&list1);
    free(&list2);
}

define_test(for_list_iterates_indices_and_values_and_nodes)
{
    linked_list<int> list1;
    linked_list<int> list2;

    init(&list1, 3);
    init(&list2, 3);

    list1[0] = 1;
    list1[1] = 2;
    list1[2] = 3;

    for_list(i, v, node, &list1)
        list2[i] = *v;

    assert_equal(list1[0], list2[0]);
    assert_equal(list1[1], list2[1]);
    assert_equal(list1[2], list2[2]);

    free(&list1);
    free(&list2);
}

define_test(hash_hashes_list)
{
    linked_list<int> list1;
    linked_list<int> list2;
    linked_list<int> list3;
    linked_list<int> list4;

    init(&list1, 3);
    init(&list2, 3);
    init(&list3, 3);
    init(&list4, 4);

    list1[0] = 1;
    list1[1] = 2;
    list1[2] = 3;

    list2[0] = 1;
    list2[1] = 2;
    list2[2] = 3;

    list3[0] = 3;
    list3[1] = 2;
    list3[2] = 1;

    list4[0] = 1;
    list4[1] = 2;
    list4[2] = 3;
    list4[3] = 4;

    hash_t hsh1 = hash(&list1);
    hash_t hsh2 = hash(&list2);
    hash_t hsh3 = hash(&list3);
    hash_t hsh4 = hash(&list4);

    assert_equal(hsh1, hsh2);
    assert_not_equal(hsh1, hsh3);
    assert_not_equal(hsh1, hsh4);
    assert_not_equal(hsh3, hsh4);

    free(&list1);
    free(&list2);
    free(&list3);
    free(&list4);
}

define_default_test_main();
