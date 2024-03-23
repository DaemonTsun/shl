
#pragma once

/* program_context.hpp

Defines the program_context struct, along with functions to access, modify and
set the program context.
*/

#include "shl/allocator.hpp"

struct program_context
{
    // s32 thread_id;

    ::allocator allocator;

    // TODO: logger
};

const program_context default_context{.allocator = default_allocator};

program_context *get_context_pointer();
program_context *set_context_pointer(program_context *next);
