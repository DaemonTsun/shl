
#include "shl/platform.hpp"

#if Linux && !defined(NDEBUG)
#define ASSERT_PRINT_MESSAGE 1
#include "shl/assert.hpp"
#include "shl/impl/linux/exit.hpp"
#include "shl/impl/linux/memory.hpp"
#include "shl/impl/linux/thread.hpp"
#include "shl/print.hpp"
#include "shl/time.hpp"
#include <valgrind/valgrind.h>

typedef void (*clone_function)(clone_args* arg);
void clone_thread_func(clone_args *arg)
{
    put("clone_test: hello from thread\n");
    (void)arg;
    exit(0);
}

void clone_test()
{
    /* We use the clone3 syscall to create a thread, wait 2 seconds (without locks)
       and then deallocate the thread stack.
       
       This is just example code and has some drawbacks:

       - The thread function (clone_thread_func in this case) only gets 
         the clone_args as arguments and no other information, not even a
         user pointer, requiring the use of other mechanisms to synchronize
         with the parent thread.

       - The thread stack only houses the function pointer and nothing else.
         Maybe this is desired.
    */
    s64 stack_size = 4 * 1024 * 1024;
    void *stack = mmap(nullptr, stack_size,
                PROT_WRITE | PROT_READ,
                MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN | MAP_STACK);
    u64 stack_ret = (u64)stack;
    assert(stack_ret < -4096ul && stack_ret > 0);

    s64 count = stack_size / sizeof(clone_function);
    clone_function *stack_end = (clone_function*)stack + count - 1;
    *stack_end = clone_thread_func;

    put("clone_test: hello from main\n");

    sys_int tid = 0;
    clone_args args{};
    args.flags = CLONE_DEFAULT_FLAGS | CLONE_PARENT_SETTID;
    args.stack = (u64)stack;
    args.stack_size = (s64)((char*)stack_end - (char*)stack);
    args.parent_tid = (u64)&tid;

    sys_int ret = clone3(&args);
    assert(ret > 0);

    tprint("clone_test: hello from main after thread start, child thread id: %\n", tid);

    sleep_ms(2000);

    put("clone_test: hello from main after wait\n");
    munmap(stack, stack_size);
}

void *linux_thread_func(void *arg)
{
    assert(arg != nullptr);
    put("linux_thread_start_test: hello from thread\n");
    thread_stack_head *head = (thread_stack_head*)arg;

    assert(((u64*)head->extra_data)[0] == 0x0102030405060708ul);
    sleep_ms(2000);
    put("linux_thread_start_test: hello from thread after sleep\n");

    return arg;
}

void linux_thread_start_test()
{
    /* This test is a bit better, using thread_stack_head to store the clone
       arguments in the threads stack memory, as well as other information
       such as a user function, argument and result, which can be accessed
       as long as the thread stack is not destroyed.

       extra_size may be used to reserve space within the thread stack,
       higher up than the stack head, for arbitrary use.

       Not shown here is the option to set the clone entry point, which here
       is default_clone_entry, which simply sets the result within the head
       to user_function(user_argument), then exits.
    */
    error err{};
    s64 stack_size = 4 * 1024 * 1024;
    s64 extra_size = 4096;

    void *stack = thread_stack_create(stack_size, &err);
    assert(err.error_code == 0);

    thread_stack_head *head = get_thread_stack_head(stack, stack_size, extra_size);
    assert(head != nullptr);
    head->user_function = linux_thread_func;
    head->user_function_argument = (void*)head;
    ((u64*)head->extra_data)[0] = 0x0102030405060708ul;

    assert(head->user_function_result == nullptr);

    put("linux_thread_start_test: hello from main\n");

    sys_int tid = linux_thread_start(head, &err);
    assert(tid > 0);
    assert(err.error_code == 0);

    tprint("linux_thread_start_test: hello from main after thread start, child thread id: %\n", tid);

    bool join_ok = linux_thread_join(head, nullptr, &err);
    assert(join_ok);
    assert(err.error_code == 0);

    put("linux_thread_start_test: hello from main after join\n");

    assert(head->user_function_result == (void*)head);
    thread_stack_destroy(stack, stack_size);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (RUNNING_ON_VALGRIND)
        return 0;

    // clone_test();
    linux_thread_start_test();

    return 0;
}

#else
// non-linux platform

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}
#endif
