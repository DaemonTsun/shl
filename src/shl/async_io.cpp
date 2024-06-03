
#include "shl/at_exit.hpp"
#include "shl/async_io.hpp"
#include "shl/debug.hpp"

#if Linux
#include "shl/impl/linux/io_uring_context.hpp"

#define IO_URING_ENTRIES 256 
thread_local io_uring_context _ctx{};

io_uring_context *_get_async_context()
{
    if (_ctx.fd == 0)
    {
        // TODO: params?
        io_uring_params params{};
        error err{};
        if (!io_uring_create_context(&_ctx, IO_URING_ENTRIES, &params, &err))
        {
            _ctx.fd = 0;
            breakpoint();
            return nullptr;
        }
    }

    return &_ctx;
}
#endif

void async_read(async_task *t, io_handle h, void *buf, s64 buf_size)
{
#if Linux
    io_uring_cmd_read(_get_async_context(), (io_uring_task*)t, h, buf, buf_size);
#endif
}

void async_write(async_task *t, io_handle h, void *buf, s64 buf_size)
{
#if Linux
    io_uring_cmd_write(_get_async_context(), (io_uring_task*)t, h, buf, buf_size);
#endif
}

bool async_submit_tasks(error *err)
{
#if Linux
    return io_uring_submit_commands(_get_async_context(), 0, err);
#endif
}

bool async_task_is_done(async_task *task)
{
#if Linux
    return io_uring_task_is_done((io_uring_task*)task);
#endif
}

bool async_await(async_task *task, error *err)
{
#if Linux
    return io_uring_task_await(_get_async_context(), (io_uring_task*)task, err);
#endif
}
