
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/memory.hpp"

extern "C" void *mmap(void *address, sys_int length, sys_int protection, sys_int flags)
{
    return linux_syscall6(SYS_mmap,
                          address,
                          (void*)length,
                          (void*)protection,
                          (void*)flags,
                          (void*)(sys_int)0,
                          (void*)(sys_int)0
                          );
}

extern "C" void *fmmap(void *address, sys_int length, sys_int protection, sys_int flags, int fd, sys_int offset)
{
    return linux_syscall6(SYS_mmap,
                          address,
                          (void*)length,
                          (void*)protection,
                          (void*)flags,
                          (void*)(sys_int)fd,
                          (void*)offset
                          );
}

extern "C" sys_int munmap(void *address, sys_int length)
{
    return (sys_int)linux_syscall2(SYS_munmap,
                                   address,
                                   (void*)length);
}
