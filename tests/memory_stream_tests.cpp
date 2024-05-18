
#include <string.h>

#include <t1/t1.hpp>

#include "shl/number_types.hpp"
#include "shl/memory_stream.hpp"

define_test(init_initializes_memory_stream)
{
    memory_stream mem{};

    init(&mem, 64);

    assert_not_equal(mem.data, nullptr);
    assert_equal(mem.size, 64);
    assert_equal(mem.position, 0);

    free(&mem);
}

define_test(memory_stream_is_open_checks_if_is_open)
{
    memory_stream mem{};

    // false only because null initialized
    assert_equal(is_open(&mem), false);

    init(&mem, 64);

    assert_equal(is_open(&mem), true);

    free(&mem);
    assert_equal(is_open(&mem), false);
}

define_test(memory_stream_is_at_end_checks_if_stream_is_at_end)
{
    memory_stream mem{};

    init(&mem, 64);

    assert_equal(is_at_end(&mem), false);

    seek(&mem, 63);
    assert_equal(is_at_end(&mem), false);

    seek(&mem, 64);
    assert_equal(is_at_end(&mem), true);

    free(&mem);
}

define_test(memory_stream_is_ok_checks_if_stream_is_readable)
{
    memory_stream mem{};

    assert_equal(is_ok(&mem), false);

    init(&mem, 16);
    assert_equal(is_ok(&mem), true);

    seek(&mem, 16);
    assert_equal(is_ok(&mem), false);

    free(&mem);
    assert_equal(is_ok(&mem), false);
}

define_test(memory_stream_block_count_returns_block_count)
{
    memory_stream mem{};

    init(&mem, 16);
    assert_equal(block_count(&mem, 1), 16);
    assert_equal(block_count(&mem, 2), 8);
    assert_equal(block_count(&mem, 4), 4);
    assert_equal(block_count(&mem, 8), 2);
    assert_equal(block_count(&mem, 16), 1);

    free(&mem);
}

define_test(memory_stream_current_returns_current_position_pointer)
{
    memory_stream mem{};

    init(&mem, 16);
    assert_equal(current(&mem), mem.data);

    seek(&mem, 4);
    assert_equal(current(&mem), mem.data + 4);

    free(&mem);
}

define_test(memory_stream_current_block_start_returns_start_of_current_block)
{
    memory_stream mem{};

    init(&mem, 16);
    assert_equal(current_block_start(&mem, 1), mem.data);

    seek(&mem, 2);
    assert_equal(current_block_start(&mem, 4), mem.data + 0);
    seek(&mem, 4);
    assert_equal(current_block_start(&mem, 4), mem.data + 4);
    seek(&mem, 6);
    assert_equal(current_block_start(&mem, 4), mem.data + 4);

    free(&mem);
}

define_test(memory_stream_seek_seeks)
{
    memory_stream mem{};

    init(&mem, 16);

    assert_equal(mem.position, 0);

    seek(&mem, 2);
    assert_equal(mem.position, 2);

    seek(&mem, 2);
    assert_equal(mem.position, 2);

    assert_equal(seek(&mem, 2, IO_SEEK_CUR), 4);
    assert_equal(mem.position, 4);

    assert_equal(seek(&mem, -2, IO_SEEK_END), 14);
    assert_equal(mem.position, 14);

    seek(&mem, 20);
    assert_equal(mem.position, 16);

    free(&mem);
}

define_test(memory_stream_seek_block_seeks_block)
{
    memory_stream mem{};

    init(&mem, 16);

    seek_block(&mem, 0, 4);
    assert_equal(mem.position, 0);

    seek_block(&mem, 1, 4);
    assert_equal(mem.position, 4);

    seek_block(&mem, 1, 4);
    assert_equal(mem.position, 4);

    assert_equal(seek_block(&mem, 1, 4, IO_SEEK_CUR), 8);
    assert_equal(mem.position, 8);

    seek_block(&mem, 20, 4);
    assert_equal(mem.position, 16);

    free(&mem);
}

define_test(memory_stream_read_block_reads_block)
{
    memory_stream mem{};
    defer { free(&mem); };

    init(&mem, 12);
    u32 *data = (u32*)mem.data;
    *data++ = 0x30303030u;
    *data++ = 0x00636261u;
    *data++ = 0x10101010u;

    u32 r;
    assert_equal(read_block(&mem, &r, sizeof(u32)), (s64)sizeof(u32));
    assert_equal(r, 0x30303030u);

    assert_equal(read_block(&mem, &r, sizeof(u32)), (s64)sizeof(u32));
    assert_equal(r, 0x00636261u);

    assert_equal(read_block(&mem, &r, sizeof(u32)), (s64)sizeof(u32));
    assert_equal(r, 0x10101010u);

    assert_equal(read_block(&mem, &r, sizeof(u32)), 0);

    seek(&mem, 10); // input is 12 bytes long, cannot read a 4 byte block from position 10!
    assert_equal(read_block(&mem, &r, sizeof(u32)), 0);
}

define_test(memory_stream_read_block_reads_nth_block)
{
    memory_stream mem;
    defer { free(&mem); };

    init(&mem, 12);
    u32 *data = (u32*)mem.data;
    *data++ = 0x30303030u;
    *data++ = 0x00636261u;
    *data++ = 0x10101010u;

    u32 r;
    assert_equal(read_block(&mem, &r, 0, sizeof(u32)), (s64)sizeof(u32));
    assert_equal(r, 0x30303030u);

    assert_equal(read_block(&mem, &r, 1, sizeof(u32)), (s64)sizeof(u32));
    assert_equal(r, 0x00636261u);

    assert_equal(read_block(&mem, &r, 2, sizeof(u32)), (s64)sizeof(u32));
    assert_equal(r, 0x10101010u);

    assert_equal(read_block(&mem, &r, 3, sizeof(u32)), 0);
}

define_test(memory_stream_read_blocks_reads_blocks)
{
    memory_stream mem;
    defer { free(&mem); };

    init(&mem, 12);
    u32 *data = (u32*)mem.data;
    *data++ = 0x30303030u;
    *data++ = 0x00636261u;
    *data++ = 0x10101010u;

    u32 r[3] = {0};
    assert_equal(read_blocks(&mem, r, 0, sizeof(u32)), 0);
    assert_equal(read_blocks(&mem, r, 1, sizeof(u32)), 1);
    assert_equal(r[0], 0x30303030u);

    seek(&mem, 0);

    assert_equal(read_blocks(&mem, r, 2, sizeof(u32)), 2);
    assert_equal(r[0], 0x30303030u);
    assert_equal(r[1], 0x00636261u);

    seek(&mem, 0);

    assert_equal(read_blocks(&mem, r, 3, sizeof(u32)), 3);
    assert_equal(r[0], 0x30303030u);
    assert_equal(r[1], 0x00636261u);
    assert_equal(r[2], 0x10101010u);

    seek(&mem, 6);

    assert_equal(read_blocks(&mem, r, 3, sizeof(u32)), 1);
    assert_equal(r[0], 0x10100063u);
}

define_test(memory_stream_seek_next_alignment_seeks_to_next_alignment)
{
    memory_stream mem{};

    init(&mem, 16);

    assert_equal(mem.position, 0);

    seek_next_alignment(&mem, 2);
    assert_equal(mem.position, 2);

    mem.position = 1;

    seek_next_alignment(&mem, 2);
    assert_equal(mem.position, 2);

    seek_next_alignment(&mem, 4);
    assert_equal(mem.position, 4);

    seek_next_alignment(&mem, 4);
    assert_equal(mem.position, 8);
    mem.position = 5;

    seek_next_alignment(&mem, 4);
    assert_equal(mem.position, 8);

    free(&mem);
}

define_test(memory_stream_read_reads)
{
    const char *data = "hello world";
    memory_stream mem{};

    init(&mem, 16);

    write(&mem, data, strlen(data));

    char null[5] = {0};
    write(&mem, &null, 5);

    u32 rdata;
    mem.position = 0;
    assert_equal(read(&mem, &rdata, 4), 4);
    assert_equal(rdata, 0x6c6c6568u); // "hell" in u32 little endian

    mem.position = 14;
    assert_equal(read(&mem, &rdata, 4), 2);
    assert_equal(rdata, 0x6c6c0000u);

    free(&mem);
}

define_test(memory_stream_read_with_number_of_items_reads)
{
    const char *data = "hello world";
    memory_stream mem{};

    init(&mem, 16);

    write(&mem, data, strlen(data));
    mem.position = 0;

    u32 rdata;
    assert_equal(read(&mem, &rdata, 2, 2), 2);
    assert_equal(rdata, 0x6c6c6568u); // "hell" in u32 little endian

    mem.position = 14;
    assert_equal(read(&mem, &rdata, 4, 1), 0);
    assert_equal(rdata, 0x6c6c6568u);

    free(&mem);
}

define_test(memory_stream_read_with_type_ptr_reads)
{
    const char *data = "hello world";
    memory_stream mem{};

    init(&mem, 16);

    write(&mem, data, strlen(data));

    char null[5] = {0};
    write(&mem, &null, 5);

    u32 x = 0;
    write_at(&mem, &x, 12, 0);
    mem.position = 0;

    u32 rdata;
    assert_equal(read(&mem, &rdata), 4);
    assert_equal(rdata, 0x6c6c6568u); // "hell" in u32 little endian

    mem.position = 14;
    assert_equal(read(&mem, &rdata), 2);
    assert_equal(rdata, 0x6c6c0000u);

    free(&mem);
}

define_test(memory_stream_hash_hashes_memory_stream)
{
    const char *data = "hello world";
    memory_stream mem{};

    init(&mem, 11);

    write(&mem, data, strlen(data));
    mem.position = 0;

    hash_t hsh = hash(&mem);
    assert_equal(hash(&mem), hsh);
    assert_equal(hash(&mem), hsh);
    assert_equal(hash(&mem), hsh);
    assert_equal(hash(&mem), hsh);

    write_at(&mem, "abc", 0, 3);

    assert_not_equal(hash(&mem), hsh);

    free(&mem);
}

define_default_test_main();
