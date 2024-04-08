
/* linux/futex.hpp

Linux futex syscalls and constants.
*/

#pragma once

#include "shl/time.hpp"
#include "shl/number_types.hpp"

#define FUTEX_WAKE_ALL S32_MAX

extern "C" sys_int futex(s32 *addr, sys_int futex_op, s32 val, void *val2, s32 *addr2, s32 val3);

sys_int futex_wait(s32 *addr, s32 expected_value, timespan *timeout = nullptr);
sys_int futex_wake(s32 *addr, s32 count = FUTEX_WAKE_ALL);

#ifndef FUTEX_WAIT
#  define FUTEX_WAIT            0
#  define FUTEX_WAKE            1
#  define FUTEX_FD              2
#  define FUTEX_REQUEUE         3
#  define FUTEX_CMP_REQUEUE     4
#  define FUTEX_WAKE_OP         5
#  define FUTEX_LOCK_PI         6
#  define FUTEX_UNLOCK_PI       7
#  define FUTEX_TRYLOCK_PI      8
#  define FUTEX_WAIT_BITSET     9
#  define FUTEX_WAKE_BITSET     10
#  define FUTEX_WAIT_REQUEUE_PI 11
#  define FUTEX_CMP_REQUEUE_PI  12
#  define FUTEX_LOCK_PI2        13

#  define FUTEX_PRIVATE_FLAG    0x80
#  define FUTEX_CLOCK_REALTIME  0x100
#  define FUTEX_CMD_MASK        ~(FUTEX_PRIVATE_FLAG | FUTEX_CLOCK_REALTIME)
#endif
