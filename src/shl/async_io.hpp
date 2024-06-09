
/* async_io.hpp

Low-level async functions.

TODO: docs

TODO: change design so async_tasks are managed by shl (since there can only be
ASYNC_ENTRIES number of tasks anyway, and the user should store results
independent of tasks).
To do this, probably make async_task (public) an u16 index into the
managed tasks, make tasks reusable only if they were waited for with
async_await.
Why? So the library can internally handle different kinds of tasks, including
aggregate tasks, without the user having to see all sub-tasks.
This allows for arbitrary async extensions, e.g. an awaitable filesystem copy
command that copies all entries asynchronously; as well as arbitrary linking
of tasks.
*/

#pragma once

#include "shl/platform.hpp"
#include "shl/io.hpp"
#include "shl/number_types.hpp"

struct async_task
{
#if Windows
    u32 status;
    s32 result;
#elif Linux
    s32 flags;
    s32 result;
#endif
};

void async_read(async_task *t,  io_handle h, void *buf, s64 buf_size);
void async_read(async_task *t,  io_handle h, void *buf, s64 buf_size, s64 offset);
void async_write(async_task *t, io_handle h, void *buf, s64 buf_size);
void async_write(async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset);

struct io_buffer
{
    void *data;
#if Linux
    s64   size; // not used on windows, all buffers must be of size pagesize
#endif
};

void async_read_scatter(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count);
void async_read_scatter(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);
void async_write_gather(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count);
void async_write_gather(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);

bool async_submit_tasks(error *err = nullptr);
void async_process_open_tasks();
bool async_task_is_done(async_task *task);
bool async_await(async_task *task, error *err = nullptr);
