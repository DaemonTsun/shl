
#pragma once

/* thread.hpp

Threading library.

Example:

    void *my_thread_function(void *arg)
    {
    ...
    }

    thread t{};

    create_thread(&t, my_thread_function);

    thread_start(&t);

    if (thread_is_running(&t)) { ... }
    if (thread_is_stopped(&t)) { ... }

    thread_stop(&t);

    void *return_value = thread_result(&t);

    ...

    thread_destroy(&t);


Typedefs and structs

thread_function is a function type that takes a void pointer argument and returns
a void pointer. It is the user entry point type for threads; the argument is
supplied by the user.

The thread struct contains information about a thread: its ID, starting function,
starting function argument, the starting context (which is copied when starting
the thread), and OS specific information necessary to manipulate the thread.

A thread may access a thread-local (but persisting after the thread exits)
allocator in the program context. The available size of this allocator is
determined by the storage_size parameter in thread_create().
Anything that should persist after the thread exits but is allocated within
the thread should be stored using the thread-local allocator, e.g. the
return value of the user function.


Thread state diagram:

    thread_create()  <--------------------
    thread_is_ready() == true       |    |
    THREAD_STATE_READY              |    |
            |                       |    |
            |                       |    |
            V                       |    |
    thread_start()                  |    |
    thread_is_running() == true     |    |
    THREAD_STATE_RUNNING            |    |
            |                       |    |
            |                       |    | destroyed / create goes both directions
            V                       |    |
    Thread function exits.  >--------    |
    you can wait for a thread to         |
    exit with thread_stop().             |
    thread_is_stopped() == true          |
    THREAD_STATE_STOPPED                 |
            |                            |
            |                            |
            V                            |
    thread_destroy() <--------------------
    invalid state
    

For obvious reasons, you shouldn't destroy a thread while it's running.

Functions:
thread_create(*thread, Func, *Arg[, *error])
    Creates a new thread object or reuses an existing one, if possible, and sets
    up OS specific thread data.

    Func is the thread function to be executed, with Arg as its argument.
    
    Returns whether or not the function succeeded.


thread_create(*thread, Func, *Arg, *Context, Stack_size, Storage_size[, *error])
    Creates a new thread object or reuses an existing one, if possible, and sets
    up OS specific thread data.

    Func is the thread function to be executed, with Arg as its argument.

    Context is the starting program_context to be used as the context of the
    thread. The context is copied into thread storage.

    Stacksize is the requested size of the thread stack, rounded up to the
    nearest pagesize.

    Storagesize is the requested size for extra memory which may be accessed
    by the thread as long as the thread is running, and by other threads
    as long as the target thread is stopped (or ready) and not destroyed.
    At the head of the extra storage is an arena, directly after which is
    the copied program_context. Use arena->start to find the first free
    address within the storage size, if necessary.
    
    Returns whether or not the function succeeded.


thread_destroy(*thread[, *error])
    Destroys the thread object. Thread should be ready or stopped before
    being destroyed.
    Returns whether or not the function succeeded.


thread_start(*thread[, *error])
    Starts the thread with the configuration from thread_create.
    Returns whether or not the function succeeded.


thread_is_ready(*thread)
    Returns whether or not the thread is in a "ready" state and available
    to be started.
    Note for this and the other state functions: these functions are, if
    possible, atomic operations.


thread_is_running(*thread)
    Returns whether or not the thread is in a "running" state.
    You may wait for the thread to stop with thread_stop().


thread_is_stopped(*thread)
    Returns whether or not the thread is in a "stopped" state.


thread_stop(*thread, *wait_timeout = nullptr[, *error])
    Waits until the thread is stopped.
    If wait_timeout is nullptr, waits indefinitely, otherwise waits
    for at most wait_timeout time.
    Returns whether or not the thread is stopped.


thread_result(*thread)
    Returns the return value of the user function of the thread.
    Should only be used after a thread is stopped.


thread_storage(*thread, *out_data, *out_size)
    Sets out_data to the start of the extra storage allocated for the thread
    to be used as thread storage.
    Sets out_size to the size of the storage.
    An arena object is located at the start of the storage, with the first
    member being a pointer to the first free space within the storage.

*/

#include "shl/error.hpp"
#include "shl/time.hpp"
#include "shl/program_context.hpp"

typedef void *(*thread_function)(void *arg);

struct thread
{
    s32 thread_id;
    thread_function starting_function;
    void *argument;
    program_context *starting_context;
    void *os_thread_data;
};

bool thread_create(thread *t, thread_function func, void *argument = nullptr, error *err = nullptr);
bool thread_create(thread *t, thread_function func, void *argument, program_context *ctx_base, s64 stack_size, s64 storage_size, error *err = nullptr);
bool thread_destroy(thread *t, error *err = nullptr);

bool thread_start(thread *t, error *err = nullptr);
bool thread_is_ready(thread *t);
bool thread_is_running(thread *t);
bool thread_is_stopped(thread *t);

bool thread_stop(thread *t, timespan *wait_timeout = nullptr, error *err = nullptr);

// make sure thread is stopped and not destroyed for these
void *thread_result(thread *t);
void thread_storage(thread *t, void **out_data, s64 *out_size);
