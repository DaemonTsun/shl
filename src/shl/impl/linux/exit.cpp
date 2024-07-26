
#include "shl/number_types.hpp"
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/exit.hpp"

void exit(int status)
{
    linux_syscall1(SYS_exit, (void*)(sys_int)status);
    __builtin_unreachable();
}

void exit_group(int status)
{
    linux_syscall1(SYS_exit_group, (void*)(sys_int)status);
    __builtin_unreachable();
}
