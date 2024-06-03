
/* async_io.hpp

Low-level async functions.
 */

#pragma once

#include "shl/platform.hpp"
#include "shl/io.hpp"
#include "shl/number_types.hpp"

struct async_task
{
#if Windows
    // same structure as OVERLAPPED
    u64 internal;
    u64 offset;
    void *event;
#elif Linux
    s32 flags;
    s32 result;
#endif
};

void async_read(async_task *t,  io_handle h, void *buf, s64 buf_size);
void async_write(async_task *t, io_handle h, void *buf, s64 buf_size);

bool async_submit_tasks(error *err = nullptr);
bool async_task_is_done(async_task *task);
bool async_await(async_task *task, error *err = nullptr);
