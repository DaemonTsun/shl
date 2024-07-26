
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/futex.hpp"

sys_int futex(s32 *addr, sys_int futex_op, s32 val, void *val2, s32 *addr2, s32 val3)
{
    return (sys_int)linux_syscall6(SYS_futex,
                                   (void*)addr,
                                   (void*)futex_op,
                                   (void*)(sys_int)val,
                                   (void*)val2,
                                   (void*)addr2,
                                   (void*)(sys_int)val3);
}

sys_int futex_wait(s32 *addr, s32 expected_value, timespan *timeout)
{
    return futex(addr, FUTEX_WAIT, expected_value, (void*)timeout, 0, 0);
}

sys_int futex_wake(s32 *addr, s32 count)
{
    return futex(addr, FUTEX_WAKE, count, nullptr, 0, 0);
}
