
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/statx.hpp"

sys_int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *out)
{
    return (sys_int)linux_syscall5(SYS_statx,
                                  (void*)(sys_int)dirfd,
                                  (void*)pathname,
                                  (void*)(sys_int)flags,
                                  (void*)(sys_uint)mask,
                                  (void*)out);
}
