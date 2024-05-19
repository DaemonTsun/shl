
/* io_uring.hpp

Linux io_uring header.
*/
#pragma once

#include "shl/number_types.hpp"

extern "C"
{
// Submission offsets
struct io_sqring_offsets {
	u32 head;
	u32 tail;
	u32 ring_mask;
	u32 ring_entries;
	u32 flags;
	u32 dropped;
	u32 array;
	u32 _padding;
	u64 _reserved;
};

// Completion offsets
struct io_cqring_offsets {
	u32 head;
	u32 tail;
	u32 ring_mask;
	u32 ring_entries;
	u32 overflow;
	u32 cqes;
	u32 flags;
	u32 _padding;
	u64 _reserved;
};

struct io_uring_params
{
    u32 sq_entries;
    u32 cq_entries;
    u32 flags;
    u32 sq_thread_cpu;
    u32 sq_thread_idle;
    u32 features;
    u32 _reserved[4];
    struct io_sqring_offsets sq_off;
    struct io_cqring_offsets cq_off;
};

// Submission entry
struct io_uring_sqe
{
    u8  opcode;
    u8  flags;
    u16 priority;
    s32 fd;
    s64 offset;
    void *data;
    u32 data_size;

    union
    {
        u32 rw_flags;
        u32 fsync_flags;
        u16 poll_events;
        u32 sync_range_flags;
        u32 msg_flags;
    };
    
    void *userdata;

    union
    {
        u16 buf_index;
        u64 _reserved[3];
    };
};

// Completion entry
struct io_uring_cqe
{
    void *userdata;
    s32 result;
    u32 flags;
};

int io_uring_setup(u32 entries, io_uring_params *params);
sys_int io_uring_enter(int fd, u32 to_submit, u32 min_complete, int flags, void *args, sys_int arg_count);
sys_int io_uring_register(int fd, int opcode, void *args, sys_int arg_count);
}
