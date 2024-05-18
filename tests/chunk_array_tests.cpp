
#define t1_use_stdlib 1
#include <t1/t1.hpp>

#include "shl/string.hpp"
#include "shl/defer.hpp"
#include "shl/chunk_array.hpp"

define_test(init_initializes_chunk_array)
{
    chunk_array<int, 8> arr;
    init(&arr);
    defer { free(&arr); };

    assert_equal(arr.all_chunks.data, nullptr);
    assert_equal(arr.nonfull_chunks.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(chunk_count(&arr), 0);
}

define_test(zero_init_initializes_chunk_array)
{
    chunk_array<int, 8> arr{};
    defer { free(&arr); };

    assert_equal(arr.all_chunks.data, nullptr);
    assert_equal(arr.nonfull_chunks.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(chunk_count(&arr), 0);
}

define_test(add_element_adds_element_to_empty_chunk_array)
{
    chunk_array<int, 8> arr{};
    defer { free(&arr); };

    assert_equal(arr.all_chunks.data, nullptr);
    assert_equal(arr.nonfull_chunks.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(chunk_count(&arr), 0);

    add_element(&arr, 10);

    assert_not_equal(arr.all_chunks.data, nullptr);
    assert_not_equal(arr.nonfull_chunks.data, nullptr);
    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);
    assert_equal(arr.all_chunks[0]->data[0], 10);
    assert_equal(arr.nonfull_chunks.size, 1);
}

define_test(add_element_automatically_adds_more_chunks)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    assert_equal(arr.all_chunks.data, nullptr);
    assert_equal(arr.nonfull_chunks.data, nullptr);
    assert_equal(arr.size, 0);
    assert_equal(chunk_count(&arr), 0);

    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);
    add_element(&arr, 4);
    add_element(&arr, 5);

    assert_not_equal(arr.all_chunks.data, nullptr);
    assert_not_equal(arr.nonfull_chunks.data, nullptr);
    assert_equal(arr.size, 5);
    assert_equal(chunk_count(&arr), 2);
    assert_equal(arr.all_chunks[0]->data[0], 1);
    assert_equal(arr.all_chunks[0]->data[1], 2);
    assert_equal(arr.all_chunks[0]->data[2], 3);
    assert_equal(arr.all_chunks[0]->data[3], 4);
    assert_equal(arr.all_chunks[1]->data[0], 5);
    assert_equal(arr.nonfull_chunks.size, 1);
    assert_equal(arr.nonfull_chunks[0], arr.all_chunks[1]); // only second chunk is nonfull
}

define_test(remove_element_does_nothing_if_index_is_outside_chunk_count)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);

    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);

    remove_element(&arr, {.chunk_index = 2, .slot_index = 0});

    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);
}

define_test(remove_element_does_nothing_if_index_is_outside_element_count)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);

    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);

    remove_element(&arr, {.chunk_index = 0, .slot_index = 5});

    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);
}

define_test(remove_element_removes_element_from_chunk)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);

    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);

    remove_element(&arr, {.chunk_index = 0, .slot_index = 0});

    assert_equal(arr.size, 0);
    assert_equal(chunk_count(&arr), 1);
    assert_equal(arr.nonfull_chunks.size, 1);
    assert_equal(arr.nonfull_chunks[0]->used_count, 0);
}

define_test(remove_element_re_adds_chunk_to_nonfull_chunks)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);

    assert_equal(arr.size, 1);
    assert_equal(chunk_count(&arr), 1);
    assert_equal(arr.nonfull_chunks.size, 1);

    add_element(&arr, 1);
    add_element(&arr, 1);
    add_element(&arr, 1);

    assert_equal(arr.size, 4);
    assert_equal(arr.nonfull_chunks.size, 0);

    remove_element(&arr, {.chunk_index = 0, .slot_index = 0});

    assert_equal(arr.size, 3);
    assert_equal(arr.nonfull_chunks.size, 1);
    assert_equal(arr.nonfull_chunks[0]->used_count, 3);
    assert_equal(arr.nonfull_chunks[0]->used[0], false);
    assert_equal(arr.nonfull_chunks[0]->used[1], true);
    assert_equal(arr.nonfull_chunks[0]->used[2], true);
    assert_equal(arr.nonfull_chunks[0]->used[3], true);
}

define_test(at_returns_nullptr_if_index_is_out_of_range)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);
    add_element(&arr, 4);
    add_element(&arr, 5);

    assert_equal(arr.size, 5);

    int *ret = at(&arr, {.chunk_index = 3, .slot_index = 0});

    assert_equal(ret, nullptr);

    ret = at(&arr, {.chunk_index = 0, .slot_index = 6});

    assert_equal(ret, nullptr);
}

define_test(at_gets_element_by_chunk_and_slot)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);
    add_element(&arr, 4);
    add_element(&arr, 5);

    assert_equal(arr.size, 5);

    int *ret = at(&arr, {.chunk_index = 0, .slot_index = 0});

    assert_equal(ret, arr.all_chunks[0]->data + 0);

    ret = at(&arr, {.chunk_index = 0, .slot_index = 3});

    assert_equal(ret, arr.all_chunks[0]->data + 3);

    chunk_item_index index = {.chunk_index = 1, .slot_index = 0};
    ret = at(&arr, index);

    assert_equal(ret, arr.all_chunks[1]->data + 0);
    assert_equal(*ret, arr[index]);
}

define_test(int_index_operator_gets_nth_chunk)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);
    add_element(&arr, 2);

    assert_equal(arr.size, 2);

    assert_equal(&arr[0], arr.all_chunks[0]);
}

define_test(clear_clears_chunk_array)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);
    add_element(&arr, 4);
    add_element(&arr, 5);

    assert_equal(arr.size, 5);
    assert_equal(arr.all_chunks.size, 2);
    assert_equal(arr.nonfull_chunks.size, 1);

    clear(&arr);

    assert_equal(arr.size, 0);
    assert_equal(arr.all_chunks.size, 2);
    assert_equal(arr.nonfull_chunks.size, 2);

    for_array(chnk_, &arr.all_chunks)
    {
        chunk<int, 4> *chnk = *chnk_;

        assert_equal(chnk->used_count, 0);

        for_array(used, &chnk->used)
            assert_equal(*used, false);
    }
}

define_test(free_values_frees_used_values_of_chunk_array)
{
    chunk_array<string, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, "hello"_s);
    add_element(&arr, "world"_s);
    add_element(&arr, "bye"_s);

    assert_equal(arr.size, 3);
    assert_equal(arr.all_chunks.size, 1);
    assert_equal(arr.nonfull_chunks.size, 1);

    free_values(&arr);

    // strings still exist but their memory is freed
    assert_equal(arr.size, 3);
    assert_equal(arr.all_chunks.size, 1);
    assert_equal(arr.nonfull_chunks.size, 1);
}

define_test(free_can_free_values)
{
    chunk_array<string, 4> arr{};

    add_element(&arr, "hello"_s);
    add_element(&arr, "world"_s);
    add_element(&arr, "bye"_s);

    assert_equal(arr.size, 3);
    assert_equal(arr.all_chunks.size, 1);
    assert_equal(arr.nonfull_chunks.size, 1);

    // would leak memory without the <true> 
    free<true>(&arr);

    assert_equal(arr.size, 0);
    assert_equal(arr.all_chunks.size, 0);
    assert_equal(arr.nonfull_chunks.size, 0);
}

define_test(search_returns_nullptr_if_not_found)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);

    int *ret = search(&arr, 4);

    assert_equal(ret, nullptr);
}

define_test(search_returns_pointer_to_element_if_found)
{
    chunk_array<int, 4> arr{};
    defer { free(&arr); };

    add_element(&arr, 1);
    add_element(&arr, 2);
    add_element(&arr, 3);

    int *ret = search(&arr, 2);

    assert_equal(ret, arr[0].data + 1);
    assert_equal(*ret, arr[0][1]);
}

define_test(hash_hashes_chunk_array)
{
    chunk_array<int, 4> arr1{};
    chunk_array<int, 4> arr2{};
    chunk_array<int, 4> arr3{};
    defer { free(&arr1); };
    defer { free(&arr2); };
    defer { free(&arr3); };

    add_element(&arr1, 1);
    add_element(&arr1, 2);

    add_element(&arr2, 1);
    add_element(&arr2, 2);

    add_element(&arr3, 3);
    add_element(&arr3, 5);

    hash_t h1 = hash(&arr1);
    hash_t h2 = hash(&arr2);
    hash_t h3 = hash(&arr3);

    assert_equal(h1, h2);
    assert_not_equal(h1, h3);
}

define_test(for_chunk_array_iterates_chunk_array)
{
    chunk_array<chunk_item_index, 3> arr{};
    defer { free(&arr); };

    add_element(&arr, {0, 0});
    add_element(&arr, {0, 0});
    add_element(&arr, {0, 0});
    add_element(&arr, {0, 0});

    for_chunk_array(i, v, &arr)
        *v = i;

    assert_equal(arr[0][0].chunk_index, 0);
    assert_equal(arr[0][0].slot_index,  0);

    assert_equal(arr[0][1].chunk_index, 0);
    assert_equal(arr[0][1].slot_index,  1);

    assert_equal(arr[0][2].chunk_index, 0);
    assert_equal(arr[0][2].slot_index,  2);

    assert_equal(arr[1][0].chunk_index, 1);
    assert_equal(arr[1][0].slot_index,  0);
}

define_default_test_main();
