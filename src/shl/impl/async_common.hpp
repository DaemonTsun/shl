
/*
Common definitions for async.
*/
#pragma once

// How many asynchronous tasks can be set up in the thread-local context.
// On Windows, should be a multiple of MAXIMUM_WAIT_OBJECTS.
// On Linux, may not be more than 4096 (see io_uring paper)
#define ASYNC_ENTRIES 64 
#define ASYNC_ENTRIES_MASK (ASYNC_ENTRIES - 1)

#define ASYNC_STATUS_READY   0 // Task can be set up
#define ASYNC_STATUS_SETUP   1 // Task is set up but not started (do not overwrite)
#define ASYNC_STATUS_RUNNING 2 // Task is running and may be done
#define ASYNC_STATUS_DONE    3 // Task is done and can be reset to ready

struct async_task
{
    s64 result;

    struct
    {
        u32 status      : 2;
        u32 _reserved   : 18;
        u32 index       : 12;
    };

    u32 _reserved2;
};

