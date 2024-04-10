
#include "shl/platform.hpp"
#include "shl/assert.hpp"
#include "shl/thread.hpp"
#include "shl/allocator_arena.hpp"

#if Windows
#include <windows.h>
#else
#include "shl/impl/linux/thread.hpp"
#include "shl/impl/linux/exit.hpp"

#define Stacksize         0x00300000
#define Stacksize_Storage 0x00100000

static void _clone_entry(thread_stack_head *head)
{
    // this is within the new thread

    // The context is within the extra data of the thread stack, guaranteeing
    // the context is accessible and survives for as long as the thread stack
    // exists.
    program_context *nctx = (program_context*)((char*)head->extra_data + sizeof(arena));
    nctx->thread_id = head->tid;
    set_context_pointer(nctx);

    __atomic_store_n(&head->state, THREAD_STATE_RUNNING, __ATOMIC_SEQ_CST);
    head->user_function_result = head->user_function(head->user_function_argument);
    __atomic_store_n(&head->state, THREAD_STATE_STOPPED, __ATOMIC_SEQ_CST);

    futex_wake(&head->join_futex);
    exit(0);
}
#endif

bool thread_create(thread *t, thread_function func, void *argument, error *err)
{
    return thread_create(t, func, argument, nullptr, 0, 0, err);
}

bool thread_create(thread *t, thread_function func, void *argument, program_context *ctx_base, s64 stack_size, s64 storage_size, error *err)
{
    assert(t != nullptr);
    
    if (ctx_base == nullptr)
        ctx_base = get_context_pointer();

#if Linux
    thread_stack_head *head = nullptr;

    void *stack = nullptr;

    if (t->os_thread_data == nullptr)
    {
        // a destroyed or new thread
        if (stack_size <= 0)
            stack_size = Stacksize;

        if (storage_size <= 0)
            storage_size = Stacksize_Storage;

        stack = thread_stack_create(stack_size, err);

        if (stack == nullptr)
            return false;
    }
    else
    {
        head = (thread_stack_head*)t->os_thread_data;
        stack = (void*)head->clone_args.stack;
        assert(stack != nullptr);

        if (stack_size > 0)
        {
            // check if new stack size larger different than before,
            // then reallocate if it is.
            if (stack_size > head->original_stack_size)
            {
                if (!thread_stack_destroy(stack, head->original_stack_size, err))
                    return false;

                stack = thread_stack_create(stack_size, err);

                if (stack == nullptr)
                    return false;
            }
        }
        else
            stack_size = head->original_stack_size;

        if (storage_size <= 0)
            storage_size = head->extra_data_size;
    }

    head = get_thread_stack_head(stack, stack_size, storage_size);

    if (head == nullptr)
    {
        thread_stack_destroy(stack, stack_size);
        return false;
    }

    t->os_thread_data = (void*)head;

    t->thread_id = -1;
    t->starting_function = func;
    t->argument = argument;

    head->entry = _clone_entry;
    head->user_function = func;
    head->user_function_argument = argument;
    head->clone_args.flags |= CLONE_PARENT_SETTID;
    head->clone_args.parent_tid = (u64)(&t->thread_id);

    // we set up the arena (stack) allocator at the extra storage within
    // the thread stack.
    arena *storage_arena = (arena*)head->extra_data;
    storage_arena->start = (char*)head->extra_data + sizeof(arena);
    storage_arena->end = storage_arena->start + head->extra_data_size - sizeof(arena);

    allocator storage_alloc = arena_allocator(storage_arena);

    program_context *nctx = allocator_alloc_T(storage_alloc, program_context);
    *nctx = *ctx_base;
    nctx->thread_storage_allocator = storage_alloc;
    t->starting_context = nctx;

    return true;
#else
    // TODO: implement
    return false;
#endif
}

bool thread_destroy(thread *t, error *err)
{
    assert(t != nullptr);

#if Linux
    assert(t->os_thread_data != nullptr);
    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    assert(!linux_thread_is_running(head));

    void *stack = (void*)head->clone_args.stack;
    assert(stack != nullptr);
    
    bool ok = thread_stack_destroy(stack, head->original_stack_size, err);

    if (!ok)
        return false;

    t->os_thread_data = nullptr;
    t->thread_id = -1;

    return true;
#else
    // TODO: implement
    return false;
#endif
}

bool thread_start(thread *t, error *err)
{
    assert(t != nullptr);

#if Linux
    assert(t->os_thread_data != nullptr);
    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    assert(!linux_thread_is_running(head));

    sys_int ok = linux_thread_start(head, err);

    return ok > 0;
#else
    // TODO: implement
    return false;
#endif
}

bool thread_is_ready(thread *t)
{
    assert(t != nullptr);

#if Linux
    assert(t->os_thread_data != nullptr);
    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;
    return linux_thread_is_ready(head);
#else
    // TODO: implement
    return false;
#endif
}

bool thread_is_running(thread *t)
{
    assert(t != nullptr);

#if Linux
    if (t->os_thread_data == nullptr)
        return false;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;
    return linux_thread_is_running(head);
#else
    // TODO: implement
    return false;
#endif
}

bool thread_is_stopped(thread *t)
{
    assert(t != nullptr);

#if Linux
    if (t->os_thread_data == nullptr)
        return false;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;
    return linux_thread_is_stopped(head);
#else
    // TODO: implement
    return false;
#endif
}

bool thread_stop(thread *t, timespan *wait_timeout, error *err)
{
    assert(t != nullptr);

#if Linux
    if (t->os_thread_data == nullptr)
        return true;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    if (linux_thread_is_stopped(head))
        return true;

    if (!linux_thread_join(head, wait_timeout, err))
        return false;

    return linux_thread_is_stopped(head);
#else
    // TODO: implement
    return false;
#endif
}

void *thread_result(thread *t)
{
#if Linux
    if (t->os_thread_data == nullptr)
        return nullptr;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    // Since we're assuming the thread is stopped by this point, there is
    // no reason to lock anything.
    return head->user_function_result;
#else
    // TODO: implement
    return nullptr;
#endif
}
