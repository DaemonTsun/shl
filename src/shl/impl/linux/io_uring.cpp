
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/io_uring.hpp"

extern "C"
{
int io_uring_setup(u32 entries, io_uring_params *params)
{
    return (int)(sys_int)linux_syscall2(SYS_io_uring_setup,
        (void*)(sys_int)entries,
        (void*)params);
}

sys_int io_uring_enter(int fd, u32 to_submit, u32 min_complete, int flags, void *args, sys_int arg_count)
{
    return (int)(sys_int)linux_syscall6(SYS_io_uring_enter,
        (void*)(sys_int)fd,
        (void*)(sys_int)to_submit,
        (void*)(sys_int)min_complete,
        (void*)(sys_int)flags,
        (void*)args,
        (void*)arg_count);
}

sys_int io_uring_register(int fd, int opcode, void *args, sys_int arg_count)
{
    return (int)(sys_int)linux_syscall4(SYS_io_uring_register,
        (void*)(sys_int)fd,
        (void*)(sys_int)opcode,
        (void*)args,
        (void*)arg_count);
}

} // extern C
