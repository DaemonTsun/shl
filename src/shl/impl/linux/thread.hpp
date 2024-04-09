
/* linux/thread.hpp

Linux threads and utility functions.
*/

#pragma once

#include "shl/number_types.hpp"
#include "shl/error.hpp"
#include "shl/impl/linux/futex.hpp"

struct clone_args
{
    u64 flags;
    u64 pidfd;          // cast to int*
    u64 child_tid;      // cast to sys_int*
    u64 parent_tid;     // cast to sys_int*
    u64 exit_signal;
    u64 stack;          // cast to void*, START of stack ("low end")
    s64 stack_size;     // This one is weirder. you should write the 
    // thread entry point somewhere near the end of the stack, then set this stack size
    // to <address of where you wrote the entry point> - stack (the field before).
    // This makes sure the entry point is the next thing in the threads stack, executing
    // it on start.
    u64 tls;
    u64 set_tid;        // pid_t array??
    u64 set_tid_size;
    u64 cgroup;
};

// You probably don't want this, as this gets the clone_args as argument.
extern "C" sys_int clone3(clone_args *args, s64 arg_size = sizeof(clone_args));

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

#define CLONE_DEFAULT_FLAGS (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_PARENT | CLONE_THREAD | CLONE_IO)

#ifndef THREAD_STATE_READY
#  define THREAD_STATE_READY      0x00
#  define THREAD_STATE_RUNNING    0x01
#  define THREAD_STATE_STOPPED    0x02
#endif

// everything below here is utility
void *thread_stack_create(s64 size, error *err = nullptr);
bool  thread_stack_destroy(void *stack, s64 size, error *err = nullptr);

struct thread_stack_head;

typedef void (*clone_entry)(thread_stack_head *head);
typedef void *(*thread_entry)(void *argument);

// also adjust asm/<arch>/linux_thread.S when changing the size of this struct.
struct __attribute((aligned(16))) thread_stack_head
{
    clone_entry entry;
    thread_entry user_function;
    void *user_function_argument;
    void *user_function_result;
    s64 original_stack_size; // stack size in clone_args is different.
    void *extra_data; // pointer to extra data on stack
    s64 extra_data_size;
    s64 tid;
    s32 join_futex;
    s32 state;

    ::clone_args clone_args;
};

thread_stack_head *get_thread_stack_head(void *stack, s64 size, s64 extra_size = 0);

void default_clone_entry(thread_stack_head *head);

sys_int linux_thread_start(thread_stack_head *head, error *err = nullptr);

#define _check_linux_thread_state(Head, State)\
    (__atomic_load_n(&(Head)->state, __ATOMIC_SEQ_CST) == State)

#define linux_thread_is_ready(Head)   _check_linux_thread_state(Head, THREAD_STATE_READY)
#define linux_thread_is_running(Head) _check_linux_thread_state(Head, THREAD_STATE_RUNNING)
#define linux_thread_is_stopped(Head) _check_linux_thread_state(Head, THREAD_STATE_STOPPED)

bool linux_thread_join(thread_stack_head *head, timespan *timeout = nullptr, error *err = nullptr);
