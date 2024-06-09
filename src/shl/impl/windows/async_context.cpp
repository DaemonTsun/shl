
#include "shl/assert.hpp"
#include "shl/memory.hpp"
#include "shl/impl/windows/async_context.hpp"

static_assert(max_value(u16) >= ASYNC_ENTRIES);
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
        ::CloseHandle(ctx->events[i]);

    fill_memory(ctx, 0);
}

static void _reset_cmd(async_command *cmd)
{
    void *event = cmd->overlapped.event;
    fill_memory(cmd, 0);
    cmd->overlapped.event = event;
    cmd->status = ASYNC_STATUS_READY;
    ::ResetEvent(event);
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
    case async_op::ReadScatter:
    {
        s64 pagesize = get_system_pagesize();

        if (!ReadFileScatter(cmd->handle,
                             (FILE_SEGMENT_ELEMENT*)cmd->read_scatter_args.buffers,
                             (DWORD)(cmd->read_scatter_args.buffer_count * pagesize),
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
    case async_op::WriteScatter:
    {
        s64 pagesize = get_system_pagesize();

        if (!WriteFileGather(cmd->handle,
                             (FILE_SEGMENT_ELEMENT*)cmd->read_scatter_args.buffers,
                             (DWORD)(cmd->read_scatter_args.buffer_count * pagesize),
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
    /* TODO: socket
    default:
        return false;
    */
    }

    cmd->status = ASYNC_STATUS_RUNNING;
    SET_TASK_STATUS(cmd->task, ASYNC_STATUS_RUNNING);

    return true;
}

static u32 _next_command_index(async_context *ctx)
{
    assert(ctx->submit_count < ASYNC_ENTRIES);
    
    u32 cur = ctx->submit_index & ASYNC_ENTRIES_MASK;
    u32 failsafe_counter = 0;
    async_command *cmd = ctx->commands + cur;

    while (cmd->status != ASYNC_STATUS_READY && failsafe_counter < ASYNC_ENTRIES)
    {
        ctx->submit_index += 1;
        cur = ctx->submit_index & ASYNC_ENTRIES_MASK;
        cmd = ctx->commands + cur;
        failsafe_counter += 1;
    }

    // should never fail
    assert(failsafe_counter < ASYNC_ENTRIES);
    assert(cmd->status == ASYNC_STATUS_READY);

    ctx->submit_queue[ctx->submit_count] = (u16)(cur & 0x0000ffff);
    ctx->submit_index += 1;
    ctx->submit_count += 1;

    return cur;
}

#define _async_setup_task(Ctx, Task, OutIdx, OutCmd, OpCode)\
    assert((Ctx) != nullptr);\
    assert((Task) != nullptr);\
    u32 OutIdx = _next_command_index(Ctx);\
    async_command *OutCmd = (Ctx)->commands + OutIdx;\
    (OutCmd)->task = (Task);\
    (OutCmd)->status = ASYNC_STATUS_SETUP;\
    SET_TASK_STATUS((Task), ASYNC_STATUS_SETUP);\
    SET_TASK_INDEX((Task), OutIdx);\
    (Task)->result = 0;\
    (OutCmd)->op = OpCode;

void async_cmd_read(async_context *ctx, async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset)
{
    _async_setup_task(ctx, t, idx, cmd, async_op::Read);

    cmd->handle = h;
    cmd->read_args.buffer = buf;
    cmd->read_args.bytes_to_read = (DWORD)buf_size;
    cmd->overlapped.offset = offset;
}

void async_cmd_write(async_context *ctx, async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset)
{
    _async_setup_task(ctx, t, idx, cmd, async_op::Write);

    cmd->handle = h;
    cmd->write_args.buffer = buf;
    cmd->write_args.bytes_to_write = (DWORD)buf_size;
    cmd->overlapped.offset = offset;
}

void async_cmd_read_scatter(async_context *ctx, async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset)
{
    _async_setup_task(ctx, t, idx, cmd, async_op::ReadScatter);

    cmd->handle = h;
    cmd->read_scatter_args.buffers = buffers;
    cmd->read_scatter_args.buffer_count = (DWORD)buffer_count;
    cmd->overlapped.offset = offset;
}

void async_cmd_write_gather(async_context *ctx, async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset)
{
    _async_setup_task(ctx, t, idx, cmd, async_op::WriteGather);

    cmd->handle = h;
    cmd->write_gather_args.buffers = buffers;
    cmd->write_gather_args.buffer_count = (DWORD)buffer_count;
    cmd->overlapped.offset = offset;
}

bool async_submit_commands(async_context *ctx, error *err)
{
    u16 idx = 0;
    u32 i = 0;
    bool ok = true;
    
    while (ctx->submit_count > 0 && ok)
    {
        idx = ctx->submit_queue[i];
        async_command *cmd = ctx->commands + idx;

        if (cmd->status == ASYNC_STATUS_SETUP)
        {
            ok = _submit_cmd(cmd, err);

            if (ok)
                cmd->status = ASYNC_STATUS_RUNNING;
            else
                _reset_cmd(cmd);
        }

        ctx->submit_count -= 1;
        i += 1;
    }
    
    return ok;
}

// if out_index is not nullptr, returns on the first completed task and
// sets out_index to the index of the command that completed / failed.
static bool _process_open_commands(async_context *ctx, u32 *out_index, bool wait)
{
    bool ok = true;
    bool any_processed = false;
    u32 res = 0;
    u32 idx = 0;

    // as long as at least one object is signalled, we continue
    while (ok)
    {
        ok = false;

        for (s64 wait_batch = 0; wait_batch < ASYNC_ENTRIES; wait_batch += MAXIMUM_WAIT_OBJECTS)
        {
            res = WaitForMultipleObjectsEx(MAXIMUM_WAIT_OBJECTS,
                                           ctx->events + wait_batch,
                                           false,
                                           wait ? INFINITE : 0,
                                           true);

            ok = res < WAIT_OBJECT_0 + MAXIMUM_WAIT_OBJECTS;

            if (res == WAIT_IO_COMPLETION) // APC
            {
                ok = true;
                continue;
            }

            if (!ok)
                continue;

            any_processed = true;
            idx = res - WAIT_OBJECT_0;
            async_command *cmd = ctx->commands + idx;

            assert(cmd->status == ASYNC_STATUS_RUNNING);

            if (GetOverlappedResult(cmd->handle,
                                    (LPOVERLAPPED)&cmd->overlapped,
                                    (LPDWORD)&cmd->task->result,
                                    false))
            {
                cmd->status = ASYNC_STATUS_DONE;
                SET_TASK_STATUS(cmd->task, ASYNC_STATUS_DONE);
                ::ResetEvent(cmd->overlapped.event);
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
                    SET_TASK_STATUS(cmd->task, ASYNC_STATUS_DONE);
                    _reset_cmd(cmd);
                }
            }

            if (out_index != nullptr)
            {
                *out_index = idx;
                return true;
            }
        }
    }

    return any_processed;
}

void async_process_open_commands(async_context *ctx)
{
    _process_open_commands(ctx, nullptr, false);
}

bool async_await_task(async_context *ctx, async_task *task, error *err)
{
    if (async_task_is_done(task))
        return true;

    u32 i = GET_TASK_INDEX(task);
    async_command *cmd = ctx->commands + i;

    if (cmd->status != ASYNC_STATUS_RUNNING)
        return false;

    u32 completed_idx = ASYNC_ENTRIES;

    while (_process_open_commands(ctx, &completed_idx, true) && completed_idx != i)
        continue;

    if (completed_idx == i)
    {
        _reset_cmd(cmd);
        return true;
    }

    return false;
}
