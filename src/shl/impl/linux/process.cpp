
#include "shl/number_types.hpp"
#include "shl/impl/linux/process.hpp"
#include "shl/impl/linux/thread.hpp"

#if defined(SYS_fork)
int fork()
{
    return (int)(sys_int)linux_syscall(SYS_fork);
}
#endif

int execve(char *path, char **argv, char **env)
{
    return (int)(sys_int)linux_syscall3(SYS_execve,
                                        (void*)path,
                                        (void*)argv,
                                        (void*)env);
}

int clone_fork()
{
    return (int)(sys_int)linux_syscall5(SYS_clone,
            (void*)(sys_int)(17),
            nullptr,
            nullptr,
            nullptr,
            nullptr);
}

int getpid()
{
    return (int)(sys_int)linux_syscall(SYS_getpid);
}

int getppid()
{
    return (int)(sys_int)linux_syscall(SYS_getppid);
}

