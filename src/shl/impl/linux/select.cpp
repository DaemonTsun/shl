
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/select.hpp"
#include "shl/time.hpp"
#include "shl/print.hpp"

sys_int select(int fd_count, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, timespan *timeout)
{
    return (sys_int)linux_syscall5(SYS_select,
                                  (void*)(sys_int)fd_count,
                                  (void*)readfds,
                                  (void*)writefds,
                                  (void*)exceptfds,
                                  (void*)timeout);
}

sys_int pselect6(int fd_count, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, timespan *timeout, void *sigmask)
{
    sys_uint data[2] =
    {
        (sys_uint)sigmask, Wordsize / 8
    };

    return (sys_int)linux_syscall6(SYS_pselect6,
                                  (void*)(sys_int)fd_count,
                                  (void*)readfds,
                                  (void*)writefds,
                                  (void*)exceptfds,
                                  (void*)timeout,
                                  (void*)data);
}
