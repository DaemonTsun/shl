
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

define_default_test_main();
