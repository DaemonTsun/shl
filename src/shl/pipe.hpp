
#pragma once

/* pipe.hpp

Pipes. Use io.hpp functions with the read and write io handles of the pipe.

Functions:

init(*pipe_t[, *err]) initializes a new pipe with a read and write io_handle.
free(*pipe_t[, *err]) destroys the pipe and its io_handles.
*/

#include "shl/error.hpp"
#include "shl/io.hpp"
#include "shl/number_types.hpp"

struct pipe_t
{
    io_handle read;
    io_handle write;
};

bool init(pipe_t *p, error *err = nullptr);

// on Windows, flags is the "preferred size" for the pipe
// on Linux, it's the flags for pipe2
bool init(pipe_t *p, int flags, bool inherit, error *err = nullptr);

bool free(pipe_t *p, error *err = nullptr);
