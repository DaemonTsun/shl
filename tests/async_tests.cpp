
#include "t1/t1.hpp"

#include "shl/print.hpp"
#include "shl/time.hpp"
#include "shl/defer.hpp"
#include "shl/string.hpp"
#include "shl/async_io.hpp"

typedef string_base<sys_char> sys_string;

// cmake copies these next to test executable
#define HELLO_WORLD_TXT SYS_CHAR("hello_world.txt")

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

sys_string get_filepath(const sys_char *file)
{
    sys_string ret{};

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

sys_string hello_world_txt{};

define_test(async_read_reads_from_file)
{
    error err{};

    io_handle h = io_open(hello_world_txt.data, OPEN_FLAGS_ASYNC);
    assert_not_equal(h, INVALID_IO_HANDLE);
    defer { io_close(h); };

    char data[32] = {0};
    async_task *t;

    t = async_read(h, &data, 12);

    assert_equal(async_submit_tasks(&err), true);
    assert_equal(err.error_code, 0);

    assert_equal(async_await(t, &err), true);
    assert_equal(err.error_code, 0);
    
    assert_equal(async_task_result(t), 12);
    assert_equal(data, "Hello world!"_cs);

    fill_memory(data, 0, 32);

    t = async_read(h, &data, 5, 6);

    assert_equal(async_submit_tasks(&err), true);
    assert_equal(err.error_code, 0);

    assert_equal(async_await(t, &err), true);
    assert_equal(err.error_code, 0);

    assert_equal(async_task_result(t), 5);
    assert_equal(data, "world"_cs);
}

#if 0
// This test starts one long read (2gb) and while the file is being read
// it writes to the standard output.
// NOTE: to test on windows, use Sysinternals RAMMap64.exe to clear mapped files,
// as this test will immediately finish after the first pass otherwise. 
define_test(async_does_things_asynchronously)
{
    error err{};

#if Windows
    io_handle h = io_open(R"(C:/Temp/large_file.txt)", OPEN_FLAGS_ASYNC, OPEN_MODE_READ, &err);
#else
    io_handle h = io_open(R"(/tmp/large_file.txt)", OPEN_FLAGS_ASYNC, OPEN_MODE_READ, &err);
#endif

    if (err.error_code != 0)
        tprint("%\n", err.what);

    assert(h != INVALID_IO_HANDLE);
    defer { io_close(h); };

    s64 sz = 2 * 1024 * 1024 * 1024ul - 1ul;
    char *data = alloc<char>(sz);
    fill_memory(data, 0, sz);
    defer { dealloc(data, sz); };
    async_task *read_task;
    async_task *write_task;
    io_handle out = stdout_handle();
    int i = 0;

    read_task = async_read(h, data, sz);

    while (!async_task_is_done(read_task))
    {
        const_string tmp = tformat("hello %\n", i++);
        write_task = async_write(out, (char*)tmp.c_str, tmp.size);
        async_submit_tasks();
        async_await(write_task);

        // assert((read_task.status & 0x00ff) != (write_task.status & 0x00ff));
    }

    s64 read_result = async_task_result(read_task);

    if (read_result >= 0)
        tprint("bytes read: %\n", read_result);
    else
    {
#if Windows
        tprint("error code %: % \n", -read_result, _windows_error_message(-read_result));
#else
        tprint("error code %: % \n", -read_result, _errno_error_message(-read_result));
#endif
    }
}
#endif

define_test(async_read_scatter_reads_from_file)
{
    error err{};

    int open_flags = OPEN_FLAGS_ASYNC;
#if Windows
    open_flags |= OPEN_FLAGS_DIRECT;
#endif
    io_handle h = io_open(hello_world_txt.data, open_flags, OPEN_MODE_READ);
    assert_not_equal(h, INVALID_IO_HANDLE);
    defer { io_close(h); };

    s64 pagesize = get_system_pagesize();

#if Windows
    char *data1 = (char*)VirtualAlloc(nullptr, pagesize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    static_assert(sizeof(io_buffer) == sizeof(FILE_SEGMENT_ELEMENT));
    defer { VirtualFree(data1, pagesize, MEM_DECOMMIT | MEM_RELEASE); };

    fill_memory(data1, 0, pagesize);

    const s64 buf_count = 1;
    io_buffer bufs[buf_count] = {{data1}};
#else
    char *data1 = alloc<char>(pagesize);
    char *data2 = alloc<char>(pagesize);
    defer { dealloc(data1); dealloc(data2); };

    fill_memory(data1, 0, pagesize);
    fill_memory(data2, 0, pagesize);

    const s64 buf_count = 2;
    io_buffer bufs[buf_count] = {{data1, 5}, {data2, 7}};
#endif

    async_task *t;

    t = async_read_scatter(h, bufs, buf_count);

    assert_equal(async_submit_tasks(&err), true);
    assert_equal(err.error_code, 0);

    assert_equal(async_await(t, &err), true);
    assert_equal(err.error_code, 0);
    
#if Windows
    assert_equal(to_const_string(data1, 12), "Hello world!"_cs);
#else
    assert_equal(data1, "Hello"_cs);
    assert_equal(data2, " world!"_cs);
#endif
}

void _setup()
{
    hello_world_txt = get_filepath(HELLO_WORLD_TXT);
}

void _cleanup()
{
    free(&hello_world_txt);
}

define_test_main(_setup, _cleanup);
