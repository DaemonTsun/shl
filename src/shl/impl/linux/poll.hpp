
#pragma once

#include "shl/number_types.hpp"
#include "shl/impl/linux/syscalls.hpp"

#ifndef POLLIN
#  define POLLIN        0x0001 
#  define POLLPRI       0x0002
#  define POLLOUT       0x0004
#  define POLLERR       0x0008
#  define POLLHUP       0x0010
#  define POLLNVAL      0x0020
#  define POLLRDNORM    0x0040
#  define POLLRDBAND    0x0080
#  define POLLWRNORM    0x0100
#  define POLLWRBAND    0x0200
#  define POLLMSG       0x0400
#  define POLLREMOVE    0x1000
#  define POLLRDHUP     0x2000
#endif

struct poll_fd
{
    int fd;
    short events;
    short returned_events;
};

#if defined(SYS_poll)
sys_int poll(poll_fd *fds, int count, int timeout_milliseconds);
#endif
