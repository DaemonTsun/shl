
#include "shl/platform.hpp"

#if Linux && !defined(NDEBUG)
#define ASSERT_PRINT_MESSAGE 1
#include "shl/assert.hpp"
#include "shl/impl/linux/syscalls.hpp"
#include "shl/print.hpp"

void linux_syscall_test()
{
    linux_syscall3(SYS_write, 0, (void*)"hello world\n", (void*)(sys_int)12);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    linux_syscall_test();

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
