
#include "shl/memory.hpp" // fill_memory
#include "shl/math.hpp"

#include "shl/impl/linux/exit.hpp"
#include "shl/impl/linux/memory.hpp"
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/sysinfo.hpp"
#include "shl/impl/linux/thread.hpp"

extern "C" sys_int clone3(clone_args *args, s64 arg_size)
{
    return (sys_int)linux_syscall2(SYS_clone3,
                                   (void*)args,
                                   (void*)(sys_int)arg_size);
}

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

bool thread_stack_destroy(void *stack, s64 size, error *err)
{
    if (sys_int ret = munmap(stack, size); ret < 0)
    {
        set_error_by_code(err, -ret);
        return false;
    }

    return true;
}

thread_stack_head *get_thread_stack_head(void *stack, s64 size, s64 extra_size)
{
    static s64 pagesize = get_pagesize();
    extra_size = ceil_multiple2(extra_size, pagesize);

    if ((size - extra_size) < (s64)sizeof(thread_stack_head))
        return nullptr;

    s64 count = (size - extra_size) / sizeof(thread_stack_head);

    thread_stack_head *ret = (thread_stack_head*)stack + count - 1;
    fill_memory(ret, 0);

    ret->entry = default_clone_entry;
    ret->original_stack_size = size;
    ret->extra_data = (void*)((char*)stack + size - extra_size);
    ret->extra_data_size = extra_size;
    ret->state = THREAD_STATE_READY;
    ret->clone_args.flags = CLONE_DEFAULT_FLAGS | CLONE_CHILD_SETTID;
    ret->clone_args.child_tid = (u64)&ret->tid;
    ret->clone_args.stack = (u64)stack;
    ret->clone_args.stack_size = (s64)((char*)ret - (char*)stack);

    return ret;
}

extern "C" void *_linux_thread_start(thread_stack_head *head);

void default_clone_entry(thread_stack_head *head)
{
    // this is within the new thread
    __atomic_store_n(&head->state, THREAD_STATE_RUNNING, __ATOMIC_SEQ_CST);
    head->user_function_result = head->user_function(head->user_function_argument);
    __atomic_store_n(&head->state, THREAD_STATE_STOPPED, __ATOMIC_SEQ_CST);

    // __atomic_store_n(&head->join_futex, 1, __ATOMIC_SEQ_CST);
    futex_wake(&head->join_futex);
    exit(0);
}

sys_int linux_thread_start(thread_stack_head *head, error *err)
{
    sys_int ret = (sys_int)_linux_thread_start(head);

    if (ret < 0)
        set_error_by_code(err, -ret);

    return ret;
}

bool linux_thread_join(thread_stack_head *head, timespan *timeout, error *err)
{
    sys_int ret = futex_wait(&head->join_futex, 0, timeout);

    if (ret < 0)
    {
        set_error_by_code(err, -ret);
        return false;
    }

    return true;
}
