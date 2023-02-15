
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

define_default_test_main();
