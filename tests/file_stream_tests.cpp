
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

#include "shl/number_types.hpp"
#include "shl/defer.hpp"
#include "shl/memory.hpp"
#include "shl/file_stream.hpp"
#include "shl/streams.hpp"

// cmake copies these next to test executable
#define TXT_FILE SYS_CHAR("file_stream_text_data.txt") // 1024 bytes
#define BIN_FILE SYS_CHAR("file_stream_binary_data.bin") // 12 bytes

#if Windows
// TODO: fix windows build.
// right now this just throws some heap corruption error, which considering
// doesn't happen on linux, means that Windows is just a huge pile of shit doing
// things without telling you.
// PS: the "Visual C++ CRT" debug window popping up, disrupting any chance at debugging
// the program AND not telling you what you need to know to debug the program is probably
// one of the worst things I have ever seen in programming. Fuck you, Microsoft.
#else
const sys_char *get_executable_path()
{
    static sys_char pth[4096] = {0};
#if Windows
    auto len = GetModuleFileName(nullptr, pth, 4095);

    assert(len > 0 && len < 4095);
    pth[len] = '\0';

    return pth;
#else

    if (readlink("/proc/self/exe", pth, 4095) < 0)
        return nullptr;
    
    return pth;
#endif
}

string_base<sys_char> get_filepath(const sys_char *file)
{
    string_base<sys_char> ret{};

    const sys_char *exep = get_executable_path();

    if (exep == nullptr)
        return ret;

    set_string(&ret, exep);

    s64 idx = last_index_of(ret, SYS_CHAR('/'));

#if Windows
    if (idx < 0)
        idx = last_index_of(ret, SYS_CHAR('\\'));
#endif

    if (idx < 0)
        idx = ret.size;

    ret.size = idx;

#if Windows
    append_string(&ret, SYS_CHAR("\\"));
#else
    append_string(&ret, SYS_CHAR("/"));
#endif
    append_string(&ret, file);

    return ret;
}

define_test(file_stream_init_opens_file)
{
    file_stream fs;
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    assert_equal(init(&fs, filepath.data), true);
    assert_equal(free(&fs), true);
}

define_test(file_stream_read_reads_file_contents)
{
    file_stream fs;
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

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
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

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
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

    assert_equal(get_file_size(&fs), 12u);
    assert_equal(fs.cached_size, 12u);

    free(&fs);
}

define_test(file_stream_seek_next_alignment_seeks_next_alignment)
{
    file_stream fs;
    defer { free(&fs); };
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

    assert_equal(tell(&fs), 0);
    assert_equal(seek_next_alignment(&fs, 4), 4);
    assert_equal(seek_next_alignment(&fs, 4), 8);
    assert_equal(seek_next_alignment(&fs, 4), 12);

    seek(&fs, 1);

    assert_equal(seek_next_alignment(&fs, 4), 4);
    assert_equal(seek_next_alignment(&fs, 4), 8);

    seek(&fs, 2);

    assert_equal(seek_next_alignment(&fs, 4), 4);
    assert_equal(seek_next_alignment(&fs, 4), 8);

    seek(&fs, 3);

    assert_equal(seek_next_alignment(&fs, 4), 4);
    assert_equal(seek_next_alignment(&fs, 4), 8);

    seek(&fs, 4);

    assert_equal(seek_next_alignment2(&fs, 4), 8);

    // 5
    seek(&fs, 0);

    assert_equal(seek_next_alignment(&fs, 5), 5);
    assert_equal(seek_next_alignment(&fs, 5), 10);
}

define_test(file_stream_read_block_reads_block)
{
    file_stream fs;
    defer { free(&fs); };
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

    u32 r;
    assert_equal(read_block(&fs, &r, sizeof(u32)), sizeof(u32));
    assert_equal(r, 0x30303030u);

    assert_equal(read_block(&fs, &r, sizeof(u32)), sizeof(u32));
    assert_equal(r, 0x00636261u);

    assert_equal(read_block(&fs, &r, sizeof(u32)), sizeof(u32));
    assert_equal(r, 0x10101010u);

    assert_equal(read_block(&fs, &r, sizeof(u32)), 0);

    seek(&fs, 10); // input is 12 bytes long, cannot read a 4 byte block from position 10!
    assert_equal(read_block(&fs, &r, sizeof(u32)), 0);
}

define_test(file_stream_read_block_reads_nth_block)
{
    file_stream fs;
    defer { free(&fs); };
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

    u32 r;
    assert_equal(read_block(&fs, &r, 0, sizeof(u32)), sizeof(u32));
    assert_equal(r, 0x30303030u);

    assert_equal(read_block(&fs, &r, 1, sizeof(u32)), sizeof(u32));
    assert_equal(r, 0x00636261u);

    assert_equal(read_block(&fs, &r, 2, sizeof(u32)), sizeof(u32));
    assert_equal(r, 0x10101010u);

    assert_equal(read_block(&fs, &r, 3, sizeof(u32)), 0);
}

define_test(file_stream_read_blocks_reads_blocks)
{
    file_stream fs;
    defer { free(&fs); };
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);

    u32 r[3] = {0};
    assert_equal(read_blocks(&fs, r, 0, sizeof(u32)), 0);
    assert_equal(read_blocks(&fs, r, 1, sizeof(u32)), 1);
    assert_equal(r[0], 0x30303030u);

    seek(&fs, 0);

    assert_equal(read_blocks(&fs, r, 2, sizeof(u32)), 2);
    assert_equal(r[0], 0x30303030u);
    assert_equal(r[1], 0x00636261u);

    seek(&fs, 0);

    assert_equal(read_blocks(&fs, r, 3, sizeof(u32)), 3);
    assert_equal(r[0], 0x30303030u);
    assert_equal(r[1], 0x00636261u);
    assert_equal(r[2], 0x10101010u);

    seek(&fs, 6);

    assert_equal(read_blocks(&fs, r, 3, sizeof(u32)), 1);
    assert_equal(r[0], 0x10100063u);
}

define_test(file_stream_read_entire_file_reads_entire_file)
{
    file_stream fs;
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    init(&fs, filepath.data);
    assert_equal(get_file_size(&fs), 12u);

    char contents[12];
    assert_equal(read_entire_file(&fs, contents), 12u);
    assert_equal(strncmp(contents + 4, "abc", 4), 0);

    free(&fs);
}

define_test(streams_read_entire_file_reads_entire_file)
{
    string_base<sys_char> filepath = get_filepath(BIN_FILE);
    defer { free(&filepath); };

    memory_stream ms{};

    // read_entire_file acts as init()
    assert_equal(read_entire_file(filepath.data, &ms), true);
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
#endif

define_default_test_main();
