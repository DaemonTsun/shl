
#include "shl/platform.hpp"

#if Linux
#define ASSERT_PRINT_MESSAGE 1
#include "shl/assert.hpp"
#include "shl/impl/linux/thread.hpp"
#include "shl/print.hpp"
#include "shl/time.hpp"
#include <stdio.h>

void *thread_func(void *arg)
{
    put("hello from thread\n");

    return arg;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    error err{};


    s64 stack_size = 4 * 1024 * 1024;
    void *stack = thread_stack_create(stack_size, &err);
    assert(stack != nullptr);
    assert(err.error_code == 0);

    thread_stack_head *h = get_thread_stack_head(stack, stack_size);
    assert(h != nullptr);
    h->entry = thread_func;
    h->argument = nullptr;

    put("hello from main\n");
    printf("%lx, %p, %p\n", *(sys_int*)stack, (void*)h->setup, (void*)default_clone_entry);
    printf("stack: %p, h: %p\n", (void*)stack, (void*)h);

    sys_int ret = linux_thread_start(h, &err);
    assert(err.error_code == 0);
    assert(ret != 0);

    put("hello from main after thread start\n");

    sleep_ms(10000);

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
