
#include <iostream>
#include <string.h>

#include <t1/t1.hpp>

#include "../src/file_stream.hpp"

define_test(file_stream_init_initializes)
{
    file_stream fs;

    init(&fs);

    assert_equal(fs.handle, nullptr);
    assert_equal(fs.size, 0);
    assert_equal(fs.block_size, 1);
}

define_default_test_main();
