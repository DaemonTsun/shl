
#pragma once

/* program_context.hpp

Defines the program_context struct, along with functions to access, modify and
set the program context.
*/

#include "shl/allocator.hpp"

struct program_context
{
    ::allocator allocator;

    // TODO: logger
    // s32 thread_id;
};

const program_context default_context{.allocator = default_allocator};

program_context *get_context_pointer();

// returns the previous context pointer
program_context *set_context_pointer(program_context *next);
