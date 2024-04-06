
/* linux/thread.hpp

Linux threads and utility functions.
*/

#pragma once

#include "shl/number_types.hpp"
#include "shl/error.hpp"

void *thread_stack_create(s64 size, error *err = nullptr);
void  thread_stack_destroy(void *stack, error *err = nullptr);

struct thread_stack_head;

typedef void (*clone_entry)(thread_stack_head *s);
typedef void *(*thread_entry)(void *arg);

struct __attribute((aligned(16))) thread_stack_head
{
    clone_entry setup;
    thread_entry entry;
    void *argument;
    void *result;
    s64 join_futex;

    struct clone_args
    {
        u64 flags;
        u64 pidfd;          // cast to int*
        u64 child_tid;      // cast to sys_int*
        u64 parent_tid;     // cast to sys_int*
        u64 exit_signal;
        u64 stack;          // cast to void*
        s64 stack_size;
        u64 tls;            // cast to void*
        u64 set_tid;        // pid_t array??
        u64 set_tid_size;
        u64 cgroup;
    } clone_args;
};

thread_stack_head *get_thread_stack_head(void *stack, s64 size);

void default_clone_entry(thread_stack_head *s);

sys_int linux_thread_start(thread_stack_head *head, error *err);

#ifndef CLONE_VM
#  define CSIGNAL               0x000000ff
#  define CLONE_NEWTIME         0x00000080
#  define CLONE_VM              0x00000100
#  define CLONE_FS              0x00000200
#  define CLONE_FILES           0x00000400
#  define CLONE_SIGHAND         0x00000800
#  define CLONE_PIDFD           0x00001000
#  define CLONE_PTRACE          0x00002000
#  define CLONE_VFORK           0x00004000
#  define CLONE_PARENT          0x00008000
#  define CLONE_THREAD          0x00010000
#  define CLONE_NEWNS           0x00020000
#  define CLONE_SYSVSEM         0x00040000
#  define CLONE_SETTLS          0x00080000
#  define CLONE_PARENT_SETTID   0x00100000
#  define CLONE_CHILD_CLEARTID  0x00200000
#  define CLONE_DETACHED        0x00400000
#  define CLONE_UNTRACED        0x00800000
#  define CLONE_CHILD_SETTID    0x01000000
#  define CLONE_NEWCGROUP       0x02000000
#  define CLONE_NEWUTS          0x04000000
#  define CLONE_NEWIPC          0x08000000
#  define CLONE_NEWUSER         0x10000000
#  define CLONE_NEWPID          0x20000000
#  define CLONE_NEWNET          0x40000000
#  define CLONE_IO              0x80000000
#endif
