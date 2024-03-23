
#pragma once

/* program_context.hpp

Defines the program_context struct, along with functions to access, modify and
set the program context.

program_context contains the (thread global) allocator, (future) logger and thread
and user-supplied information.

By default, the program context is set to the default program_context which uses
the default allocator (see shl/allocator.hpp).

Context or just the allocator may be changed for a block of code using the
with_context and with_allocator macros.
Example:

    void *data1 = alloc(64); // uses default alloc

    arena a;
    ...

    with_allocator(arena_allocator(&a))
    {
        void *data2 = alloc(128); // uses arena 'a'

        // no need to free data2 since arena 'a' will be deallocated all at once
    }

    free(&a);
    dealloc(data1, 64);

get_context_pointer()
    Returns a pointer to the current program_context.
    If no context pointer was set, a default context is returned.

set_context_pointer(*ctx)
    Sets the thread current context pointer to ctx and returns the pointer
    to the old context.

*/

#include "shl/allocator.hpp"
#include "shl/defer.hpp"

struct program_context
{
    s32 thread_id;

    void *user_data;

    ::allocator allocator;

    // TODO: logger
};

const program_context default_context{
    .thread_id = 0,
    .user_data = nullptr,
    .allocator = default_allocator
};

program_context *get_context_pointer();

// returns the previous context pointer
program_context *set_context_pointer(program_context *next);

#define _with_context(NewContextPtr, Line)\
    if constexpr (program_context *_old_ptr##Line = set_context_pointer(NewContextPtr);\
                  defer { set_context_pointer(_old_ptr##Line); })\
    
#define with_context(NewContextPtr)\
    _with_context(NewContextPtr, __LINE__)
    

#define _with_allocator(NewAlloc, Line)\
    if constexpr (program_context *_old_ptr##Line = get_context_pointer();\
                    defer { set_context_pointer(_old_ptr##Line); })\
    if constexpr (program_context _nctx##Line = program_context{\
            .thread_id = _old_ptr##Line->thread_id,\
            .user_data = _old_ptr##Line->user_data,\
            .allocator = (NewAlloc)\
        }; true)\
    if constexpr ([[maybe_unused]] program_context *_toss##Line = set_context_pointer(&_nctx##Line); true)

#define with_allocator(NewAlloc)\
    _with_allocator(NewAlloc, __LINE__)

