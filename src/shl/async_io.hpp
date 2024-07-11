
/* async_io.hpp

Low-level async functions.

TODO: docs

*/

#pragma once

#include "shl/platform.hpp"
#include "shl/io.hpp"
#include "shl/number_types.hpp"

struct async_task;

async_task *async_read(io_handle h, void *buf, s64 buf_size);
async_task *async_read(io_handle h, void *buf, s64 buf_size, s64 offset);
async_task *async_write(io_handle h, void *buf, s64 buf_size);
async_task *async_write(io_handle h, void *buf, s64 buf_size, s64 offset);

struct io_buffer
{
    void *data;
#if Linux
    s64   size; // not used on windows, all buffers must be of size pagesize
#endif
};

async_task *async_read_scatter(io_handle h, io_buffer *buffers, s64 buffer_count);
async_task *async_read_scatter(io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);
async_task *async_write_gather(io_handle h, io_buffer *buffers, s64 buffer_count);
async_task *async_write_gather(io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);

async_task *async_sleep(s64 seconds, s64 nanoseconds = 0);

bool async_submit_tasks(error *err = nullptr);
void async_process_open_tasks();
bool async_task_is_done(async_task *task);
static inline s64 async_task_result(async_task *task)
{
    return *((s64*)task);
}

bool async_await(async_task *task, error *err = nullptr);
