
/* io_uring.hpp

Linux io_uring header.
*/
#pragma once

#include "shl/number_types.hpp"

// Submission offsets
struct io_sqring_offsets
{
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
struct io_cqring_offsets
{
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

// param features, set by io_uring_setup
#define IORING_FEAT_SINGLE_MMAP     (1U << 0)
#define IORING_FEAT_NODROP          (1U << 1)
#define IORING_FEAT_SUBMIT_STABLE   (1U << 2)
#define IORING_FEAT_RW_CUR_POS      (1U << 3)
#define IORING_FEAT_CUR_PERSONALITY (1U << 4)
#define IORING_FEAT_FAST_POLL       (1U << 5)
#define IORING_FEAT_POLL_32BITS     (1U << 6)
#define IORING_FEAT_SQPOLL_NONFIXED (1U << 7)
#define IORING_FEAT_EXT_ARG         (1U << 8)
#define IORING_FEAT_NATIVE_WORKERS  (1U << 9)
#define IORING_FEAT_RSRC_TAGS       (1U << 10)
#define IORING_FEAT_CQE_SKIP        (1U << 11)
#define IORING_FEAT_LINKED_FILE     (1U << 12)

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

enum io_uring_op
{
    IORING_OP_NOP,
    IORING_OP_READV,
    IORING_OP_WRITEV,
    IORING_OP_FSYNC,
    IORING_OP_READ_FIXED,
    IORING_OP_WRITE_FIXED,
    IORING_OP_POLL_ADD,
    IORING_OP_POLL_REMOVE,
    IORING_OP_SYNC_FILE_RANGE,
    IORING_OP_SENDMSG,
    IORING_OP_RECVMSG,
    IORING_OP_TIMEOUT,
    IORING_OP_TIMEOUT_REMOVE,
    IORING_OP_ACCEPT,
    IORING_OP_ASYNC_CANCEL,
    IORING_OP_LINK_TIMEOUT,
    IORING_OP_CONNECT,
    IORING_OP_FALLOCATE,
    IORING_OP_OPENAT,
    IORING_OP_CLOSE,
    IORING_OP_FILES_UPDATE,
    IORING_OP_STATX,
    IORING_OP_READ,
    IORING_OP_WRITE,
    IORING_OP_FADVISE,
    IORING_OP_MADVISE,
    IORING_OP_SEND,
    IORING_OP_RECV,
    IORING_OP_OPENAT2,
    IORING_OP_EPOLL_CTL,
    IORING_OP_SPLICE,
    IORING_OP_PROVIDE_BUFFERS,
    IORING_OP_REMOVE_BUFFERS,
    IORING_OP_TEE,
    IORING_OP_SHUTDOWN,
    IORING_OP_RENAMEAT,
    IORING_OP_UNLINKAT,
    IORING_OP_MKDIRAT,
    IORING_OP_SYMLINKAT,
    IORING_OP_LINKAT,
    IORING_OP_MSG_RING,
    IORING_OP_FSETXATTR,
    IORING_OP_SETXATTR,
    IORING_OP_FGETXATTR,
    IORING_OP_GETXATTR,
    IORING_OP_SOCKET,
    IORING_OP_URING_CMD,
    IORING_OP_SEND_ZC,
    IORING_OP_SENDMSG_ZC,

    IORING_OP_LAST,
};

// SQE flags
#define IOSQE_FIXED_FILE	    0x01
#define IOSQE_IO_DRAIN		    0x02
#define IOSQE_IO_LINK		    0x04
#define IOSQE_IO_HARDLINK	    0x08
#define IOSQE_ASYNC		        0x10
#define IOSQE_BUFFER_SELECT 	0x20
#define IOSQE_CQE_SKIP_SUCCESS	0x40

// Submission entry
struct io_uring_sqe
{
    u8  opcode;
    u8  flags;
    u16 priority;
    s32 fd;
    s64 offset;
    void *data; // "addr"
    u32 data_size; // "len"

    union
    {
        u32 op_flags;
        u16 op_flags16;
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

// enter flags
#define IORING_ENTER_GETEVENTS          (1U << 0)
#define IORING_ENTER_SQ_WAKEUP          (1U << 1)
#define IORING_ENTER_SQ_WAIT            (1U << 2)
#define IORING_ENTER_EXT_ARG            (1U << 3)
#define IORING_ENTER_REGISTERED_RING    (1U << 4)

sys_int io_uring_enter(int fd, u32 to_submit, u32 min_complete, int flags, void *args, sys_int arg_count);
sys_int io_uring_register(int fd, int opcode, void *args, sys_int arg_count);

// mmap offsets
#define IORING_OFF_SQ_RING  0ULL
#define IORING_OFF_CQ_RING  0x8000000ULL
#define IORING_OFF_SQES     0x10000000ULL
