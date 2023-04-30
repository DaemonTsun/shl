
#include <t1/t1.hpp>

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

define_default_test_main();
