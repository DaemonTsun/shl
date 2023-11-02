
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

define_test(reserve_initializes_on_empty_array)
{
    array<int> arr;

    init(&arr);
    reserve(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(reserve_does_nothing_on_same_or_smaller_size)
{
    array<int> arr;

    init(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    reserve(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    reserve(&arr, 5);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 10);
    assert_equal(arr.reserved_size, 10);

    free(&arr);
}

define_test(reserve_allocates_when_reserving_more_size)
{
    array<int> arr;

    init(&arr, 5);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 5);
    assert_equal(arr.reserved_size, 5);

    reserve(&arr, 10);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 5);
    assert_equal(arr.reserved_size, 10);

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

define_test(insert_elements_does_nothing_when_inserting_no_elements)
{
    array<int> arr;

    init(&arr, 10);

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

define_test(insert_elements_adds_elements_and_moves_next_elements_back2)
{
    array<int> arr;

    init(&arr, 4);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;

    insert_elements(&arr, 0, 1);

    assert_equal(arr.size, 5);

    assert_equal(arr[1], 1);
    assert_equal(arr[2], 2);
    assert_equal(arr[3], 3);
    assert_equal(arr[4], 4);

    free(&arr);
}

define_test(add_range_does_nothing_when_adding_nullptr)
{
    array<int> arr{};

    int *ret = add_range(&arr, (const int*)nullptr, 3);

    assert_equal(arr.size, 0);
    assert_equal(ret, nullptr);

    free(&arr);
}

define_test(add_range_does_nothing_when_adding_zero_elements)
{
    array<int> arr{};

    int elems[3] = {1, 2, 3};

    int *ret = add_range(&arr, elems, 0);

    assert_equal(arr.size, 0);
    assert_equal(ret, nullptr);

    free(&arr);
}

define_test(add_range_initializes_empty_array)
{
    array<int> arr{};

    int elems[3] = {1, 2, 3};

    int *ret = add_range(&arr, elems, 3);

    assert_equal(arr.size, 3);
    assert_equal(ret, arr.data);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[2], 3);

    free(&arr);
}

define_test(add_range_adds_elements_at_end)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    int elems[3] = {1, 2, 3};

    int *ret = add_range(&arr, elems, 3);

    assert_equal(arr.size, 6);
    assert_equal(ret, arr.data + 3);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[2], 3);
    assert_equal(arr[3], 1);
    assert_equal(arr[4], 2);
    assert_equal(arr[5], 3);

    free(&arr);
}

define_test(add_range_adds_array_at_end)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    array<int> arr2{};
    init(&arr2, 2);

    arr2[0] = 5;
    arr2[1] = 6;

    add_range(&arr, &arr2);

    assert_equal(arr.size, 5);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[2], 3);
    assert_equal(arr[3], 5);
    assert_equal(arr[4], 6);

    free(&arr2);
    free(&arr);
}

define_test(add_range_adds_slice_of_other_array)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    array<int> arr2{};
    init(&arr2, 5);

    arr2[0] = 5;
    arr2[1] = 6;
    arr2[2] = 7;
    arr2[3] = 8;
    arr2[4] = 9;

    // starting from index 1, 3 elements
    add_range(&arr, &arr2, 1, 3);

    assert_equal(arr.size, 6);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[2], 3);
    assert_equal(arr[3], 6);
    assert_equal(arr[4], 7);
    assert_equal(arr[5], 8);

    free(&arr2);
    free(&arr);
}

define_test(insert_range_initializes_empty_array)
{
    array<int> arr{};

    int elems[3] = {1, 2, 3};

    int *ret = insert_range(&arr, 0, elems, 3);

    assert_equal(arr.size, 3);
    assert_equal(ret, arr.data);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[2], 3);

    free(&arr);
}

define_test(insert_range_does_nothing_when_index_is_outside_array_range)
{
    array<int> arr{};

    int elems[3] = {1, 2, 3};

    int *ret = insert_range(&arr, 5, elems, 3);

    assert_equal(arr.size, 0);
    assert_equal(ret, nullptr);

    free(&arr);
}

define_test(insert_range_inserts_range_at_end)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    int elems[3] = {5, 6};

    int *ret = insert_range(&arr, 3, elems, 2);

    assert_equal(arr.size, 5);
    assert_equal(ret, arr.data + 3);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 2);
    assert_equal(arr[2], 3);
    assert_equal(arr[3], 5);
    assert_equal(arr[4], 6);

    free(&arr);
}

define_test(insert_range_inserts_range_at_start)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    int elems[3] = {5, 6};

    int *ret = insert_range(&arr, 0, elems, 2);

    assert_equal(arr.size, 5);
    assert_equal(ret, arr.data);

    assert_equal(arr[0], 5);
    assert_equal(arr[1], 6);
    assert_equal(arr[2], 1);
    assert_equal(arr[3], 2);
    assert_equal(arr[4], 3);

    free(&arr);
}

define_test(insert_range_inserts_range_into_array)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    int elems[3] = {5, 6};

    int *ret = insert_range(&arr, 1, elems, 2);

    assert_equal(arr.size, 5);
    assert_equal(ret, arr.data + 1);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 5);
    assert_equal(arr[2], 6);
    assert_equal(arr[3], 2);
    assert_equal(arr[4], 3);

    free(&arr);
}

define_test(insert_range_inserts_array_into_array)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    array<int> arr2{};
    init(&arr2, 2);
    
    arr2[0] = 5;
    arr2[1] = 6;

    insert_range(&arr, 1, &arr2);

    assert_equal(arr.size, 5);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 5);
    assert_equal(arr[2], 6);
    assert_equal(arr[3], 2);
    assert_equal(arr[4], 3);

    free(&arr2);
    free(&arr);
}

define_test(insert_range_inserts_array_slice_into_array)
{
    array<int> arr{};
    init(&arr, 3);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    array<int> arr2{};
    init(&arr2, 5);

    arr2[0] = 5;
    arr2[1] = 6;
    arr2[2] = 7;
    arr2[3] = 8;
    arr2[4] = 9;

    // insert at index 1, starting at index 2 in other array, 3 elements
    insert_range(&arr, 1, &arr2, 2, 3);

    assert_equal(arr.size, 6);

    assert_equal(arr[0], 1);
    assert_equal(arr[1], 7);
    assert_equal(arr[2], 8);
    assert_equal(arr[3], 9);
    assert_equal(arr[4], 2);
    assert_equal(arr[5], 3);

    free(&arr2);
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

define_test(remove_elements_removes_elements3)
{
    array<int> arr;

    init(&arr, 4);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;

    remove_elements(&arr, 0, 3);

    assert_not_equal(arr.data, nullptr);
    assert_equal(arr.size, 1);
    assert_greater_or_equal(arr.reserved_size, 1);

    assert_equal(arr[0], 4);

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

define_test(for_array_iterates_values)
{
    array<int> arr1;
    array<int> arr2;

    init(&arr1, 3);
    init(&arr2);

    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;

    for_array(v, &arr1)
    {
        int *n = add_elements(&arr2, 1);
        *n = *v;
    }

    assert_equal(arr1[0], arr2[0]);
    assert_equal(arr1[1], arr2[1]);
    assert_equal(arr1[2], arr2[2]);

    free(&arr1);
    free(&arr2);
}

define_test(for_array_iterates_indices_and_values)
{
    array<int> arr1;
    array<int> arr2;

    init(&arr1, 3);
    init(&arr2, 3);

    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;

    for_array(i, v, &arr1)
        arr2[i] = *v;

    assert_equal(arr1[0], arr2[0]);
    assert_equal(arr1[1], arr2[1]);
    assert_equal(arr1[2], arr2[2]);

    free(&arr1);
    free(&arr2);
}

define_test(hash_hashes_array)
{
    array<int> arr1;
    array<int> arr2;
    array<int> arr3;
    array<int> arr4;

    init(&arr1, 3);
    init(&arr2, 3);
    init(&arr3, 3);
    init(&arr4, 4);

    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;

    arr2[0] = 1;
    arr2[1] = 2;
    arr2[2] = 3;

    arr3[0] = 3;
    arr3[1] = 2;
    arr3[2] = 1;

    arr4[0] = 1;
    arr4[1] = 2;
    arr4[2] = 3;
    arr4[3] = 4;

    hash_t hsh1 = hash(&arr1);
    hash_t hsh2 = hash(&arr2);
    hash_t hsh3 = hash(&arr3);
    hash_t hsh4 = hash(&arr4);

    assert_equal(hsh1, hsh2);
    assert_not_equal(hsh1, hsh3);
    assert_not_equal(hsh1, hsh4);
    assert_not_equal(hsh3, hsh4);

    free(&arr1);
    free(&arr2);
    free(&arr3);
    free(&arr4);
}

define_test(search_returns_nullptr_if_key_is_not_found)
{
    array<int> arr;

    init(&arr, 5);

    for_array(i, v, &arr)
        *v = (int)i;
    
    int val = 6;

    assert_equal(search(&arr, &val), nullptr);

    free(&arr);
}

define_test(search_returns_pointer_to_element_if_key_is_found)
{
    array<int> arr;

    init(&arr, 5);

    for_array(i, v, &arr)
        *v = (int)i;
    
    int *ptr = search(&arr, 1);

    assert_not_equal(ptr, nullptr);

    *ptr = 8;
    assert_equal(arr[1], *ptr);

    free(&arr);
}

define_test(index_of_returns_negative_one_if_key_is_not_found)
{
    array<int> arr;

    init(&arr, 5);

    for_array(i, v, &arr)
        *v = (int)i;
    
    assert_equal(index_of(&arr, 6), -1ull);

    free(&arr);
}

define_test(index_of_returns_index_of_key_if_key_is_found)
{
    array<int> arr;

    init(&arr, 5);

    for_array(i, v, &arr)
        *v = (int)i;
    
    assert_equal(index_of(&arr, 1), 1);

    free(&arr);
}

define_test(contains_returns_false_if_key_is_not_found)
{
    array<int> arr;

    init(&arr, 5);

    for_array(i, v, &arr)
        *v = (int)i;
    
    int val = 6;
    assert_equal(contains(&arr, &val), false);

    free(&arr);
}

define_test(contains_returns_true_if_key_is_found)
{
    array<int> arr;

    init(&arr, 5);

    for_array(i, v, &arr)
        *v = (int)i;
    
    assert_equal(contains(&arr, 1), true);

    free(&arr);
}

define_test(free_values_frees_all_values)
{
    array<array<int>> arr_of_arrs;
    init(&arr_of_arrs, 16);

    for_array(v, &arr_of_arrs)
        init(v, 16);

    free_values(&arr_of_arrs);
    free(&arr_of_arrs);
}

define_test(free_can_free_all_values)
{
    array<array<int>> arr_of_arrs;
    init(&arr_of_arrs, 16);

    for_array(v, &arr_of_arrs)
        init(v, 16);

    // true = free all values AND array memory
    free<true>(&arr_of_arrs);
}

define_test(resize_can_free_values)
{
    array<array<int>> arr_of_arrs;
    init(&arr_of_arrs, 16);

    for_array(v, &arr_of_arrs)
        init(v, 16);

    resize<true>(&arr_of_arrs, 8);

    // if resize<true> didn't free the arrays inside the array,
    // we would have lost mememory after this free.
    free<true>(&arr_of_arrs);
}

define_test(remove_elements_can_free_values)
{
    array<array<int>> arr_of_arrs;
    init(&arr_of_arrs, 16);

    for_array(v, &arr_of_arrs)
        init(v, 16);

    remove_elements<true>(&arr_of_arrs, 8, 4);

    free<true>(&arr_of_arrs);
}

define_test(add_at_end_adds_element_at_end)
{
    array<int> arr;
    init(&arr);

    assert_equal(arr.size, 0);

    auto ret = add_at_end(&arr, 5);

    assert_equal(arr.size, 1);
    assert_equal(ret, arr.data);
    assert_equal(arr[0], 5);

    int x = 10;
    ret = add_at_end(&arr, &x);

    assert_equal(arr.size, 2);
    assert_equal(ret, arr.data + 1);
    assert_equal(arr[0], 5);
    assert_equal(arr[1], 10);

    free(&arr);
}

define_test(add_at_start_adds_element_at_start)
{
    array<int> arr;
    init(&arr);

    assert_equal(arr.size, 0);

    auto ret = add_at_start(&arr, 5);

    assert_equal(arr.size, 1);
    assert_equal(ret, arr.data);
    assert_equal(arr[0], 5);

    int x = 10;
    ret = add_at_start(&arr, &x);

    assert_equal(arr.size, 2);
    assert_equal(ret, arr.data);
    assert_equal(arr[0], 10);
    assert_equal(arr[1], 5);

    free(&arr);
}

define_test(zero_init_zero_initializes_array)
{
    array<int> arr{};

    assert_equal(arr.size, 0);

    auto ret = add_at_start(&arr, 5);

    assert_equal(arr.size, 1);
    assert_equal(ret, arr.data);
    assert_equal(arr[0], 5);

    int x = 10;
    ret = add_at_start(&arr, &x);

    assert_equal(arr.size, 2);
    assert_equal(ret, arr.data);
    assert_equal(arr[0], 10);
    assert_equal(arr[1], 5);

    free(&arr);
}

define_default_test_main();
