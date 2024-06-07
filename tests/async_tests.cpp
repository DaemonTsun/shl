
#include "t1/t1.hpp"

#include "shl/defer.hpp"
#include "shl/string.hpp"
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
    async_task t{};

    async_read(&t, h, &data, 12);

    assert_equal(async_submit_tasks(&err), true);
    assert_equal(err.error_code, 0);

    assert_equal(async_await(&t, &err), true);
    assert_equal(err.error_code, 0);
    
    assert_equal(data, "Hello world!"_cs);
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
