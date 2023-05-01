
#include <t1/t1.hpp>

#include "shl/string.hpp"
#include "shl/defer.hpp"
#include "shl/chunk_array.hpp"

define_test(init_initializes_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr);
    defer { free(&arr); };

    assert_equal(arr.chunks.data, nullptr);
    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);
}

define_test(init_initializes_chunk_array2)
{
    chunk_array<int, 8> arr;
    init(&arr, 1);
    defer { free(&arr); };

    assert_not_equal(arr.chunks.data, nullptr);
    assert_equal(array_size(&arr), 8);
    assert_equal(chunk_count(&arr), 1);
}

define_test(add_chunks_adds_chunks_to_empty_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr);
    defer { free(&arr); };

    assert_equal(arr.chunks.data, nullptr);
    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);

    array_chunk<int, 8> *ret = add_chunks(&arr, 2);

    assert_not_equal(arr.chunks.data, nullptr);
    assert_equal(arr.chunks.data, ret);
    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);
}

define_test(add_chunks_adds_chunks_to_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr, 2);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);

    array_chunk<int, 8> *ret = add_chunks(&arr, 2);

    assert_equal(arr.chunks.data + 2, ret);
    assert_equal(array_size(&arr), 32);
    assert_equal(chunk_count(&arr), 4);
}

define_test(insert_chunks_inserts_into_empty_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr);
    defer { free(&arr); };

    assert_equal(arr.chunks.data, nullptr);
    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);

    array_chunk<int, 8> *ret = insert_chunks(&arr, 0, 1);

    assert_equal(arr.chunks.data, ret);
    assert_equal(array_size(&arr), 8);
    assert_equal(chunk_count(&arr), 1);
}

define_test(insert_chunks_inserts_into_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr, 2);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);

    array_chunk<int, 8> last = arr.chunks[1];
    array_chunk<int, 8> *ret = insert_chunks(&arr, 1, 1);

    assert_equal(arr.chunks.data + 1, ret);
    assert_equal(*(arr.chunks.data + 2), last);
    assert_equal(array_size(&arr), 24);
    assert_equal(chunk_count(&arr), 3);
}

define_test(insert_chunks_does_nothing_if_index_is_outside_array)
{
    chunk_array<int, 8> arr;
    init(&arr, 2);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);

    array_chunk<int, 8> *ret = insert_chunks(&arr, 5, 1);

    assert_equal(ret, nullptr);
    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);
}

define_test(remove_chunks_does_nothing_on_empty_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);

    remove_chunks(&arr, 0, 1);

    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);
}

define_test(remove_chunks_removes_chunks_from_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr, 3);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 24);
    assert_equal(chunk_count(&arr), 3);

    remove_chunks(&arr, 0, 1);

    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);
}

define_test(remove_chunks_removes_up_to_end_of_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr, 3);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 24);
    assert_equal(chunk_count(&arr), 3);

    remove_chunks(&arr, 1, 500);

    assert_equal(array_size(&arr), 8);
    assert_equal(chunk_count(&arr), 1);
}

define_test(resize_initializes_on_empty_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);

    resize(&arr, 3);

    assert_equal(array_size(&arr), 24);
    assert_equal(chunk_count(&arr), 3);
}

define_test(resize_makes_chunk_array_larger)
{
    chunk_array<int, 8> arr;
    init(&arr, 2);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);

    resize(&arr, 5);

    assert_equal(array_size(&arr), 40);
    assert_equal(chunk_count(&arr), 5);
}

define_test(resize_makes_chunk_array_smaller)
{
    chunk_array<int, 8> arr;
    init(&arr, 5);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 40);
    assert_equal(chunk_count(&arr), 5);

    resize(&arr, 2);

    assert_equal(array_size(&arr), 16);
    assert_equal(chunk_count(&arr), 2);
}

define_test(at_gets_nth_element_in_chunk_array)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 12);
    assert_equal(chunk_count(&arr), 3);

    for_array(i, v, arr.chunks[0])
        *v = i;

    for_array(i2, v2, arr.chunks[1])
        *v2 = i2 + 4;

    for_array(i3, v3, arr.chunks[2])
        *v3 = i3 + 8;

    assert_equal(at(&arr, 0), *(arr.chunks[0]) + 0);
    assert_equal(at(&arr, 3), *(arr.chunks[0]) + 3);
    assert_equal(at(&arr, 4), *(arr.chunks[1]) + 0);

    assert_equal(*at(&arr, 3), 3);
    assert_equal(*at(&arr, 5), 5);
    assert_equal(*at(&arr, 7), 7);
    assert_equal(*at(&arr, 8), 8);
    assert_equal(*at(&arr, 11), 11);
}

define_test(nth_chunk_gets_nth_chunk)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 12);
    assert_equal(chunk_count(&arr), 3);

    assert_equal(nth_chunk(&arr, 0), arr.chunks.data + 0);
    assert_equal(nth_chunk(&arr, 1), arr.chunks.data + 1);
    assert_equal(nth_chunk(&arr, 2), arr.chunks.data + 2);
}

define_test(clear_clears_chunk_array)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 12);
    assert_equal(chunk_count(&arr), 3);

    clear(&arr);

    assert_equal(array_size(&arr), 0);
    assert_equal(chunk_count(&arr), 0);
}

define_test(for_chunk_array_iterates_chunk_array)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    assert_equal(array_size(&arr), 12);
    assert_equal(chunk_count(&arr), 3);

    for_chunk_array(i, v, &arr)
        *v = i;

    assert_equal(*at(&arr, 3), 3);
    assert_equal(*at(&arr, 5), 5);
    assert_equal(*at(&arr, 7), 7);
    assert_equal(*at(&arr, 8), 8);
    assert_equal(*at(&arr, 11), 11);
}

define_test(hash_hashes_chunk_array)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    for_chunk_array(i, v, &arr)
        *v = i;

    u32 hsh = hash(&arr);

    assert_not_equal(hsh, 0);
}

define_test(index_operator_accesses_nth_element)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    for_chunk_array(i, v, &arr)
        *v = i;

    assert_equal(arr[0], 0);
    assert_equal(arr[2], 2);
    assert_equal(arr[4], 4);
    assert_equal(arr[8], 8);
    assert_equal(arr[11], 11);

    arr[5] = 500;
    assert_equal(arr[5], 500);
}

define_test(remove_chunks_can_free_elements)
{
    chunk_array<string, 2> arr;
    init(&arr, 3);
    defer { free(&arr); };

    // first two are in the first chunk
    arr[0] = "hello"_s;
    arr[1] = "world"_s;

    remove_chunks<true>(&arr, 0, 1);
}

define_test(remove_chunk_from_start_can_free_elements)
{
    chunk_array<string, 2> arr;
    init(&arr, 3);
    defer { free(&arr); };

    // first two are in the first chunk
    arr[0] = "hello"_s;
    arr[1] = "world"_s;

    remove_chunk_from_start<true>(&arr);
}

define_test(remove_chunk_from_end_can_free_elements)
{
    chunk_array<string, 2> arr;
    init(&arr, 3);
    defer { free(&arr); };

    arr[4] = "hello"_s;
    arr[5] = "world"_s;

    remove_chunk_from_end<true>(&arr);
}

define_test(free_can_free_elements)
{
    chunk_array<string, 2> arr;
    init(&arr, 1);
    defer { free<true>(&arr); };

    arr[0] = "hello"_s;
    arr[1] = "world"_s;
}

define_test(search_returns_nullptr_if_key_is_not_found)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    for_chunk_array(i, v, &arr)
        *v = i;
    
    int val = 23;

    assert_equal(search(&arr, &val), nullptr);

    free(&arr);
}

define_test(search_returns_pointer_to_element_when_found)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    for_chunk_array(i, v, &arr)
        *v = i;
    
    int val = 6;

    assert_equal(search(&arr, &val), (*arr.chunks[1]) + 2);

    free(&arr);
}

define_test(index_of_returns_minus_one_if_key_is_not_found)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    for_chunk_array(i, v, &arr)
        *v = i;
    
    int val = 23;

    assert_equal(index_of(&arr, &val), -1);
}

define_test(index_of_returns_index_of_element_when_found)
{
    chunk_array<int, 4> arr;
    init(&arr, 3);
    defer { free(&arr); };

    for_chunk_array(i, v, &arr)
        *v = i;
    
    int val = 6;

    assert_equal(index_of(&arr, &val), 6);
}


define_default_test_main();
