
#pragma once

#include "shl/error.hpp"
#include "shl/io.hpp"
#include "shl/number_types.hpp"

struct pipe
{
    io_handle read;
    io_handle write;
};

bool init(pipe *p, error *err = nullptr);

// on Windows, flags is the "preferred size" for the pipe
// on Linux, it's the flags for pipe2
bool init(pipe *p, int flags, bool inherit, error *err = nullptr);

bool free(pipe *p, error *err = nullptr);
