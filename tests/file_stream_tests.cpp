
#include <iostream>
#include <string.h>
#include <filesystem>

#include <t1/t1.hpp>

#include "../src/number_types.hpp"
#include "../src/filesystem.hpp"
#include "../src/file_stream.hpp"

// cmake copies these next to test executable
#define TXT_FILE "file_stream_text_data.txt"
#define BIN_FILE "file_stream_binary_data.bin"

#define GET_FILEPATH(File, Name) \
    std::filesystem::path p = get_executable_path();\
    p = p.parent_path() / File;\
    const char *Name = p.c_str();

define_test(file_stream_init_initializes)
{
    file_stream fs;

    init(&fs);

    assert_equal(fs.handle, nullptr);
    assert_equal(fs.size, 0);
    assert_equal(fs.block_size, 1);
}

define_test(file_stream_close_on_unopened_file_returns_true)
{
    file_stream fs;

    init(&fs);
    assert_equal(close(&fs), true);
}

define_test(file_stream_open_opens_file)
{
    file_stream fs;
    GET_FILEPATH(BIN_FILE, filepath);

    init(&fs);
    assert_equal(open(&fs, filepath), true);
    assert_equal(close(&fs), true);
}

define_test(file_stream_read_reads_file_contents)
{
    file_stream fs;
    GET_FILEPATH(BIN_FILE, filepath);

    init(&fs);
    assert_equal(open(&fs, filepath), true);

    u32 r;
    assert_equal(read(&fs, &r), sizeof(u32));
    assert_equal(r, 0x30303030u);

    char c[4];
    assert_equal(read(&fs, &c, 4), 4);
    assert_equal(strncmp("abc", c, 4), 0);

    assert_equal(read(&fs, &r), sizeof(u32));
    assert_equal(r, 0x10101010u);

    assert_equal(close(&fs), true);
}

define_default_test_main();
