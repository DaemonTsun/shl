
#include "shl/impl/linux/poll.hpp"
#include "shl/impl/linux/syscalls.hpp"

extern "C" sys_int poll(poll_fd *fds, int count, int timeout_milliseconds)
{
    return (sys_int)linux_syscall3(SYS_poll,
                                   (void*)fds,
                                   (void*)(sys_int)count,
                                   (void*)(sys_int)timeout_milliseconds);
}

