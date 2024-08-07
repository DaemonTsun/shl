
#include "shl/number_types.hpp"
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/process.hpp"

int fork()
{
    return (int)(sys_int)linux_syscall(SYS_fork);
}

int execve(char *path, char **argv, char **env)
{
    return (int)(sys_int)linux_syscall3(SYS_execve,
                                        (void*)path,
                                        (void*)argv,
                                        (void*)env);
}

int getpid()
{
    return (int)(sys_int)linux_syscall(SYS_getpid);
}

int getppid()
{
    return (int)(sys_int)linux_syscall(SYS_getppid);
}

