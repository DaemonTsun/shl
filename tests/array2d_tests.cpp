
#include <t1/t1.hpp>

#include "shl/array2d.hpp"

define_test(init_initializes_array2d)
{
    array2d<int> arr;

    init(&arr);

    assert_equal(arr.entries.data, nullptr);
    assert_equal(arr.width, 0);
    assert_equal(arr.height, 0);
}

define_test(init_initializes_array2d2)
{
    array2d<int> arr;

    init(&arr, 2, 3);

    assert_not_equal(arr.entries.data, nullptr);
    assert_equal(arr.entries.size, 6);
    assert_equal(arr.width, 2);
    assert_equal(arr.height, 3);

    free(&arr);
}


define_default_test_main();
