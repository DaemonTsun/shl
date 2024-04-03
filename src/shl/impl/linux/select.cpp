
#include "shl/impl/linux/select.hpp"

extern "C" sys_int select(int fd_count, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, timespan *timeout)
{
    return (sys_int)linux_syscall5(SYS_select,
                                  (void*)(sys_int)fd_count,
                                  (void*)readfds,
                                  (void*)writefds,
                                  (void*)exceptfds,
                                  (void*)&timeout);
}
