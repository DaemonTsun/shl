
/* select.hpp

Linux system calls 'select' and 'pselect6'.

*/
#pragma once

#include "shl/number_types.hpp"

struct fd_set;
struct timespan; // in shl/time.hpp

extern "C" sys_int select(int fd_count, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, timespan *timeout);
extern "C" sys_int pselect6(int fd_count, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, timespan *timeout, void *sigmask);

typedef long int fd_mask;
#define FD_MaxFDs 1024 
#define FD_Masksize (8 * (int)sizeof(fd_mask))
#define FD_Index(d) ((d) / FD_Masksize)
#define FD_Mask(d) ((fd_mask) (1UL << ((d) % FD_Masksize)))

struct fd_set
{
    fd_mask bits[FD_MaxFDs / FD_Masksize];
};

#define FD_ZERO(s) do \
    {\
        fd_set *__arr = (s);\
        \
        for (unsigned int __i = 0; __i < sizeof(fd_set) / sizeof (fd_mask); ++__i)\
            __arr->bits[__i] = 0;\
    } while (0)

#define FD_SET(d, s)   ((void)((s)->bits[FD_Index(d)] |= FD_Mask(d)))
#define FD_CLR(d, s)   ((void)((s)->bits[FD_Index(d)] &= ~FD_Mask(d)))
#define FD_ISSET(d, s) (((s)->bits[FD_Index(d)] & FD_Mask (d)) != 0)
