
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <t1/t1.hpp>

#include "shl/platform.hpp"

#if Windows
#include <windows.h>
#else
#define pipe __original_pipe
#include <unistd.h>
#undef pipe
#include <linux/limits.h>
#endif

#include <filesystem>

#include "shl/number_types.hpp"
#include "shl/defer.hpp"
#include "shl/memory.hpp"
#include "shl/file_stream.hpp"
#include "shl/streams.hpp"

// cmake copies these next to test executable
#define TXT_FILE SYS_CHAR("file_stream_text_data.txt") // 1024 bytes
#define BIN_FILE SYS_CHAR("file_stream_binary_data.bin") // 12 bytes

std::filesystem::path get_executable_path()
{
#if Windows
    sys_char pth[4096] = {0};

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

sys_char *get_filepath(const sys_char *file)
{
    sys_char *ret = nullptr;

    std::filesystem::path p = get_executable_path();
    p = p.parent_path() / file;

    const sys_char *name = p.c_str();
    u64 len = string_length(name) * sizeof(sys_char);
    ret = (sys_char*)malloc(len + sizeof(sys_char));
    memset(ret, 0, len + 1);
    memcpy(ret, name, len);

    return ret;
}

define_test(file_stream_init_opens_file)
{
    file_stream fs;
    sys_char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    assert_equal(init(&fs, filepath), true);
    assert_equal(free(&fs), true);
}

define_test(file_stream_read_reads_file_contents)
{
    file_stream fs;
    sys_char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs, filepath);

    u32 r;
    assert_equal(read(&fs, &r), sizeof(u32));
    assert_equal(r, 0x30303030u);

    char c[4];
    assert_equal(read(&fs, &c, 4), 4);
    assert_equal(strncmp("abc", c, 4), 0);

    assert_equal(read(&fs, &r), sizeof(u32));
    assert_equal(r, 0x10101010u);

    free(&fs);
}

define_test(file_stream_read_at_reads_file_contents_at_position)
{
    file_stream fs;
    sys_char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs, filepath);

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

    free(&fs);
}

define_test(file_stream_get_size_gets_size_and_sets_cached_size)
{
    file_stream fs;
    sys_char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs, filepath);

    assert_equal(get_file_size(&fs), 12u);
    assert_equal(fs.cached_size, 12u);

    free(&fs);
}

define_test(file_stream_read_entire_file_reads_entire_file)
{
    file_stream fs;
    sys_char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    init(&fs, filepath);
    assert_equal(get_file_size(&fs), 12u);

    char contents[12];
    assert_equal(read_entire_file(&fs, contents), 12u);
    assert_equal(strncmp(contents + 4, "abc", 4), 0);

    free(&fs);
}

define_test(streams_read_entire_file_reads_entire_file)
{
    sys_char *filepath = get_filepath(BIN_FILE);
    defer { free_memory(filepath); };

    memory_stream ms{};

    // read_entire_file acts as init()
    assert_equal(read_entire_file(filepath, &ms), true);
    assert_equal(strncmp(ms.data + 4, "abc", 4), 0);
    free(&ms);
}

define_test(streams_read_entire_file_yields_error_on_nonexistent_path)
{
    memory_stream ms{};
    defer { free(&ms); };

    error err;

    assert_equal(read_entire_file("abc", &ms, &err), false);

    assert_not_equal(err.error_code, 0);
}

define_default_test_main();
