
#include "shl/assert.hpp"
#include "shl/at_exit.hpp"
#include "shl/async_io.hpp"
#include "shl/debug.hpp"

#if Windows
#include "shl/impl/windows/async_context.hpp"

thread_local async_context _ctx{};

void _cleanup_context()
{
    free(&_ctx);
}

async_context *_get_async_context()
{
    thread_local bool _setup = true;

    if (_setup)
    {
        _setup = false;

        error err{};
        init(&_ctx, &err);
        assert(err.error_code == 0);
        register_exit_function(_cleanup_context);
    }

    return &_ctx;
}

#elif Linux
#define ASYNC_ENTRIES 128 
#include "shl/impl/linux/io_uring_context.hpp"

thread_local io_uring_context _ctx{};

io_uring_context *_get_async_context()
{
    if (_ctx.fd == 0)
    {
        // TODO: io_uring params?
        io_uring_params params{};
        error err{};
        if (!io_uring_create_context(&_ctx, ASYNC_ENTRIES, &params, &err))
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
    async_read(t, h, buf, buf_size, 0);
}

void async_read(async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset)
{
#if Windows
    async_cmd_read(_get_async_context(), t, h, buf, buf_size, offset);
#elif Linux
    io_uring_cmd_read(_get_async_context(), (io_uring_task*)t, h, buf, buf_size, offset);
#endif
}

void async_write(async_task *t, io_handle h, void *buf, s64 buf_size)
{
    async_write(t, h, buf, buf_size, 0);
}

void async_write(async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset)
{
#if Windows
    async_cmd_write(_get_async_context(), t, h, buf, buf_size, offset);
#elif Linux
    io_uring_cmd_write(_get_async_context(), (io_uring_task*)t, h, buf, buf_size, offset);
#endif
}

void async_read_scatter(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count)
{
    async_read_scatter(t, h, buffers, buffer_count, 0);
}

void async_read_scatter(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset)
{
#if Windows
    async_cmd_read_scatter(_get_async_context(), t, h, buffers, buffer_count, offset);
#elif Linux
    io_uring_cmd_readv(_get_async_context(), (io_uring_task*)t, h, (io_vec*)buffers, buffer_count, offset);
#endif
}

void async_write_gather(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count)
{
    async_write_gather(t, h, buffers, buffer_count, 0);
}

void async_write_gather(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset)
{
#if Windows
    async_cmd_write_scatter(_get_async_context(), t, h, buffers, buffer_count, offset);
#elif Linux
    io_uring_cmd_writev(_get_async_context(), (io_uring_task*)t, h, (io_vec*)buffers, buffer_count, offset);
#endif
}

bool async_submit_tasks(error *err)
{
#if Windows
    return async_submit_commands(_get_async_context(), err);
#elif Linux
    return io_uring_submit_commands(_get_async_context(), 0, err);
#endif
}

void async_process_open_tasks()
{
#if Windows
    async_process_open_commands(_get_async_context());
#elif Linux
    io_uring_process_open_completion_queue(_get_async_context());
#endif
}

bool async_task_is_done(async_task *task)
{
#if Windows
    int status = GET_TASK_STATUS(task);
    return status == ASYNC_STATUS_DONE || status == ASYNC_STATUS_READY;
#elif Linux
    return io_uring_task_is_done((io_uring_task*)task);
#endif
}

bool async_await(async_task *task, error *err)
{
#if Windows
    return async_await_task(_get_async_context(), task, err);
#elif Linux
    return io_uring_task_await(_get_async_context(), (io_uring_task*)task, err);
#endif
}
