
#include "shl/print.hpp"

#include "shl/platform.hpp"
#define ASSERT_PRINT_MESSAGE
#include "shl/assert.hpp"
#include "shl/math.hpp"
#include "shl/allocator_arena.hpp"
#include "shl/impl/thread_common.hpp"
#include "shl/thread.hpp"

#if Windows
#include <windows.h>

// This is not a "thread_stack_head" because "head" implies it's somewhere
// meaningful in the stack, but it's not.
struct windows_thread_stack_info
{
    HANDLE thread_handle;
    thread_entry user_function;
    void *user_function_argument;
    void *user_function_result;
    s64 stack_size;
    s64 storage_size;
    void *extra_data;
    s64 extra_data_size;
    s32 tid;
    // s32 join_mutex;
    // s32 state;
};

static inline s64 _get_allocation_granularity()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (s64)info.dwAllocationGranularity;
}

static u32 _CreateThread_entry(void *argument)
{
    assert(argument != nullptr);

    windows_thread_stack_info *info = (windows_thread_stack_info*)argument;

    program_context *nctx = (program_context*)((char*)info->extra_data + sizeof(arena));
    nctx->thread_id = info->tid;
    set_context_pointer(nctx);

    // info->state = THREAD_STATE_RUNNING;
    info->user_function_result = info->user_function(info->user_function_argument);
    // info->state = THREAD_STATE_STOPPED;

    ExitThread(0);
}

static inline s32 _get_thread_status(HANDLE h)
{
    if (h == nullptr)
        return THREAD_STATE_READY;

    if (WaitForSingleObject(h, 0) == 0)
        return THREAD_STATE_STOPPED;
    else
    {
        DWORD code = -1u;

        if (GetExitCodeThread(h, &code) && code == STILL_ACTIVE)
            return THREAD_STATE_RUNNING;
    }

    return -1;
}

#else // Linux
#include "shl/impl/linux/thread.hpp"
#include "shl/impl/linux/exit.hpp"

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

#if Windows
    windows_thread_stack_info *info = nullptr;
    static s64 alloc_granularity = _get_allocation_granularity();
    HANDLE old_thread = nullptr;

    if (t->os_thread_data == nullptr)
    {
        // a destroyed or new thread
        if (stack_size <= 0)
            stack_size = THREAD_STACKSIZE_DEFAULT;

        if (storage_size <= 0)
            storage_size = THREAD_STACKSIZE_STORAGE_DEFAULT;

        stack_size   = ceil_multiple2(stack_size,   alloc_granularity);
        storage_size = ceil_multiple2(storage_size, alloc_granularity);

        info = (windows_thread_stack_info*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, storage_size);

        if (info == nullptr)
        {
            set_GetLastError_error(err);
            return false;
        }
    }
    else
    {
        info = (windows_thread_stack_info*)t->os_thread_data;
        old_thread = info->thread_handle;

        if (stack_size <= 0)
            stack_size = info->stack_size;
        else
            stack_size = ceil_multiple2(stack_size, alloc_granularity);

        if (storage_size <= 0)
            storage_size = info->extra_data_size;
        else if (storage_size > info->extra_data_size)
        {
            storage_size = ceil_multiple2(storage_size, alloc_granularity);
            info = (windows_thread_stack_info*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (void*)info, storage_size);

            if (info == nullptr)
            {
                set_GetLastError_error(err);
                return false;
            }
        }
    }

    if (old_thread != nullptr)
    {
        if (_get_thread_status(old_thread) == THREAD_STATE_RUNNING)
        {
            if (!TerminateThread(info->thread_handle, 0))
            {
                set_GetLastError_error(err);
                return false;
            }
        }

        if (!CloseHandle(old_thread))
        {
            set_GetLastError_error(err);
            return false;
        }

        info->thread_handle = nullptr;
    }

    info->user_function = func;
    info->user_function_argument = argument;
    info->stack_size = stack_size;
    info->storage_size = storage_size;
    info->extra_data = (void*)((char*)info + alloc_granularity);
    info->extra_data_size = storage_size - alloc_granularity;

    t->os_thread_data = (void*)info;

    // we set up the arena (stack) allocator at the extra storage within
    // the thread stack.
    arena *storage_arena = (arena*)info->extra_data;
    storage_arena->start = (char*)info->extra_data + sizeof(arena);
    storage_arena->end = storage_arena->start + info->extra_data_size - sizeof(arena);

    allocator storage_alloc = arena_allocator(storage_arena);

    program_context *nctx = allocator_alloc_T(storage_alloc, program_context);
    *nctx = *ctx_base;
    nctx->thread_storage_allocator = storage_alloc;
    t->starting_context = nctx;

    return true;
#elif Linux
    thread_stack_head *head = nullptr;

    void *stack = nullptr;

    if (t->os_thread_data == nullptr)
    {
        // a destroyed or new thread
        if (stack_size <= 0)
            stack_size = THREAD_STACKSIZE_DEFAULT;

        if (storage_size <= 0)
            storage_size = THREAD_STACKSIZE_STORAGE_DEFAULT;

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
    return false;
#endif
}

bool thread_destroy(thread *t, error *err)
{
    assert(t != nullptr);

#if Windows
    assert(t->os_thread_data != nullptr);
    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    s32 state = _get_thread_status(info->thread_handle);

    if (state == THREAD_STATE_RUNNING)
    {
        if (!TerminateThread(info->thread_handle, 0))
        {
            set_GetLastError_error(err);
            return false;
        }
    }

    if (info->thread_handle != nullptr)
    {
        if (!CloseHandle(info->thread_handle))
        {
            set_GetLastError_error(err);
            return false;
        }

        info->thread_handle = nullptr;
    }

    if (info->storage_size > 0 && !HeapFree(GetProcessHeap(), 0, (void*)info))
    {
        set_GetLastError_error(err);
        return false;
    }

    info = nullptr;

    t->os_thread_data = nullptr;
    t->thread_id = -1;

    return true;
#elif Linux
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
    return false;
#endif
}

bool thread_start(thread *t, error *err)
{
    assert(t != nullptr);

#if Windows
    assert(t->os_thread_data != nullptr);
    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    [[maybe_unused]] s32 state = _get_thread_status(info->thread_handle);
    assert(state == THREAD_STATE_READY);

    info->thread_handle = CreateThread(
        nullptr, // security attributes
        (SIZE_T)info->stack_size,
        (LPTHREAD_START_ROUTINE)_CreateThread_entry,
        (void*)info,
        (DWORD)0,
        (LPDWORD)&info->tid
    );

    if (info->thread_handle == nullptr)
    {
        set_GetLastError_error(err);
        return false;
    }

    t->thread_id = info->tid;

    return true;
#elif Linux
    assert(t->os_thread_data != nullptr);
    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    assert(!linux_thread_is_running(head));

    sys_int ok = linux_thread_start(head, err);

    return ok > 0;
#else
    return false;
#endif
}

bool thread_is_ready(thread *t)
{
    assert(t != nullptr);

#if Windows
    assert(t->os_thread_data != nullptr);
    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    s32 state = _get_thread_status(info->thread_handle);
    return state == THREAD_STATE_READY;
#elif Linux
    assert(t->os_thread_data != nullptr);
    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;
    return linux_thread_is_ready(head);
#else
    return false;
#endif
}

bool thread_is_running(thread *t)
{
    assert(t != nullptr);

#if Windows
    assert(t->os_thread_data != nullptr);
    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    s32 state = _get_thread_status(info->thread_handle);
    return state == THREAD_STATE_RUNNING;
#elif Linux
    if (t->os_thread_data == nullptr)
        return false;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;
    return linux_thread_is_running(head);
#else
    return false;
#endif
}

bool thread_is_stopped(thread *t)
{
    assert(t != nullptr);

#if Windows
    assert(t->os_thread_data != nullptr);
    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    s32 state = _get_thread_status(info->thread_handle);
    return state == THREAD_STATE_STOPPED;
#elif Linux
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

#if Windows
    if (t->os_thread_data == nullptr)
        return true;

    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    s32 state = _get_thread_status(info->thread_handle);

    if (state == THREAD_STATE_STOPPED)
        return true;

    u32 ms_timeout = 0; 

    if (wait_timeout == nullptr)
        ms_timeout = INFINITE;
    else
    {
        ms_timeout += (u32)wait_timeout->seconds * 1000;
        ms_timeout += (u32)wait_timeout->nanoseconds / 1000000;
    }

    DWORD res = WaitForSingleObject(info->thread_handle, ms_timeout);

    if (res != 0)
    {
        set_GetLastError_error(err);
        return false;
    }

    return true;
#elif Linux
    if (t->os_thread_data == nullptr)
        return true;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    if (linux_thread_is_stopped(head))
        return true;

    if (!linux_thread_join(head, wait_timeout, err))
        return false;

    return linux_thread_is_stopped(head);
#else
    return false;
#endif
}

void *thread_result(thread *t)
{
#if Windows
    if (t->os_thread_data == nullptr)
        return nullptr;

    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;

    return info->user_function_result;
#elif Linux
    if (t->os_thread_data == nullptr)
        return nullptr;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;

    // Since we're assuming the thread is stopped by this point, there is
    // no reason to lock anything.
    return head->user_function_result;
#else
    return nullptr;
#endif
}

void thread_storage(thread *t, void **out_data, s64 *out_size)
{
    assert(out_data != nullptr);
    assert(out_size != nullptr);

#if Windows
    if (t->os_thread_data == nullptr)
        return;

    windows_thread_stack_info *info = (windows_thread_stack_info*)t->os_thread_data;
    *out_data = info->extra_data;
    *out_size = info->extra_data_size;
#elif Linux
    if (t->os_thread_data == nullptr)
        return;

    thread_stack_head *head = (thread_stack_head*)t->os_thread_data;
    *out_data = head->extra_data;
    *out_size = head->extra_data_size;
#else
    return;
#endif
}
