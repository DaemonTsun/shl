
#define t1_use_stdlib 1
#include <t1/t1.hpp>

#include "shl/fixed_array.hpp"

/*
define_test(zero_array)
{
    fixed_array<int, 0> arr;

    assert_equal(array_size(&arr), 0);
}
*/

define_test(at_gets_pointer_to_nth_element)
{
    fixed_array<int, 3> arr;

    arr.data[0] = 1;
    arr.data[1] = 2;
    arr.data[2] = 3;

    assert_equal(at(&arr, 0), arr.data);
    assert_equal(*at(&arr, 0), 1);
    assert_equal(at(&arr, 1), arr.data + 1);
    assert_equal(*at(&arr, 1), 2);
    assert_equal(at(&arr, 2), arr.data + 2);
    assert_equal(*at(&arr, 2), 3);
}

define_test(index_access_operator_gets_reference_to_element_at)
{
    fixed_array<int, 3> arr;

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    assert_equal(at(&arr, 0), arr.data);
    assert_equal(*at(&arr, 0), 1);
    assert_equal(at(&arr, 1), arr.data + 1);
    assert_equal(*at(&arr, 1), 2);
    assert_equal(at(&arr, 2), arr.data + 2);
    assert_equal(*at(&arr, 2), 3);
}

define_test(for_array_iterates_indices_and_values)
{
    fixed_array<int, 3> arr1;
    fixed_array<int, 3> arr2;

    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;

    for_array(i, v, &arr1)
        arr2[i] = *v;

    assert_equal(arr1[0], arr2[0]);
    assert_equal(arr1[1], arr2[1]);
    assert_equal(arr1[2], arr2[2]);
}

define_test(hash_hashes_array)
{
    fixed_array<int, 3> arr1 = {1, 2, 3};
    fixed_array<int, 3> arr2{1, 2, 3};
    fixed_array<int, 3> arr3{3, 2, 1};
    fixed_array arr4{1, 2, 3, 4};

    hash_t hsh1 = hash(&arr1);
    hash_t hsh2 = hash(&arr2);
    hash_t hsh3 = hash(&arr3);
    hash_t hsh4 = hash(&arr4);

    assert_equal(hsh1, hsh2);
    assert_not_equal(hsh1, hsh3);
    assert_not_equal(hsh1, hsh4);
    assert_not_equal(hsh3, hsh4);
}

define_test(search_returns_nullptr_if_key_is_not_found)
{
    fixed_array<int, 3> arr;

    for_array(i, v, &arr)
        *v = (int)i;
    
    int val = 6;

    assert_equal(search(&arr, &val), nullptr);
}

define_test(search_returns_pointer_to_element_if_key_is_found)
{
    fixed_array<int, 3> arr;

    for_array(i, v, &arr)
        *v = (int)i;
    
    int *ptr = search(&arr, 1);

    assert_not_equal(ptr, nullptr);

    *ptr = 8;
    assert_equal(arr[1], *ptr);
}

define_test(index_of_returns_negative_one_if_key_is_not_found)
{
    fixed_array<int, 3> arr;

    for_array(i, v, &arr)
        *v = (int)i;
    
    assert_equal(index_of(&arr, 6), -1);
}

define_test(index_of_returns_index_of_key_if_key_is_found)
{
    fixed_array<int, 3> arr;

    for_array(i, v, &arr)
        *v = (int)i;
    
    assert_equal(index_of(&arr, 1), 1);
}

define_test(contains_returns_false_if_key_is_not_found)
{
    fixed_array<int, 3> arr;

    for_array(i, v, &arr)
        *v = (int)i;
    
    int val = 6;
    assert_equal(contains(&arr, &val), false);
}

define_test(contains_returns_true_if_key_is_found)
{
    fixed_array<int, 3> arr;

    for_array(i, v, &arr)
        *v = (int)i;
    
    assert_equal(contains(&arr, 1), true);
}

define_test(constexpr_fixed_array_tests)
{
    constexpr fixed_array arr{1, 2, 3};

    static_assert(array_size(&arr) == 3);
    static_assert(arr[1] == 2);
    static_assert(*at(&arr, 0) == 1);
    static_assert(index_of(&arr, 2) == 1);
    static_assert(search(&arr, 2) == arr.data+1);
    static_assert(contains(&arr, 3));
}

define_default_test_main();

