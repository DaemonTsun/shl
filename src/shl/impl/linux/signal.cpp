
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/signal.hpp"

extern "C" sys_int kill(int pid, int status)
{
    return (sys_int)linux_syscall2(SYS_kill,
                                   (void*)(sys_int)pid,
                                   (void*)(sys_int)status);
}
