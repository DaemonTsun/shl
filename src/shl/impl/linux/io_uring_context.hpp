
/* io_uring_conetxt.hpp

A higher level interface to io_uring syscalls.
If you were looking for plain io_uring syscalls, see io_uring.hpp/.cpp.
*/

#pragma once

#include "shl/error.hpp"
#include "shl/memory.hpp"
#include "shl/impl/linux/io_uring.hpp"

struct io_uring_context
{
    int fd;
    u32 features;

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

    s32 to_submit; 
};

bool io_uring_create_context(io_uring_context *ctx, u32 entries, io_uring_params *params, error *err = nullptr);
bool io_uring_destroy_context(io_uring_context *ctx, error *err = nullptr);

// also increases the tail index by one
s32 io_uring_next_sqe_index(io_uring_context *ctx);

#define TASK_STATUS_SETUP   0x01000000
#define TASK_STATUS_DONE    0x02000000
#define TASK_STATUS_MASK    0xff000000
#define TASK_STATUS_UNMASK  0x00ffffff

struct io_uring_task
{
    s32 flags;
    s32 result;
};

static inline bool io_uring_task_is_done(io_uring_task *task)
{
    return (task->flags & TASK_STATUS_MASK) == TASK_STATUS_DONE;
}

void io_uring_cmd_read(io_uring_context *ctx, io_uring_task *task,  int fd, void *buf, s64 buf_size, s64 offset);
void io_uring_cmd_write(io_uring_context *ctx, io_uring_task *task, int fd, void *buf, s64 buf_size, s64 offset);

// submits all unsubmitted commands in the queue.
// if wait_for_entries is 0, does not wait for any commands.
// if wait_for_entries is n, waits for n commands to complete.
// if wait_for_entries is -1, waits for all commands to complete.
bool io_uring_submit_commands(io_uring_context *ctx, s32 wait_for_entries = 0, error *err = nullptr);

void io_uring_process_open_completion_queue(io_uring_context *ctx);
bool io_uring_task_await(io_uring_context *ctx, io_uring_task *task, error *err = nullptr);
