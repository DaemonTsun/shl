
#include <t1/t1.hpp>

#include "shl/array.hpp"
#include "shl/sort.hpp"

int custom_comparer(const int *a, const int *b)
{
    int a_val = *a;
    int b_val = *b;

    if (a_val < b_val) return 1;
    if (a_val > b_val) return -1;
    return 0;
}

define_test(sort_sorts_ascending)
{
    array<int> arr;
    init(&arr, 4);

    arr.data[0] = 5;
    arr.data[1] = 6;
    arr.data[2] = 2;
    arr.data[3] = 0;

    sort(arr.data, arr.size);

    assert_equal(arr.data[0], 0);
    assert_equal(arr.data[1], 2);
    assert_equal(arr.data[2], 5);
    assert_equal(arr.data[3], 6);

    free(&arr);
}

define_test(sort_sorts_according_to_comparer)
{
    array<int> arr;
    init(&arr, 4);

    arr.data[0] = 5;
    arr.data[1] = 6;
    arr.data[2] = 2;
    arr.data[3] = 0;

    sort(arr.data, arr.size, custom_comparer);

    assert_equal(arr.data[0], 6);
    assert_equal(arr.data[1], 5);
    assert_equal(arr.data[2], 2);
    assert_equal(arr.data[3], 0);

    free(&arr);
}

define_test(sorted_search_returns_pointer_to_searched_element)
{
    array<int> arr;
    init(&arr, 4);

    arr.data[0] = 5;
    arr.data[1] = 6;
    arr.data[2] = 2;
    arr.data[3] = 0;

    int searchfor = 6;
    int *ptr = sorted_search(&searchfor, arr.data, arr.size);

    assert_not_equal(ptr, nullptr);
    assert_equal(ptr, arr.data + 1);
    assert_equal(ptr - arr.data, 1);

    free(&arr);
}

define_test(sorted_search_returns_nullptr_when_element_not_found)
{
    array<int> arr;
    init(&arr, 4);

    arr.data[0] = 5;
    arr.data[1] = 6;
    arr.data[2] = 2;
    arr.data[3] = 0;

    int searchfor = 7;
    int *ptr = sorted_search(&searchfor, arr.data, arr.size);

    assert_equal(ptr, nullptr);

    free(&arr);
}

define_test(sorted_index_of_returns_the_sorted_index_of_an_element_when_found)
{
    array<int> arr;
    init(&arr, 4);

    arr.data[0] = 5;
    arr.data[1] = 6;
    arr.data[2] = 2;
    arr.data[3] = 0;

    int searchfor = 6;
    u64 index = sorted_index_of(&searchfor, arr.data, arr.size);

    assert_equal(index, 1);

    free(&arr);
}

define_test(sorted_index_of_returns_largest_number_when_not_found)
{
    array<int> arr;
    init(&arr, 4);

    arr.data[0] = 5;
    arr.data[1] = 6;
    arr.data[2] = 2;
    arr.data[3] = 0;

    int searchfor = 7;
    u64 index = sorted_index_of(&searchfor, arr.data, arr.size);

    assert_equal(index, -1);

    free(&arr);
}

define_default_test_main();
