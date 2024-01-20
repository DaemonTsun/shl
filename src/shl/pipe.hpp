
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
bool init(pipe *p, u64 preferred_size, bool inherit, error *err = nullptr);

bool free(pipe *p, error *err = nullptr);
