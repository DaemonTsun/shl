
/* linux/memory.hpp

Linux mmap and other memory relevant syscalls and constants.
*/

#pragma once

#include "shl/number_types.hpp"

void *mmap(void *address, sys_int length, sys_int protection, sys_int flags);
void *mmap(void *address, sys_int length, sys_int protection, sys_int flags, int fd, sys_int offset);

#define MMAP_IS_ERROR(Addr) ((u64)(Addr) == 0 || (u64)(Addr) > -4096ul)

sys_int munmap(void *address, sys_int length);

#ifndef PROT_READ
#  define PROT_NONE       0x0
#  define PROT_READ       0x1
#  define PROT_WRITE      0x2
#  define PROT_EXEC       0x4
#endif

#ifndef MAP_SHARED
#  define MAP_SHARED    0x01
#  define MAP_PRIVATE   0x02
#  define MAP_FIXED     0x10
#  define MAP_ANONYMOUS 0x20

#  define MAP_GROWSDOWN     0x00100
#  define MAP_DENYWRITE     0x00800
#  define MAP_EXECUTABLE    0x01000
#  define MAP_LOCKED        0x02000
#  define MAP_NORESERVE     0x04000
#  define MAP_POPULATE      0x08000
#  define MAP_NONBLOCK      0x10000
#  define MAP_STACK         0x20000
#  define MAP_HUGETLB       0x40000
#  define MAP_SYNC          0x80000
#endif
