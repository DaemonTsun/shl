
#include "shl/number_types.hpp"
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/process.hpp"

extern "C" int fork()
{
    return (int)(sys_int)linux_syscall(SYS_fork);
}

extern "C" int execve(char *path, char **argv, char **env)
{
    return (int)(sys_int)linux_syscall3(SYS_execve,
                                        (void*)path,
                                        (void*)argv,
                                        (void*)env);
}

extern "C" int getpid()
{
    return (int)(sys_int)linux_syscall(SYS_getpid);
}

extern "C" int getppid()
{
    return (int)(sys_int)linux_syscall(SYS_getppid);
}

extern "C" int dup(int oldfd)
{
    return (int)(sys_int)linux_syscall1(SYS_dup,
                                        (void*)(sys_int)oldfd);
}

extern "C" int dup2(int oldfd, int newfd)
{
    return (int)(sys_int)linux_syscall2(SYS_dup2,
                                        (void*)(sys_int)oldfd,
                                        (void*)(sys_int)newfd);
}

extern "C" int dup3(int oldfd, int newfd, int flags)
{
    return (int)(sys_int)linux_syscall3(SYS_dup2,
                                        (void*)(sys_int)oldfd,
                                        (void*)(sys_int)newfd,
                                        (void*)(sys_int)flags);
}

