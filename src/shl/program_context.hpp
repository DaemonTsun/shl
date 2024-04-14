
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
#include "shl/macros.hpp"
#include "shl/defer.hpp"

struct program_context
{
    s32 thread_id;

    void *user_data;

    ::allocator allocator;
    ::allocator thread_storage_allocator;

    // TODO: logger
};

const program_context default_context{
    .thread_id = 0,
    .user_data = nullptr,
    .allocator = default_allocator,
    .thread_storage_allocator = null_allocator
};

program_context *get_context_pointer();

// returns the previous context pointer
program_context *set_context_pointer(program_context *next);

#ifndef with_context
#  define __with_context(NewContextPtr, Line)\
      if constexpr (program_context *JOIN(_old_ptr, Line) = set_context_pointer(NewContextPtr); true)\
      if constexpr (defer { set_context_pointer(JOIN(_old_ptr, Line)); }; true)
      
#  define _with_context(NewContextPtr, Line) __with_context(NewContextPtr, Line)
#  define with_context(NewContextPtr) _with_context(NewContextPtr, __LINE__)
#endif
    
#ifndef with_allocator
#  define __with_allocator(NewAlloc, Line)\
      if constexpr (program_context *JOIN(_old_ptr, Line) = get_context_pointer(); true)\
      if constexpr (defer { set_context_pointer(JOIN(_old_ptr, Line)); }; true)\
      if constexpr (program_context JOIN(_nctx, Line) = program_context{\
              .thread_id = JOIN(_old_ptr, Line)->thread_id,\
              .user_data = JOIN(_old_ptr, Line)->user_data,\
              .allocator = (NewAlloc),\
              .thread_storage_allocator = JOIN(_old_ptr, Line)->thread_storage_allocator\
          }; true)\
      if constexpr ([[maybe_unused]] program_context *_toss##Line = set_context_pointer(&JOIN(_nctx, Line)); true)

#  define _with_allocator(NewAlloc, Line) __with_allocator(NewAlloc, Line)
#  define with_allocator(NewAlloc) _with_allocator(NewAlloc, __LINE__)
#endif


// utility
#ifndef _set_allocator_if_not_set
#  define _set_allocator_if_not_set(Ptr)\
      if ((Ptr)->allocator.alloc == nullptr)\
      {\
          (Ptr)->allocator = get_context_pointer()->allocator;\
      }
#endif

