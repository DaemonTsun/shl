
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

s64 read(pipe *p, char *buf, u64 size, error *err);
s64 write(pipe *p, const char *buf, u64 size, error *err);
