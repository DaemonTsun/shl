
#include <string.h>

#include <t1/t1.hpp>

#include "shl/number_types.hpp"
#include "shl/memory_stream.hpp"

define_test(memory_stream_initializes)
{
    memory_stream mem;

    init(&mem);

    assert_equal(mem.data, nullptr);
    assert_equal(mem.size, 0);
    assert_equal(mem.block_size, 1);
    assert_equal(mem.position, 0);
}

define_test(memory_stream_open_opens_new_stream)
{
    memory_stream mem;

    init(&mem);

    assert_equal(open(&mem, 16), true);

    assert_not_equal(mem.data, nullptr);
    assert_equal(mem.size, 16);
    assert_equal(mem.block_size, 1);
    assert_equal(mem.position, 0);

    close(&mem);
}

define_test(memory_stream_is_open_checks_if_is_open)
{
    memory_stream mem;

    init(&mem);
    assert_equal(is_open(&mem), false);

    assert_equal(open(&mem, 16), true);
    assert_equal(is_open(&mem), true);

    close(&mem);
    assert_equal(is_open(&mem), false);
}

define_test(memory_stream_is_at_end_checks_if_stream_is_at_end)
{
    memory_stream mem;

    init(&mem);

    assert_equal(open(&mem, 16), true);
    assert_equal(is_at_end(&mem), false);

    seek(&mem, 16);
    assert_equal(is_at_end(&mem), true);

    close(&mem);
}

define_test(memory_stream_is_ok_checks_if_stream_is_ok)
{
    memory_stream mem;

    init(&mem);
    assert_equal(is_ok(&mem), false);

    assert_equal(open(&mem, 16), true);
    assert_equal(is_ok(&mem), true);

    seek(&mem, 16);
    assert_equal(is_ok(&mem), false);

    close(&mem);
    assert_equal(is_ok(&mem), false);
}

define_test(memory_stream_block_count_returns_block_count)
{
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    assert_equal(block_count(&mem), 16);

    mem.block_size = 2;
    assert_equal(block_count(&mem), 8);

    mem.block_size = 4;
    assert_equal(block_count(&mem), 4);

    close(&mem);
}

define_test(memory_stream_current_returns_current_position_pointer)
{
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    assert_equal(current(&mem), mem.data);

    seek(&mem, 4);
    assert_equal(current(&mem), mem.data + 4);

    close(&mem);
}

define_test(memory_stream_current_block_start_returns_start_of_current_block)
{
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    assert_equal(current_block_start(&mem), mem.data);

    mem.block_size = 4;
    seek(&mem, 2);
    assert_equal(current_block_start(&mem), mem.data + 0);
    seek(&mem, 4);
    assert_equal(current_block_start(&mem), mem.data + 4);
    seek(&mem, 6);
    assert_equal(current_block_start(&mem), mem.data + 4);

    close(&mem);
}

define_test(memory_stream_seek_seeks)
{
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    assert_equal(mem.position, 0);

    seek(&mem, 2);
    assert_equal(mem.position, 2);

    seek(&mem, 2);
    assert_equal(mem.position, 2);

    assert_equal(seek(&mem, 2, SEEK_CUR), 0);
    assert_equal(mem.position, 4);

    assert_equal(seek(&mem, -2, SEEK_END), 0);
    assert_equal(mem.position, 14);

    seek(&mem, 20);
    assert_equal(mem.position, 16);

    close(&mem);
}

define_test(memory_stream_seek_block_seeks_block)
{
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);

    mem.block_size = 4;
    seek_block(&mem, 0);
    assert_equal(mem.position, 0);

    seek_block(&mem, 1);
    assert_equal(mem.position, 4);

    seek_block(&mem, 1);
    assert_equal(mem.position, 4);

    assert_equal(seek_block(&mem, 1, SEEK_CUR), 0);
    assert_equal(mem.position, 8);

    seek_block(&mem, 20);
    assert_equal(mem.position, 16);

    close(&mem);
}

define_test(memory_stream_seek_next_alignment_seeks_to_next_alignment)
{
    memory_stream mem;

    init(&mem);
    assert_equal(open(&mem, 16), true);
    assert_equal(mem.position, 0);

    seek_next_alignment(&mem, 2);
    assert_equal(mem.position, 0);

    mem.position = 1;

    seek_next_alignment(&mem, 2);
    assert_equal(mem.position, 2);

    seek_next_alignment(&mem, 4);
    assert_equal(mem.position, 4);

    seek_next_alignment(&mem, 4);
    assert_equal(mem.position, 4);
    mem.position = 5;

    seek_next_alignment(&mem, 4);
    assert_equal(mem.position, 8);

    close(&mem);
}


define_test(memory_stream_read_reads)
{
    const char *data = "hello world";
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    write(&mem, data, strlen(data));

    char null[5] = {0};
    write(&mem, &null, 5);

    u32 rdata;
    mem.position = 0;
    assert_equal(read(&mem, &rdata, 4), 4);
    assert_equal(rdata, 0x6c6c6568); // "hell" in u32 little endian

    mem.position = 14;
    assert_equal(read(&mem, &rdata, 4), 2);
    assert_equal(rdata, 0x6c6c0000);

    close(&mem);
}

define_test(memory_stream_read_with_number_of_items_reads)
{
    const char *data = "hello world";
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    write(&mem, data, strlen(data));
    mem.position = 0;

    u32 rdata;
    assert_equal(read(&mem, &rdata, 2, 2), 2);
    assert_equal(rdata, 0x6c6c6568); // "hell" in u32 little endian

    mem.position = 14;
    assert_equal(read(&mem, &rdata, 4, 1), 0);
    assert_equal(rdata, 0x6c6c6568);

    close(&mem);
}

define_test(memory_stream_read_with_type_ptr_reads)
{
    const char *data = "hello world";
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 16), true);
    write(&mem, data, strlen(data));

    char null[5] = {0};
    write(&mem, &null, 5);

    u32 x = 0;
    write_at(&mem, &x, 12, 0);
    mem.position = 0;

    u32 rdata;
    assert_equal(read(&mem, &rdata), 4);
    assert_equal(rdata, 0x6c6c6568); // "hell" in u32 little endian

    mem.position = 14;
    assert_equal(read(&mem, &rdata), 2);
    assert_equal(rdata, 0x6c6c0000);

    close(&mem);
}

define_test(memory_stream_hash_hashes_memory_stream)
{
    const char *data = "hello world";
    memory_stream mem;

    init(&mem); // inits block_count to 1
    assert_equal(open(&mem, 11), true);
    write(&mem, data, strlen(data));
    mem.position = 0;

    hash_t hsh = hash(&mem);
    assert_equal(hash(&mem), hsh);
    assert_equal(hash(&mem), hsh);
    assert_equal(hash(&mem), hsh);
    assert_equal(hash(&mem), hsh);

    write_at(&mem, "abc", 0, 3);

    assert_not_equal(hash(&mem), hsh);

    close(&mem);
}

define_default_test_main();
