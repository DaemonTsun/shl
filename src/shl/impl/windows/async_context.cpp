
#include "shl/assert.hpp"
#include "shl/memory.hpp"
#include "shl/impl/windows/async_context.hpp"

static_assert(sizeof(async_overlapped) == sizeof(OVERLAPPED));
static_assert(offset_of(async_overlapped, event) == offset_of(OVERLAPPED, hEvent));

bool init(async_context *ctx, error *err)
{
    assert(ctx != nullptr);

    fill_memory(ctx, 0);

    for (int i = 0; i < ASYNC_ENTRIES; ++i)
    {
        HANDLE event = ::CreateEventA(nullptr, true, false, nullptr);

        if (event == INVALID_IO_HANDLE)
        {
            set_GetLastError_error(err);

            // unroll
            for (int j = 0; j < i; ++j)
                ::CloseHandle(ctx->events[j]);

            return false;
        }

        ctx->events[i] = event;
        ctx->commands[i].overlapped.event = event;
    }

    return true;
}

void free(async_context *ctx)
{
    if (ctx == nullptr)
        return;

    for (int i = 0; i < ASYNC_ENTRIES; ++i)
        CloseHandle(ctx->events[i]);

    fill_memory(ctx, 0);
}

static bool _submit_cmd(async_command *cmd, error *err)
{
    if (cmd->status != ASYNC_STATUS_SETUP)
        return false;

    int error_code = 0;

    switch (cmd->op)
    {
    case async_op::Read:
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
    case async_op::Write:
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
    case async_op::ReadScatter:
    case async_op::WriteScatter:
    default:
        return false;
    }

    cmd->status = ASYNC_STATUS_RUNNING;
    SET_TASK_STATUS(cmd->task, ASYNC_STATUS_RUNNING);

    return true;
}

#define _async_setup_task(Ctx, Task, OutIdx, OutCmd, OpCode)\
    assert((Ctx) != nullptr);\
    assert((Task) != nullptr);\
    u32 OutIdx = (Ctx)->submit_index & ASYNC_ENTRIES_MASK;\
    async_command *OutCmd = (Ctx)->commands + OutIdx;\
    (Ctx)->submit_index += 1;\
    (Ctx)->submit_count += 1;\
    assert(cmd->status != ASYNC_STATUS_RUNNING);\
    (OutCmd)->overlapped.offset = 0;\
    (OutCmd)->task = (Task);\
    (OutCmd)->status = ASYNC_STATUS_SETUP;\
    SET_TASK_STATUS((Task), ASYNC_STATUS_SETUP);\
    (Task)->result = 0;\
    (OutCmd)->op = OpCode;

void async_cmd_read(async_context *ctx, async_task *t, io_handle h, void *buf, s64 buf_size)
{
    _async_setup_task(ctx, t, idx, cmd, async_op::Read);

    cmd->handle = h;
    cmd->read_args.buffer = buf;
    cmd->read_args.bytes_to_read = (DWORD)buf_size;
}

void async_cmd_write(async_context *ctx, async_task *t, io_handle h, void *buf, s64 buf_size)
{
    _async_setup_task(ctx, t, idx, cmd, async_op::Write);

    cmd->handle = h;
    cmd->write_args.buffer = buf;
    cmd->write_args.bytes_to_write = (DWORD)buf_size;
}

bool async_submit_commands(async_context *ctx, error *err)
{
    // TODO: overflow handling???
    assert(ctx->submit_count <= ctx->submit_index);

    u32 start_index = ctx->submit_index - ctx->submit_count;
    u32 i = start_index & ASYNC_ENTRIES_MASK;
    bool ok = true;
    
    while (ctx->submit_count > 0 && ok)
    {
        async_command *cmd = ctx->commands + i;

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
}

void async_process_open_commands(async_context *ctx)
{
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

            async_command *cmd = ctx->commands + idx;

            if (cmd->status != ASYNC_STATUS_RUNNING)
                continue;

            if (GetOverlappedResult(cmd->handle,
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
                    cmd->status = ASYNC_STATUS_READY;
                }
            }
        }
    }
}

bool async_await_task(async_context *ctx, async_task *task, error *err)
{
    if (async_task_is_done(task))
        return true;

    u32 i = GET_TASK_INDEX(task);
    async_command *cmd = ctx->commands + i;

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
        cmd->status = ASYNC_STATUS_READY;
        return false;
    }

    cmd->status = ASYNC_STATUS_DONE;
    SET_TASK_STATUS(cmd->task, ASYNC_STATUS_DONE);

    return true;
}
