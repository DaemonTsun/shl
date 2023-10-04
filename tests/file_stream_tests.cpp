
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <t1/t1.hpp>

#include "shl/platform.hpp"

#if Windows
#include <windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif

#include <filesystem>

#include "shl/number_types.hpp"
#include "shl/defer.hpp"
#include "shl/memory.hpp"
#include "shl/file_stream.hpp"
#include "shl/streams.hpp"

// cmake copies these next to test executable
#define TXT_FILE "file_stream_text_data.txt" // 1024 bytes
#define BIN_FILE "file_stream_binary_data.bin" // 12 bytes

std::filesystem::path get_executable_path()
{
#if Windows
    char pth[4096] = {0};

    auto length = GetModuleFileName(nullptr, pth, 4095);

    if (length == 0)
        return std::filesystem::path();

    return std::filesystem::path(pth);
#else
    char pth[PATH_MAX] = {0};

    if (readlink("/proc/self/exe", pth, PATH_MAX) < 0)
        return std::filesystem::path();
    
    return std::filesystem::path(pth);
#endif
}

char *get_filepath(const char *file)
{
    char *ret = nullptr;

    std::filesystem::path p = get_executable_path();
    p = p.parent_path() / file;

#if Windows
    // thanks windows
    const wchar_t *name = p.c_str();
    size_t len = wcslen(name) * 4;
    ret = (char*)malloc(len + 1);
    memset(ret, 0, len + 1);

    wcstombs(ret, name, len);

#else
    const char *name = p.c_str();
    size_t len = strlen(name);
    ret = (char*)malloc(len+1);
    memset(ret, 0, len+1);

    memcpy(ret, name, len);

#endif

    return ret;
}

define_test(file_stream_init_initializes)
{
    file_stream fs;

    init(&fs);

    assert_equal(fs.handle, nullptr);
    assert_equal(fs.size, 0);
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
    char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs);
    assert_equal(open(&fs, filepath), true);
    assert_equal(close(&fs), true);
}

define_test(file_stream_read_reads_file_contents)
{
    file_stream fs;
    char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

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

define_test(file_stream_read_at_reads_file_contents_at_position)
{
    file_stream fs;
    char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs);
    assert_equal(open(&fs, filepath), true);

    u32 r;
    assert_equal(read_at(&fs, &r, 0), sizeof(u32));
    assert_equal(r, 0x30303030u);

    char c[2];
    assert_equal(read_at(&fs, &c, 6, 2), 2);
    assert_equal(strncmp("c", c, 2), 0);

    assert_equal(read_at(&fs, &c, 4, 2), 2);
    assert_equal(strncmp("ab", c, 2), 0);

    assert_equal(read_at(&fs, &r, 8), sizeof(u32));
    assert_equal(r, 0x10101010u);

    assert_equal(close(&fs), true);
}

define_test(file_stream_calculate_size_calculates_and_set_size)
{
    file_stream fs;
    char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs);
    assert_equal(open(&fs, filepath), true);
    assert_equal(calculate_file_size(&fs), 12u);
    assert_equal(fs.size, 12u);

    assert_equal(close(&fs), true);
}

define_test(file_stream_read_entire_file_reads_entire_file)
{
    file_stream fs;
    char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs);
    assert_equal(open(&fs, filepath), true);
    assert_equal(calculate_file_size(&fs), 12u);

    char contents[12];
    assert_equal(read_entire_file(&fs, contents), 12u);
    assert_equal(strncmp(contents + 4, "abc", 4), 0);

    assert_equal(close(&fs), true);
}

define_test(streams_read_entire_file_reads_entire_file)
{
    char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    memory_stream ms{};

    assert_equal(read_entire_file(filepath, &ms), true);
    assert_equal(strncmp(ms.data + 4, "abc", 4), 0);
    assert_equal(close(&ms), true);
}

define_test(streams_read_entire_file_yields_error_on_nonexistent_path)
{
    memory_stream ms{};
    defer { close(&ms); };

    error err;

    assert_equal(read_entire_file("abc", &ms, &err), false);

    // err.what will be something like "could not open file 'abc': no such file or directory"
    assert_not_equal(err.what, nullptr);
}

define_default_test_main();
