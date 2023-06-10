
#include <t1/t1.hpp>

#include "shl/set.hpp"

define_test(init_initializes_set)
{
    set<int> st;

    init(&st);

    assert_equal(st.data.data, nullptr);
    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(init_initializes_set2)
{
    set<int> st;

    init(&st, 10);

    assert_not_equal(st.data.data, nullptr);
    assert_equal(st.data.reserved_size, 10);

    free(&st);
}

define_test(zero_init_initializes_set)
{
    set<int> st{};

    assert_equal(st.data.data, nullptr);
    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(insert_element_inserts_to_empty_set)
{
    set<int> st{};

    assert_equal(st.data.data, nullptr);
    assert_equal(array_size(&st), 0);

    int *ret = insert_element(&st, 1);

    assert_not_equal(st.data.data, nullptr);
    assert_not_equal(ret, nullptr);
    assert_equal(st.data.data, ret);
    assert_equal(array_size(&st), 1);
    assert_equal(st.data[0], 1);

    free(&st);
}

define_test(insert_element_doesnt_insert_duplicates)
{
    set<int> st{};

    int *ret = insert_element(&st, 1);

    assert_not_equal(st.data.data, nullptr);
    assert_not_equal(ret, nullptr);
    assert_equal(st.data.data, ret);
    assert_equal(array_size(&st), 1);
    assert_equal(st.data[0], 1);

    ret = insert_element(&st, 1);
    assert_equal(st.data.data, ret);
    assert_equal(array_size(&st), 1);
    assert_equal(st.data[0], 1);

    free(&st);
}

define_test(insert_element_doesnt_insert_duplicates2)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 1);
    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);

    assert_equal(array_size(&st), 3);
    assert_equal(st.data[0], 1);
    assert_equal(st.data[1], 2);
    assert_equal(st.data[2], 3);

    free(&st);
}

define_test(remove_element_does_nothing_on_empty_set)
{
    set<int> st{};

    assert_equal(array_data(&st), nullptr);
    assert_equal(array_size(&st), 0);

    remove_element(&st, 12);

    assert_equal(array_data(&st), nullptr);
    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(remove_element_does_nothing_when_element_not_in_set)
{
    set<int> st{};

    insert_element(&st, 5);

    assert_equal(array_size(&st), 1);

    remove_element(&st, 12);

    assert_equal(array_size(&st), 1);
    assert_equal(st[0], 5);

    free(&st);
}

define_test(remove_element_removes_element_from_set)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    assert_equal(array_size(&st), 4);
    assert_equal(st[0], 1);
    assert_equal(st[1], 2);
    assert_equal(st[2], 3);
    assert_equal(st[3], 4);

    remove_element(&st, 3);

    assert_equal(array_size(&st), 3);
    assert_equal(st[0], 1);
    assert_equal(st[1], 2);
    assert_equal(st[2], 4);

    remove_element(&st, 1);

    assert_equal(array_size(&st), 2);
    assert_equal(st[0], 2);
    assert_equal(st[1], 4);

    remove_element(&st, 4);

    assert_equal(array_size(&st), 1);
    assert_equal(st[0], 2);

    remove_element(&st, 2);

    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(remove_elements_by_index_removes_element_at_index)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    assert_equal(array_size(&st), 4);

    remove_elements_at_index(&st, 2, 2);

    assert_equal(array_size(&st), 2);
    assert_equal(st[0], 1);
    assert_equal(st[1], 2);

    remove_elements_at_index(&st, 0, 1);

    assert_equal(array_size(&st), 1);
    assert_equal(st[0], 2);

    remove_elements_at_index(&st, 0, 1);

    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(resize_resizes_set)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    assert_equal(array_size(&st), 4);

    resize(&st, 2);

    assert_equal(array_size(&st), 2);
    assert_equal(st[0], 1);
    assert_equal(st[1], 2);

    resize(&st, 0);

    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(clear_clears_set)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    assert_equal(array_size(&st), 4);

    clear(&st);

    assert_equal(array_size(&st), 0);

    free(&st);
}

define_test(search_returns_pointer_to_element_in_set_when_found)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    int *ret = search(&st, 1);
    assert_equal(ret, st.data.data);

    ret = search(&st, 4);
    assert_equal(ret, st.data.data + 3);

    free(&st);
}

define_test(search_returns_nullptr_when_not_found)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    int *ret = search(&st, 5);
    assert_equal(ret, nullptr);

    free(&st);
}

define_test(index_of_returns_pointer_to_element_in_set_when_found)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    s64 ret = index_of(&st, 1);
    assert_equal(ret, 0);

    ret = index_of(&st, 4);
    assert_equal(ret, 3);

    free(&st);
}

define_test(index_of_returns_minus_one_when_not_found)
{
    set<int> st{};

    insert_element(&st, 1);
    insert_element(&st, 2);
    insert_element(&st, 3);
    insert_element(&st, 4);

    s64 ret = index_of(&st, 5);
    assert_equal(ret, -1);

    free(&st);
}

define_default_test_main();
