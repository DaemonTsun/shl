
#include "shl/assert.hpp"
#include "shl/impl/linux/io.hpp"
#include "shl/impl/linux/memory.hpp"
#include "shl/impl/linux/io_uring_context.hpp"

bool io_uring_create_context(io_uring_context *ctx, u32 entries, io_uring_params *params, error *err)
{
    assert(ctx != nullptr);

    ctx->fd = io_uring_setup(entries, params);

    if (ctx->fd < 0)
    {
        int code = -ctx->fd;
        set_error_by_code(err, -code);
        return false;
    }

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

    return true;
}

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

#define _io_uring_setup_task(Ctx, Task, OutIdx, OutSqe, OpCode)\
    assert((Ctx) != nullptr);\
    assert((Task) != nullptr);\
    (Task)->result = 0;\
    u32 OutIdx = io_uring_next_sqe_index(Ctx);\
    io_uring_sqe *OutSqe = (Ctx)->sqes + OutIdx;\
    (Ctx)->sq.array[OutIdx] = OutIdx;\
    (Task)->flags = ((Task)->flags & TASK_STATUS_UNMASK) | TASK_STATUS_SETUP;\
    (Ctx)->to_submit += 1;\
    fill_memory(OutSqe, 0);\
    OutSqe->opcode = OpCode;\
    OutSqe->flags = (u8)((Task)->flags & 0x000000ff);\
    OutSqe->userdata = (void*)(Task);

void io_uring_cmd_read(io_uring_context *ctx, io_uring_task *task, int fd, void *buf, s64 buf_size)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_READ);

    sqe->fd = fd;
    sqe->data = buf;
    sqe->data_size = (u32)(buf_size);
}

void io_uring_cmd_write(io_uring_context *ctx, io_uring_task *task, int fd, void *buf, s64 buf_size)
{
    _io_uring_setup_task(ctx, task, idx, sqe, IORING_OP_WRITE);

    sqe->fd = fd;
    sqe->data = buf;
    sqe->data_size = (u32)(buf_size);
}

bool io_uring_submit_commands(io_uring_context *ctx, s32 wait_for_entries, error *err)
{
    if (ctx->to_submit == 0)
        return true;

    if (wait_for_entries == -1)
        wait_for_entries = ctx->to_submit;

    sys_int code = io_uring_enter(ctx->fd, ctx->to_submit, wait_for_entries, wait_for_entries > 0 ? IORING_ENTER_GETEVENTS : 0, nullptr, 0);

    if (code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

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
        io_uring_task *task = (io_uring_task*)cqe->userdata;
        task->result = cqe->result;
        task->flags = (task->flags & TASK_STATUS_UNMASK) | TASK_STATUS_DONE;

        head += 1;
    }

    *cring_head = head;
}

bool io_uring_task_await(io_uring_context *ctx, io_uring_task *task, error *err)
{
    assert(ctx != nullptr);
    assert(task != nullptr);

    if ((task->flags & TASK_STATUS_MASK) == TASK_STATUS_DONE)
        return true;

    if ((task->flags & TASK_STATUS_MASK) != TASK_STATUS_SETUP)
        return false;
    
    sys_int code = 0;

    do
    {
        io_uring_process_open_completion_queue(ctx);

        if (io_uring_task_is_done(task))
            break;

        code = io_uring_enter(ctx->fd, 0, 1, IORING_ENTER_GETEVENTS, nullptr, 0);
    }
    while (code >= 0);

    if (code < 0)
    {
        set_error_by_code(err, -code);
        return false;
    }

    return (task->flags & TASK_STATUS_MASK) == TASK_STATUS_DONE;
}
