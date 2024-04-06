
#include "shl/memory.hpp"
#include "shl/print.hpp"
#include <stdio.h>

#include "shl/impl/linux/exit.hpp"
#include "shl/impl/linux/memory.hpp"
#include "shl/impl/linux/thread.hpp"

void *thread_stack_create(s64 size, error *err)
{
    void *ret = mmap(nullptr, size,
                PROT_WRITE | PROT_READ,
                MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN | MAP_STACK);

    sys_uint errcode = (sys_uint)ret;

    // [-4096, -1] is the range for errors.
    if (errcode > -4096ul)
    {
        set_error_by_code(err, -((sys_int)errcode));
        return nullptr;
    }

    return ret;
}

#define aligned_u64 u64 __attribute__((aligned(8)))

thread_stack_head *get_thread_stack_head(void *stack, s64 size)
{
    if (size < (s64)sizeof(thread_stack_head))
        return nullptr;

    s64 count = size / sizeof(thread_stack_head);

    thread_stack_head *ret = (thread_stack_head*)stack + count - 1;
    fill_memory(ret, 0);
    ret->setup = default_clone_entry;
    ret->clone_args.flags = CLONE_VM | CLONE_FS | CLONE_FILES
                          | CLONE_SIGHAND | CLONE_PARENT | CLONE_THREAD
                          // | CLONE_SYSVSEM | CLONE_SETTLS
                          | CLONE_IO;
    ret->clone_args.stack = (aligned_u64)stack;
    ret->clone_args.stack_size = (u64)((char*)ret - (char*)stack);
    // ret->clone_args.tls = (u64)stack + (size / 2);

    return ret;
}

void default_clone_entry(thread_stack_head *s)
{
    (void)s;
    put("fug\n");
    // inside new thread
    // s->result = s->entry(s->argument);

    // __atomic_store_n(&s->join_futex, 1, __ATOMIC_SEQ_CST);
    // futex_wake(&s->join_futex);
    exit(0);
}

extern "C" void *_linux_thread_start(void*);

sys_int linux_thread_start(thread_stack_head *head, error *err)
{
    printf("%p\n", (void*)head);
    sys_int ret = (sys_int)_linux_thread_start((void*)head);

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
        return -1;
    }

    return ret;
}
