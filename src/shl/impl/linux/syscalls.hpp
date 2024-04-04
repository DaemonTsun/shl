
/* syscalls.hpp

Linux system call IDs.
*/

#pragma once

#include "shl/platform.hpp"

#if Linux

#  include "shl/architecture.hpp"

#  if   Architecture == ARCH_x86
#    include "shl/impl/linux/x86/syscalls.h"
#  elif Architecture == ARCH_x86_64
#    include "shl/impl/linux/x86_64/syscalls.h"
#  elif Architecture == ARCH_aarch64
#    include "shl/impl/linux/aarch64/syscalls.h"
#  endif

#  include "shl/number_types.hpp"

extern "C"
{
void *linux_syscall (sys_int number);
void *linux_syscall1(sys_int number, void *arg1);
void *linux_syscall2(sys_int number, void *arg1, void *arg2);
void *linux_syscall3(sys_int number, void *arg1, void *arg2, void *arg3);
void *linux_syscall4(sys_int number, void *arg1, void *arg2, void *arg3, void *arg4);
void *linux_syscall5(sys_int number, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);
void *linux_syscall6(sys_int number, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6);
}

#endif // Linux
