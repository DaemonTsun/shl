
#include <t1/t1.hpp>

#include "shl/string.hpp"
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

define_test(index_operator_gets_nth_row)
{
    array2d<int> arr;

    init(&arr, 2, 3);

    int *row0 = arr[0];

    assert_not_equal(row0, nullptr);
    assert_equal(row0, arr.entries.data);

    int *row1 = arr[1];

    assert_not_equal(row1, nullptr);
    assert_equal(row1, arr.entries.data + arr.width);

    int *row2 = arr[2];

    assert_not_equal(row2, nullptr);
    assert_equal(row2, arr.entries.data + arr.width * 2);

    free(&arr);
}

define_test(for_array2d_iterates_array2d)
{
    array2d<int> arr;

    init(&arr, 2, 3);

    arr[0][0] = 0;
    arr[0][1] = 1;
    arr[1][0] = 2;
    arr[1][1] = 3;
    arr[2][0] = 4;
    arr[2][1] = 5;

    int i = 0;

    for_array2d(x, y, cell, &arr)
    {
        assert_equal(*cell, x + y * arr.width);
        i += 1;
    }

    assert_equal(i, 6);

    free(&arr);
}

define_test(resize_enlarges_height_of_array2d)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    bool ok = resize(&arr, 2, 3);

    assert_equal(ok, true);

    assert_equal(arr.width,  2);
    assert_equal(arr.height, 3);
    assert_equal(arr.entries.size, arr.width * arr.height);

    assert_equal(arr[0][0], "a"_cs);
    assert_equal(arr[0][1], "b"_cs);
    assert_equal(arr[1][0], "c"_cs);
    assert_equal(arr[1][1], "d"_cs);

    // new elements are always 0 initialized
    assert_equal(arr[2][0].data, nullptr);
    assert_equal(arr[2][0].size, 0);
    assert_equal(arr[2][1].data, nullptr);
    assert_equal(arr[2][1].size, 0);

    free<true>(&arr);
}

define_test(resize_enlarges_width_of_array2d)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    bool ok = resize(&arr, 3, 2);

    assert_equal(ok, true);

    assert_equal(arr.width,  3);
    assert_equal(arr.height, 2);
    assert_equal(arr.entries.size, arr.width * arr.height);

    // note: elements are still at the same indices (but not memory positions)
    // as they were before.
    assert_equal(arr[0][0], "a"_cs);
    assert_equal(arr[0][1], "b"_cs);
    assert_equal(arr[1][0], "c"_cs);
    assert_equal(arr[1][1], "d"_cs);

    // new elements are always 0 initialized
    assert_equal(arr[0][2].data, nullptr);
    assert_equal(arr[0][2].size, 0);
    assert_equal(arr[1][2].data, nullptr);
    assert_equal(arr[1][2].size, 0);

    free<true>(&arr);
}

define_test(resize_shrinks_height_of_array2d)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    // since we're shrinking and string has memory allocation, set
    // FreeValues template param to true to free them correctly
    bool ok = resize<true>(&arr, 2, 1);

    assert_equal(ok, true);

    assert_equal(arr.width,  2);
    assert_equal(arr.height, 1);
    assert_equal(arr.entries.size, arr.width * arr.height);

    assert_equal(arr[0][0], "a"_cs);
    assert_equal(arr[0][1], "b"_cs);

    free<true>(&arr);
}

define_test(resize_shrinks_width_of_array2d)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    bool ok = resize<true>(&arr, 1, 2);

    assert_equal(ok, true);

    assert_equal(arr.width,  1);
    assert_equal(arr.height, 2);
    assert_equal(arr.entries.size, arr.width * arr.height);

    assert_equal(arr[0][0], "a"_cs);
    assert_equal(arr[1][0], "c"_cs);

    free<true>(&arr);
}

define_test(resize_shrinks_to_zero)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    bool ok = resize<true>(&arr, 0, 0);

    assert_equal(ok, true);

    assert_equal(arr.width,  0);
    assert_equal(arr.height, 0);
    assert_equal(arr.entries.size, arr.width * arr.height);

    free<true>(&arr);
}

define_test(resize_test)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    bool ok = resize<true>(&arr, 1, 4);

    assert_equal(ok, true);

    assert_equal(arr.width,  1);
    assert_equal(arr.height, 4);
    assert_equal(arr.entries.size, arr.width * arr.height);
    
    assert_equal(arr[0][0], "a"_cs);
    assert_equal(arr[1][0], "c"_cs);
    assert_equal(arr[2][0].data, nullptr);
    assert_equal(arr[3][0].data, nullptr);

    free<true>(&arr);
}

define_test(resize_test2)
{
    array2d<string> arr;

    init(&arr, 2, 2);

    arr[0][0] = "a"_s;
    arr[0][1] = "b"_s;
    arr[1][0] = "c"_s;
    arr[1][1] = "d"_s;

    bool ok = resize<true>(&arr, 8, 8);

    assert_equal(ok, true);

    assert_equal(arr.width,  8);
    assert_equal(arr.height, 8);
    assert_equal(arr.entries.size, arr.width * arr.height);
    
    assert_equal(arr[0][0], "a"_cs);
    assert_equal(arr[0][1], "b"_cs);
    assert_equal(arr[1][0], "c"_cs);
    assert_equal(arr[1][1], "d"_cs);

    free<true>(&arr);
}

define_default_test_main();
