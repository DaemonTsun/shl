
#include "shl/assert.hpp"
#include "shl/at_exit.hpp"
#include "shl/async_io.hpp"
#include "shl/debug.hpp"

// on windows, should be a multiple of MAXIMUM_WAIT_OBJECTS
#define ASYNC_ENTRIES 128 
#define ASYNC_ENTRIES_MASK (ASYNC_ENTRIES - 1)

#if Windows
#include <windows.h>
#include "shl/memory.hpp"
#define ASYNC_STATUS_READY   0
#define ASYNC_STATUS_SETUP   1
#define ASYNC_STATUS_RUNNING 2
#define ASYNC_STATUS_DONE    3

#define SET_TASK_STATUS(Task, Status)   (Task)->status = ((Status) << 28) | ((Task)->status & 0x0fffffff)
#define GET_TASK_STATUS(Task)           ((Task)->status >> 28)
#define SET_TASK_INDEX(Task, Idx)       ((Task)->status = ((Idx & 0x0fffffff) | ((Task)->status & 0xf0000000)
#define GET_TASK_INDEX(Task)            ((Task)->status & 0x0fffffff)

struct _overlapped
{
    // same structure as win32 OVERLAPPED
    u64 internal;
    u64 internal_high;
    u64 offset;
    void *event;
};

static_assert(sizeof(_overlapped) == sizeof(OVERLAPPED));
static_assert(offsetof(_overlapped, event) == offsetof(OVERLAPPED, hEvent));

enum class _async_op
{
    Read,
    Write,
    ReadScatter,
    WriteScatter,
    // TODO: winsock2 send, receive, etc
};

struct _async_cmd
{
    int status;
    _async_op   op;
    _overlapped overlapped;
    async_task *task;
    io_handle handle;

    union
    {
        struct
        {
            void *buffer;
            s32 bytes_to_read;
        } read_args;

        struct
        {
            void *buffer;
            s32 bytes_to_write;
        } write_args;

        /* TODO: implement
        struct
        {
            buffers,
            sizes,
            bytes
        } read_scatter_args;
        */
    };
};

struct _async_context
{
    _async_cmd commands[ASYNC_ENTRIES];
    HANDLE events[ASYNC_ENTRIES];
    u32 submit_index;
    u32 submit_count;
};

thread_local _async_context _ctx{};

void _cleanup_context()
{
    for (int i = 0; i < ASYNC_ENTRIES; ++i)
        CloseHandle(_ctx.events[i]);

    fill_memory(&_ctx, 0);
}

_async_context *_get_async_context()
{
    thread_local bool _setup = true;

    if (_setup)
    {
        _setup = false;
        fill_memory(&_ctx, 0);

        for (int i = 0; i < ASYNC_ENTRIES; ++i)
        {
            _ctx.events[i] = CreateEventA(nullptr, true, false, nullptr);
            assert(_ctx.events[i] != INVALID_IO_HANDLE);
            _ctx.commands[i].overlapped.event = _ctx.events[i];
        }

        register_exit_function(_cleanup_context);
    }

    return &_ctx;
}

bool _submit_cmd(_async_cmd *cmd, error *err)
{
    if (cmd->status != ASYNC_STATUS_SETUP)
        return false;

    int error_code = 0;

    switch (cmd->op)
    {
    case _async_op::Read:
    {
        if (!ReadFile(cmd->handle,
                      cmd->read_args.buffer,
                      cmd->read_args.bytes_to_read,
                      nullptr,
                      (LPOVERLAPPED)&cmd->overlapped))
        {
            error_code = GetLastError();

            if (error_code != ERROR_IO_PENDING)
            {
                set_error_by_code(err, error_code);
                cmd->task->result = -error_code;
                return false;
            }
        }

        break;
    }
    case _async_op::Write:
    {
        if (!WriteFile(cmd->handle,
                       cmd->write_args.buffer,
                       cmd->write_args.bytes_to_write,
                       nullptr,
                       (LPOVERLAPPED)&cmd->overlapped))
        {
            error_code = GetLastError();

            if (error_code != ERROR_IO_PENDING)
            {
                set_error_by_code(err, error_code);
                cmd->task->result = -error_code;
                return false;
            }
        }

        break;
    }
    // TODO: implement
    case _async_op::ReadScatter:
    case _async_op::WriteScatter:
    default:
        return false;
    }

    cmd->status = ASYNC_STATUS_RUNNING;
    SET_TASK_STATUS(cmd->task, ASYNC_STATUS_RUNNING);

    return true;
}

#elif Linux
#include "shl/impl/linux/io_uring_context.hpp"
thread_local io_uring_context _ctx{};

io_uring_context *_get_async_context()
{
    if (_ctx.fd == 0)
    {
        // TODO: params?
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
#if Windows
    _async_context *ctx = _get_async_context();

    u32 idx = ctx->submit_index & ASYNC_ENTRIES_MASK;
    _async_cmd *cmd = ctx->commands + idx;
    ctx->submit_index += 1;
    ctx->submit_count += 1;

    assert(cmd->status != ASYNC_STATUS_RUNNING);

    cmd->overlapped.offset = 0;
    cmd->task = t;
    cmd->status = ASYNC_STATUS_SETUP;
    SET_TASK_STATUS(t, ASYNC_STATUS_SETUP);
    t->result = 0;

    cmd->op = _async_op::Read;
    cmd->handle = h;
    cmd->read_args.buffer = buf;
    cmd->read_args.bytes_to_read = (DWORD)buf_size;
#elif Linux
    io_uring_cmd_read(_get_async_context(), (io_uring_task*)t, h, buf, buf_size);
#endif
}

void async_write(async_task *t, io_handle h, void *buf, s64 buf_size)
{
#if Windows
    _async_context *ctx = _get_async_context();

    u32 idx = ctx->submit_index & ASYNC_ENTRIES_MASK;
    _async_cmd *cmd = ctx->commands + idx;
    ctx->submit_index += 1;
    ctx->submit_count += 1;

    assert(cmd->status != ASYNC_STATUS_RUNNING);

    cmd->overlapped.offset = 0;
    cmd->task = t;
    cmd->status = ASYNC_STATUS_SETUP;
    SET_TASK_STATUS(t, ASYNC_STATUS_SETUP);
    t->result = 0;

    cmd->op = _async_op::Write;
    cmd->handle = h;
    cmd->read_args.buffer = buf;
    cmd->read_args.bytes_to_read = (DWORD)buf_size;
#elif Linux
    io_uring_cmd_write(_get_async_context(), (io_uring_task*)t, h, buf, buf_size);
#endif
}

bool async_submit_tasks(error *err)
{
#if Windows
    _async_context *ctx = _get_async_context();
    // TODO: overflow handling???
    assert(ctx->submit_count <= ctx->submit_index);

    u32 start_index = ctx->submit_index - ctx->submit_count;
    u32 i = start_index & ASYNC_ENTRIES_MASK;
    bool ok = true;
    
    while (ctx->submit_count > 0 && ok)
    {
        _async_cmd *cmd = ctx->commands + i;

        if (cmd->status == ASYNC_STATUS_SETUP)
        {
            ok = _submit_cmd(cmd, err);

            if (ok)
                cmd->status = ASYNC_STATUS_RUNNING;
            else
            {
                fill_memory(cmd, 0);
                cmd->status = ASYNC_STATUS_READY;
            }
        }

        ctx->submit_count -= 1;
        start_index = start_index + 1;
        i = start_index & ASYNC_ENTRIES_MASK;
    }
    
    return ok;
#elif Linux
    return io_uring_submit_commands(_get_async_context(), 0, err);
#endif
}

void async_process_open_tasks()
{
#if Windows
    _async_context *ctx = _get_async_context();

    bool ok = true;
    u32 res = 0;
    u32 idx = 0;

    // as long as at least one object is signalled, we continue
    while (ok)
    {
        ok = false;

        for (s64 wait_batch = 0; wait_batch < ASYNC_ENTRIES; wait_batch += MAXIMUM_WAIT_OBJECTS)
        {
            res = WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS,
                                         ctx->events + wait_batch,
                                         false,
                                         0);

            ok |= (/*(res >= WAIT_OBJECT_0) &&*/ (res < WAIT_OBJECT_0 + MAXIMUM_WAIT_OBJECTS));

            if (!ok)
                continue;

            idx = res - WAIT_OBJECT_0;

            _async_cmd *cmd = ctx->commands + idx;

            if (cmd->status != ASYNC_STATUS_RUNNING)
                continue;

            
            if (!GetOverlappedResult(cmd->handle,
                                     (LPOVERLAPPED)&cmd->overlapped,
                                     (LPDWORD)&cmd->task->result,
                                     false))
            {
                cmd->status = ASYNC_STATUS_DONE;
                SET_TASK_STATUS(cmd->task, ASYNC_STATUS_DONE);
            }
            else
            {
                int error_code = GetLastError();
                
                if (error_code == ERROR_IO_PENDING)
                    // huh???
                    continue;
                else
                {
                    cmd->task->result = -error_code;
                    fill_memory(cmd, 0);
                    cmd->status = ASYNC_STATUS_READY;
                }
            }
        }
    }
#elif Linux
    io_uring_process_open_completion_queue(_get_async_context());
#endif
}

bool async_task_is_done(async_task *task)
{
#if Windows
    return GET_TASK_STATUS(task) == ASYNC_STATUS_DONE;
#elif Linux
    return io_uring_task_is_done((io_uring_task*)task);
#endif
}

bool async_await(async_task *task, error *err)
{
#if Windows
    if (async_task_is_done(task))
        return true;

    _async_context *ctx = _get_async_context();

    u32 i = GET_TASK_INDEX(task);
    _async_cmd *cmd = ctx->commands + i;

    if (cmd->status != ASYNC_STATUS_RUNNING)
        return false;

    bool ok = GetOverlappedResult(cmd->handle,
                                  (LPOVERLAPPED)&cmd->overlapped,
                                  (LPDWORD)&cmd->task->result,
                                  true);

    if (!ok)
    {
        int error_code = GetLastError();
        set_error_by_code(err, error_code);
        cmd->task->result = -error_code;
        fill_memory(cmd, 0);
        cmd->status = ASYNC_STATUS_READY;
        return false;
    }

    cmd->status = ASYNC_STATUS_DONE;
    SET_TASK_STATUS(cmd->task, ASYNC_STATUS_DONE);

    return true;

#elif Linux
    return io_uring_task_await(_get_async_context(), (io_uring_task*)task, err);
#endif
}
