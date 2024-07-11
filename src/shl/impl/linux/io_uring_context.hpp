
/* io_uring_conetxt.hpp

A higher level interface to io_uring syscalls.
If you were looking for plain io_uring syscalls, see io_uring.hpp/.cpp.
*/

#pragma once

#include "shl/error.hpp"
#include "shl/memory.hpp"
#include "shl/impl/linux/io_uring.hpp"
#include "shl/impl/async_common.hpp"

struct io_uring_context
{
    int fd;
    u32 features;
    u32 entry_count;

    struct
    {
        void *base;
        s64 size;
        u32 *head;
        u32 *tail;
        u32 *ring_mask;
        u32 *ring_entries;
        u32 *array;
        u32 flags;
    } sq;

    struct
    {
        void *base;
        s64 size;
        u32 *head;
        u32 *tail;
        u32 *ring_mask;
        u32 *ring_entries;
    } cq;

    io_uring_sqe *sqes;
    s64 sqes_size;
    io_uring_cqe *cqes;

    // list of indices of sqes that will be submitted with next call to
    // io_uring_submit_commands. This is used to update state of these
    // sqes to ASYNC_STATUS_RUNNING.
    s16 *submit_queue;
    u32 submit_index;
    u32 submit_count;

    async_task *tasks;
};

bool io_uring_create_context(io_uring_context *ctx, u32 entries, io_uring_params *params, error *err = nullptr);
bool io_uring_destroy_context(io_uring_context *ctx, error *err = nullptr);

// also increases the tail index by one
s32 io_uring_next_sqe_index(io_uring_context *ctx);

static inline bool io_uring_task_is_done(async_task *task)
{
    return task->status == ASYNC_STATUS_DONE;
}

async_task *io_uring_cmd_read(io_uring_context *ctx,  int fd, void *buf, s64 buf_size, s64 offset);
async_task *io_uring_cmd_write(io_uring_context *ctx, int fd, void *buf, s64 buf_size, s64 offset);

struct io_vec
{
    void *data;
    s64   size;
};

async_task *io_uring_cmd_readv(io_uring_context *ctx,  int fd, io_vec *buffers, s64 buffer_count, s64 offset);
async_task *io_uring_cmd_writev(io_uring_context *ctx, int fd, io_vec *buffers, s64 buffer_count, s64 offset);

struct io_uring_timespec
{
    s64 seconds;
    s64 nanoseconds;
};

// cq_count = 0 -> timeout
// otherwise, wait for cq_count cqes to be done
async_task *io_uring_timeout(io_uring_context *ctx, io_uring_timespec *ts, s64 cq_count);

// submits all unsubmitted commands in the queue.
// if wait_for_entries is 0, does not wait for any commands.
// if wait_for_entries is n, waits for n commands to complete.
// if wait_for_entries is -1, waits for all commands to complete.
bool io_uring_submit_commands(io_uring_context *ctx, s32 wait_for_entries = 0, error *err = nullptr);

void io_uring_process_open_completion_queue(io_uring_context *ctx);
bool io_uring_task_await(io_uring_context *ctx, async_task *task, error *err = nullptr);
