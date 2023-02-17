
#include <t1/t1.hpp>

#include "shl/array.hpp"

define_test(init_initializes_array)
{
    array<int> arr;

    init(&arr);

    assert_equal(arr.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(arr.reserved_size, 0);
}

define_test(init_initializes_array2)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(resize_initializes_on_empty_array)
{
    array<int> arr;

    init(&arr);
    resize(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(resize_does_nothing_on_same_size)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    resize(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(resize_resizes_array)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    resize(&arr, 5);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 5);
    assert_equal(arr.reserved_size, 5);

    free(&arr);
}

define_test(resize_resizes_array2)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    resize(&arr, 15);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 15);
    assert_equal(arr.reserved_size, 15);

    free(&arr);
}

define_test(add_elements_adds_at_least_n_elements)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    u64 previous_size = arr.size;
    int *ptr = add_elements(&arr, 5);
    int *previous_last = arr.data + previous_size;

    assert_not_equal(arr.data, nullptr);
    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, previous_last);
    assert_equal(arr.size, 15);
    assert_greater_or_equal(arr.reserved_size, 15);

    free(&arr);
}

define_test(add_elements_returns_nullptr_when_adding_zero_elements)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    int *ptr = add_elements(&arr, 0);

    assert_not_equal(arr.data, nullptr);
    assert_equal(ptr, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(add_elements_adds_elements_to_empty_array)
{
    array<int> arr;

    init(&arr);

    assert_equal(arr.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(arr.reserved_size, 0);

    int *ptr = add_elements(&arr, 5);

    assert_not_equal(arr.data, nullptr);
    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, arr.data);
    assert_equal(arr.size, 5);
    assert_greater_or_equal(arr.reserved_size, 5);

    free(&arr);
}

define_test(insert_elements_adds_elements_to_empty_array)
{
    array<int> arr;

    init(&arr);

    assert_equal(arr.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(arr.reserved_size, 0);

    int *ptr = insert_elements(&arr, arr.size, 5);

    assert_not_equal(arr.data, nullptr);
    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, arr.data);
    assert_equal(arr.size, 5);
    assert_greater_or_equal(arr.reserved_size, 5);

    free(&arr);
}

define_test(insert_elements_adds_elements_at_back_at_insert_on_back)
{
    array<int> arr;

    init(&arr, 10);

    u64 previous_size = arr.size;
    int *ptr = insert_elements(&arr, arr.size, 5);
    int *previous_last = arr.data + previous_size;

    assert_not_equal(arr.data, nullptr);
    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, previous_last);
    assert_equal(arr.size, 15);
    assert_greater_or_equal(arr.reserved_size, 15);

    free(&arr);
}

define_test(insert_returns_nullptr_on_insert_beyond_array_size)
{
    array<int> arr;

    init(&arr, 10);

    int *ptr = insert_elements(&arr, arr.size + 4 /*arbitrary*/, 5);

    assert_not_equal(arr.data, nullptr);
    assert_equal(ptr, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(insert_elements_doesnt_add_elements_when_inserting_zero_elements)
{
    array<int> arr;

    init(&arr, 10);

    u64 previous_size = arr.size;
    int *ptr = insert_elements(&arr, 5, 0);

    assert_not_equal(arr.data, nullptr);
    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, arr.data + 5);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(insert_elements_adds_elements_and_moves_next_elements_back)
{
    array<int> arr;

    init(&arr, 4);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;

    int *ptr = insert_elements(&arr, 2, 5);

    assert_not_equal(arr.data, nullptr);
    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, arr.data + 2);
    assert_equal(arr.size, 9);
    assert_greater_or_equal(arr.reserved_size, 9);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[7], 3);
    assert_equal(arr[8], 4);

    free(&arr);
}

define_test(remove_elements_does_nothing_on_empty_array)
{
    array<int> arr;

    init(&arr);

    assert_equal(arr.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(arr.reserved_size, 0);

    remove_elements(&arr, 0, 1);

    assert_equal(arr.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(arr.reserved_size, 0);

    free(&arr);
}

define_test(remove_elements_does_nothing_when_removing_no_elements)
{
    array<int> arr;

    init(&arr, 10);

    remove_elements(&arr, 5, 0);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(remove_elements_does_nothing_when_removing_elements_outside_of_array_size)
{
    array<int> arr;

    init(&arr, 5);

    remove_elements(&arr, 8, 1);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 5);
    assert_equal(arr.reserved_size, 5);

    free(&arr);
}

define_test(remove_elements_removes_elements)
{
    array<int> arr;

    init(&arr, 6);

    remove_elements(&arr, 3, 3);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 3);
    assert_greater_or_equal(arr.reserved_size, 3);

    free(&arr);
}

define_test(remove_elements_removes_elements2)
{
    array<int> arr;

    init(&arr, 4);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;

    remove_elements(&arr, 1, 2);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 2);
    assert_greater_or_equal(arr.reserved_size, 2);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 4);

    free(&arr);
}

define_test(remove_elements_doesnt_remove_elements_beyond_array_size)
{
    array<int> arr;

    init(&arr, 6);

    remove_elements(&arr, 3, 500);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 3);
    assert_greater_or_equal(arr.reserved_size, 3);

    free(&arr);
}

define_test(shrink_to_fit_shrinks_the_array_to_fit_only_used_elements)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    add_elements(&arr, 5);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 15);
    assert_greater_or_equal(arr.reserved_size, 15);

    shrink_to_fit(&arr);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 15);
    assert_equal(arr.reserved_size, 15);

    free(&arr);
}

define_test(at_gets_pointer_to_nth_element)
{
    array<int> arr;
    init(&arr, 3);

    arr.data[0] = 1;
    arr.data[1] = 2;
    arr.data[2] = 3;

    assert_equal(at(&arr, 0), arr.data);
    assert_equal(*at(&arr, 0), 1);
    assert_equal(at(&arr, 1), arr.data + 1);
    assert_equal(*at(&arr, 1), 2);
    assert_equal(at(&arr, 2), arr.data + 2);
    assert_equal(*at(&arr, 2), 3);

    free(&arr);
}

define_test(index_access_operator_gets_reference_to_element_at)
{
    array<int> arr;
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    assert_equal(at(&arr, 0), arr.data);
    assert_equal(*at(&arr, 0), 1);
    assert_equal(at(&arr, 1), arr.data + 1);
    assert_equal(*at(&arr, 1), 2);
    assert_equal(at(&arr, 2), arr.data + 2);
    assert_equal(*at(&arr, 2), 3);

    free(&arr);
}

define_default_test_main();
