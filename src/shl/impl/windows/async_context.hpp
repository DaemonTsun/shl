
/* windows/async_context.hpp

Helper structure for OVERLAPPED on win32.
*/

#pragma once

#include <windows.h>
#include "shl/async_io.hpp"
#include "shl/impl/async_common.hpp"

struct async_overlapped
{
    // same structure as win32 OVERLAPPED
    u64 internal;
    u64 internal_high;
    u64 offset;
    void *event;
};

enum class async_op
{
    Read,
    Write,
    ReadScatter,
    WriteGather,
    // TODO: Sockets https://learn.microsoft.com/en-us/windows/win32/winsock/overlapped-i-o-and-event-objects-2
};

struct async_command
{
    int status;
    async_op   op;
    async_overlapped overlapped;
    async_task *task;
    io_handle handle;

    union
    {
        struct
        {
            void *buffer;
            s32 bytes_to_read;
        } read_args;

        struct
        {
            void *buffer;
            s32 bytes_to_write;
        } write_args;

        struct
        {
            io_buffer *buffers;
            s32 buffer_count;
        } read_scatter_args;

        struct
        {
            io_buffer *buffers;
            s32 buffer_count;
        } write_gather_args;
    };
};

struct async_context
{
    async_command commands[ASYNC_ENTRIES];
    HANDLE events[ASYNC_ENTRIES];
    u16 submit_queue[ASYNC_ENTRIES];
    u32 submit_index;
    u32 submit_count;
};

bool init(async_context *ctx, error *err);
void free(async_context *ctx);

void async_cmd_read(async_context *ctx, async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset);
void async_cmd_write(async_context *ctx, async_task *t, io_handle h, void *buf, s64 buf_size, s64 offset);

void async_cmd_read_scatter(async_context *ctx, async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);
void async_cmd_write_gather(async_context *ctx, async_task *t, io_handle h, io_buffer *buffers, s64 buffer_count, s64 offset);
// TODO: sockets, etc

bool async_submit_commands(async_context *ctx, error *err);
void async_process_open_commands(async_context *ctx);
bool async_await_task(async_context *ctx, async_task *task, error *err);
