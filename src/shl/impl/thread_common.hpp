
#pragma once

#ifndef THREAD_STATE_READY
#  define THREAD_STATE_READY      0x00
#  define THREAD_STATE_RUNNING    0x01
#  define THREAD_STATE_STOPPED    0x02
#endif

#ifndef THREAD_STACKSIZE_DEFAULT
#  define THREAD_STACKSIZE_DEFAULT         0x00300000
#  define THREAD_STACKSIZE_STORAGE_DEFAULT 0x00100000
#endif

typedef void *(*thread_entry)(void *argument);
