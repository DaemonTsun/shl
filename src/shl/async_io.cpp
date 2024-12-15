
#include "shl/assert.hpp"
#include "shl/exit.hpp"
#include "shl/async_io.hpp"
#include "shl/debug.hpp"

#if Windows
#include "shl/impl/windows/async_context.hpp"

thread_local async_context _ctx{};

static void _cleanup_context()
{
    free(&_ctx);
}

static async_context *_get_async_context()
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
#include "shl/impl/linux/io_uring_context.hpp"

thread_local io_uring_context _ctx{};

static void _cleanup_context()
{
    io_uring_destroy_context(&_ctx);
}

static io_uring_context *_get_async_context()
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
        register_exit_function(_cleanup_context);
    }

    return &_ctx;
}
#endif

async_task *async_read(io_handle h, void *buf, s64 buf_size)
{
    return async_read(h, buf, buf_size, 0);
}

async_task *async_read(io_handle h, void *buf, s64 buf_size, s64 offset)
{
#if Windows
    return async_cmd_read(_get_async_context(), h, buf, buf_size, offset);
#elif Linux
    return io_uring_cmd_read(_get_async_context(), h, buf, buf_size, offset);
#endif
}

async_task *async_write(io_handle h, void *buf, s64 buf_size)
{
    return async_write(h, buf, buf_size, 0);
}

async_task *async_write(io_handle h, void *buf, s64 buf_size, s64 offset)
{
#if Windows
    return async_cmd_write(_get_async_context(), h, buf, buf_size, offset);
#elif Linux
    return io_uring_cmd_write(_get_async_context(), h, buf, buf_size, offset);
#endif
}

async_task *async_read_scatter(io_handle h, io_buffer *buffers, s64 buffer_count)
{
    return async_read_scatter(h, buffers, buffer_count, 0);
}

async_task *async_read_scatter(io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset)
{
#if Windows
    return async_cmd_read_scatter(_get_async_context(), h, buffers, buffer_count, offset);
#elif Linux
    return io_uring_cmd_readv(_get_async_context(), h, (io_vec*)buffers, buffer_count, offset);
#endif
}

async_task *async_write_gather(io_handle h, io_buffer *buffers, s64 buffer_count)
{
    return async_write_gather(h, buffers, buffer_count, 0);
}

async_task *async_write_gather(io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset)
{
#if Windows
    return async_cmd_write_gather(_get_async_context(), h, buffers, buffer_count, offset);
#elif Linux
    return io_uring_cmd_writev(_get_async_context(), h, (io_vec*)buffers, buffer_count, offset);
#endif
}

async_task *async_sleep(s64 seconds, s64 nanoseconds)
{
#if Windows
    (void)seconds;
    (void)nanoseconds;
    // TODO: implement
    return nullptr;
#elif Linux
    io_uring_timespec ts{seconds, nanoseconds};
    return io_uring_timeout(_get_async_context(), &ts, 0);
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

int async_task_status(async_task *task)
{
    return task->status;
}

bool async_await(async_task *task, s64 *out_result, error *err)
{
#if Windows
    return async_await_task(_get_async_context(), task, out_result, err);
#elif Linux
    return io_uring_task_await(_get_async_context(), task, out_result, err);
#endif
}
