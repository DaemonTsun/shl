
//#include "shl/print.hpp"
#define tprint(...)
#include "shl/assert.hpp"
#include "shl/impl/linux/io.hpp"
#include "shl/impl/linux/memory.hpp"
#include "shl/impl/linux/io_uring_context.hpp"

bool io_uring_create_context(io_uring_context *ctx, u32 entries, io_uring_params *params, error *err)
{
    assert(ctx != nullptr);

    fill_memory(ctx, 0);

    ctx->fd = io_uring_setup(entries, params);

    if (ctx->fd < 0)
    {
        int code = -ctx->fd;
        set_error_by_code(err, -code);
        return false;
    }

    ctx->entry_count = entries;
    ctx->features = params->features;

    ctx->sq.size = params->sq_off.array + params->sq_entries * sizeof(u32);
    ctx->cq.size = params->cq_off.cqes  + params->cq_entries * sizeof(io_uring_cqe);

    if (ctx->features & IORING_FEAT_SINGLE_MMAP)
    {
        if (ctx->cq.size > ctx->sq.size)
            ctx->sq.size = ctx->cq.size;

        ctx->cq.size = ctx->sq.size;
    }

    ctx->sq.base = mmap(nullptr, ctx->sq.size,
        PROT_READ | PROT_WRITE, 
        MAP_SHARED | MAP_POPULATE,
        ctx->fd,
        IORING_OFF_SQ_RING);

    if (MMAP_IS_ERROR(ctx->sq.base))
    {
        int code = -(int)(sys_int)ctx->sq.base;
        set_error_by_code(err, -code);
        return false;
    }

    if (ctx->features & IORING_FEAT_SINGLE_MMAP)
        ctx->cq.base = ctx->sq.base;
    else
    {
        // need to map completion queue seperately
        ctx->cq.base = mmap(nullptr, ctx->cq.size,
            PROT_READ | PROT_WRITE, 
            MAP_SHARED | MAP_POPULATE,
            ctx->fd,
            IORING_OFF_CQ_RING);

        if (MMAP_IS_ERROR(ctx->cq.base))
        {
            int code = -(int)(sys_int)ctx->sq.base;
            set_error_by_code(err, -code);
            return false;
        }
    }

    ctx->sqes_size = params->sq_entries * sizeof(io_uring_sqe);
    ctx->sqes = (io_uring_sqe*)mmap(nullptr, ctx->sqes_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_POPULATE,
        ctx->fd,
        IORING_OFF_SQES);

    if (MMAP_IS_ERROR(ctx->sqes))
    {
        int code = -(int)(sys_int)ctx->sq.base;
        set_error_by_code(err, -code);
        return false;
    }

    ctx->cqes = (io_uring_cqe*)((char*)ctx->cq.base + params->cq_off.cqes);

    ctx->sq.head = (u32*)((char*)ctx->sq.base + params->sq_off.head);
    ctx->sq.tail = (u32*)((char*)ctx->sq.base + params->sq_off.tail);
    ctx->sq.ring_mask = (u32*)((char*)ctx->sq.base + params->sq_off.ring_mask);
    ctx->sq.ring_entries = (u32*)((char*)ctx->sq.base + params->sq_off.ring_entries);
    ctx->sq.array = (u32*)((char*)ctx->sq.base + params->sq_off.array);
    ctx->sq.flags = params->sq_off.flags;

    ctx->cq.head = (u32*)((char*)ctx->cq.base + params->cq_off.head);
    ctx->cq.tail = (u32*)((char*)ctx->cq.base + params->cq_off.tail);
    ctx->cq.ring_mask = (u32*)((char*)ctx->cq.base + params->cq_off.ring_mask);
    ctx->cq.ring_entries = (u32*)((char*)ctx->cq.base + params->cq_off.ring_entries);

    ctx->tasks = alloc<async_task>(ctx->entry_count);
    fill_memory(ctx->tasks, 0, ctx->entry_count * sizeof(async_task));
    ctx->submit_queue = alloc<s16>(ctx->entry_count);

    return true;
}

bool io_uring_destroy_context(io_uring_context *ctx, error *err)
{
    if (ctx == nullptr || ctx->fd < 0)
        return true;

    sys_int code = 0;

    if (ctx->cq.base != nullptr && ctx->cq.base != ctx->sq.base)
    if (code = munmap(ctx->cq.base, ctx->cq.size); code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    if (ctx->sq.base != nullptr)
    if (code = munmap(ctx->sq.base, ctx->sq.size); code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    if (ctx->sqes != nullptr)
    if (code = munmap(ctx->sqes, ctx->sqes_size); code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    ctx->sqes = nullptr;
    ctx->sqes_size = 0;
    ctx->sq.base = nullptr;
    ctx->sq.size = 0;
    ctx->cq.base = nullptr;
    ctx->cq.size = 0;

    if (code = close(ctx->fd); code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    ctx->fd = -1;
    ctx->features = 0;
    dealloc(ctx->tasks, ctx->entry_count);
    dealloc(ctx->submit_queue, ctx->entry_count);
    ctx->tasks = nullptr;

    return true;
}

/*
s32 io_uring_next_sqe_index(io_uring_context *ctx)
{
    assert(ctx != nullptr);

    u32 *sring_tail = ctx->sq.tail;
    u32 *sring_mask = ctx->sq.ring_mask;
    u32 tail = *sring_tail;
    u32 idx = tail & *sring_mask;

    if (*sring_tail != (tail + 1))
        *sring_tail = tail + 1;

    return idx;
}
*/
s32 io_uring_next_free_sqe_index(io_uring_context *ctx)
{
    assert(ctx != nullptr);

    u32 *sring_tail = ctx->sq.tail;
    u32 mask = *ctx->sq.ring_mask;
    u32 tail = *sring_tail;
    u32 idx = tail & mask;
    u32 failsafe_counter = 0;

    while (ctx->tasks[idx].status != ASYNC_STATUS_READY && failsafe_counter < ctx->entry_count)
    {
        tail += 1;
        idx = tail & mask;

        failsafe_counter += 1;
    }

    if (failsafe_counter >= ctx->entry_count)
        return -1;

    *sring_tail = tail + 1;

    return idx;
}

#define _io_uring_setup_task(Ctx, OutTask, OutIdx, OutSqe, OpCode)\
    assert((Ctx) != nullptr);\
    s32 OutIdx = io_uring_next_free_sqe_index(Ctx);\
    if (OutIdx < 0) return nullptr;\
    io_uring_sqe *OutSqe = (Ctx)->sqes + OutIdx;\
    async_task *OutTask = (Ctx)->tasks + OutIdx;\
    (Ctx)->sq.array[OutIdx] = OutIdx;\
    (Ctx)->submit_queue[(Ctx)->submit_count] = OutIdx;\
    (Ctx)->submit_count += 1;\
    fill_memory(OutSqe, 0);\
    OutSqe->opcode = OpCode;\
    OutSqe->userdata = (void*)(OutTask);\
    fill_memory(OutTask, 0);\
    (OutTask)->status = ASYNC_STATUS_SETUP;\
    (OutTask)->index = OutIdx;\
    tprint("new task % %: %\n", OutIdx, OutTask, OpCode);

async_task *io_uring_cmd_read(io_uring_context *ctx, int fd, void *buf, s64 buf_size, s64 offset)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_READ);

    sqe->fd = fd;
    sqe->data = buf;
    sqe->data_size = (u32)(buf_size);
    sqe->offset = offset;

    return task;
}

async_task *io_uring_cmd_write(io_uring_context *ctx, int fd, void *buf, s64 buf_size, s64 offset)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_WRITE);

    sqe->fd = fd;
    sqe->data = buf;
    sqe->data_size = (u32)(buf_size);
    sqe->offset = offset;

    return task;
}

async_task *io_uring_cmd_readv(io_uring_context *ctx, int fd, io_vec *buffers, s64 buffer_count, s64 offset)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_READV);

    sqe->fd = fd;
    sqe->data = buffers;
    sqe->data_size = (u32)(buffer_count);
    sqe->offset = offset;

    return task;
}

async_task *io_uring_cmd_writev(io_uring_context *ctx, int fd, io_vec *buffers, s64 buffer_count, s64 offset)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_WRITEV);

    sqe->fd = fd;
    sqe->data = buffers;
    sqe->data_size = (u32)(buffer_count);
    sqe->offset = offset;

    return task;
}

async_task *io_uring_timeout(io_uring_context *ctx, io_uring_timespec *ts, s64 cq_count)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_TIMEOUT);

    tprint("sleeping for % seconds, % nanoseconds\n", ts->seconds, ts->nanoseconds);
    sqe->fd = -1;
    sqe->data = ts;
    sqe->data_size = 1; // must be 1
    sqe->offset = cq_count;

    return task;
}

bool io_uring_submit_commands(io_uring_context *ctx, s32 wait_for_entries, error *err)
{
    if (ctx->submit_count == 0)
        return true;

    if (wait_for_entries == -1)
        wait_for_entries = ctx->submit_count;

    sys_int code = io_uring_enter(ctx->fd,
                                  ctx->submit_count,
                                  wait_for_entries,
                                  wait_for_entries > 0 ? IORING_ENTER_GETEVENTS : 0,
                                  nullptr,
                                  0);

    if (code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    for (s32 i = 0; i < (s32)ctx->submit_count; ++i)
        ctx->tasks[ctx->submit_queue[i]].status = ASYNC_STATUS_RUNNING;

    ctx->submit_count = 0;

    return true;
}

void io_uring_process_open_completion_queue(io_uring_context *ctx)
{
    u32 *cring_head = ctx->cq.head;
    u32 *cring_tail = ctx->cq.tail;
    u32 *cring_mask = ctx->cq.ring_mask;
    io_uring_cqe *cqe = nullptr;
    u32 head = *cring_head;

    while (head != *cring_tail)
    {
        cqe = ctx->cqes + (head & *cring_mask);
        async_task *task = (async_task*)cqe->userdata;
        task->result = cqe->result;
        task->status = ASYNC_STATUS_DONE;

        tprint("task % (%) is done\n", (int)task->index, task);

        head += 1;
    }

    *cring_head = head;
}

bool io_uring_task_await(io_uring_context *ctx, async_task *task, s64 *result, error *err)
{
    assert(ctx != nullptr);
    assert(task != nullptr);

    if (task->status == ASYNC_STATUS_DONE)
    {
        if (result != nullptr)
            *result = task->result;

        task->status = ASYNC_STATUS_READY;
        return true;
    }

    if (task->status != ASYNC_STATUS_RUNNING)
        return false;
    
    sys_int code = 0;

    do
    {
        io_uring_process_open_completion_queue(ctx);

        if (task->status == ASYNC_STATUS_DONE)
            break;

        code = io_uring_enter(ctx->fd, 0, 1, IORING_ENTER_GETEVENTS, nullptr, 0);
    }
    while (code >= 0);

    if (code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    if (task->status != ASYNC_STATUS_DONE)
        return false;

    if (result != nullptr)
        *result = task->result;

    task->status = ASYNC_STATUS_READY;
    return true;
}
