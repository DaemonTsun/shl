
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
