
#pragma once

// TODO: docs

#include "shl/number_types.hpp"
#include "shl/error.hpp"

struct ring_buffer
{
    char *data;
    u64 size;
    u32 mapping_count; // how many times the memory is mapped
};

bool init(ring_buffer *buf, u64 min_size, u32 mapping_count = 3, error *err = nullptr);
bool free(ring_buffer *buf, error *err = nullptr);
