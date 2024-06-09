
/* async_io.hpp

Low-level async functions.

TODO: docs
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
    s64   size; // not used on windows, all buffers must be of size pagesize
};

#if Windows
#define io_buffer_size(X) 0
#else
#define io_buffer_size(X) X
#endif

void async_read_scatter(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count);
void async_read_scatter(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);
void async_write_gather(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count);
void async_write_gather(async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);

bool async_submit_tasks(error *err = nullptr);
void async_process_open_tasks();
bool async_task_is_done(async_task *task);
bool async_await(async_task *task, error *err = nullptr);
