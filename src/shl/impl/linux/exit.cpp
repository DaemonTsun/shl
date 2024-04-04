
#include "shl/number_types.hpp"
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/exit.hpp"

extern "C" void exit(int status)
{
    linux_syscall1(SYS_exit, (void*)(sys_int)status);
    while (1);
}

extern "C" void exit_group(int status)
{
    linux_syscall1(SYS_exit_group, (void*)(sys_int)status);
    while (1);
}
