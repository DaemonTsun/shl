
#pragma once

/* ring_buffer.hpp

Contiguous, fixed-size, cyclic data structure.

A ring buffer is a buffer which repeats in virtual memory, e.g. if a ring
buffer has size 8, accessing the 8th index (out of bounds in e.g. an array)
yields the first element of the buffer.

The data field in the ring_buffer structure is the pointer to the first mapping
of the ring buffer.
The size field in the ring_buffer is the (actual) size of each mapping, i.e.
ring_buffer.data[ring_buffer.size] accesses the first element of the ring
buffer.


Functions:

init(*buf, min_size, mapping_count, [*err])
    initializes a ring_buffer which is at least min_size bytes long, with mapping_count
    total mappings (1 mapping would be a normal array, i.e. useless). The mappings are
    contiguous, i.e. right next to each other in virtual memory.
    On failure, returns false and optionally writes the error(code) to err,
    otherwise returns true.

free(*buf, [*err])
    frees the ring_buffer buf.
    On failure, returns false and optionally writes the error(code) to err,
    otherwise returns true.


Example:

    ring_buffer buf{};
    error err{};

    if (!init(&buf, 512, 2, &err))
    {
        // error handling...
    }

    buf.data[0] = 'a';

    // buf.data[0] and buf.data[buf.size] are now 'a'.
    // Do not use the min_size (512 in this case) as the buffer size,
    // buf.size will be set by init to the first multiple of the systems
    // pagesize that is equal to or greater than min_size, and it will
    // be the actual size of the ring buffer.

*/

#include "shl/number_types.hpp"
#include "shl/error.hpp"

struct ring_buffer
{
    char *data;
    s64 size;
    s32 mapping_count; // how many times the memory is mapped
};

bool init(ring_buffer *buf, s64 min_size, s32 mapping_count = 3, error *err = nullptr);
bool free(ring_buffer *buf, error *err = nullptr);

bool resize(ring_buffer *buf, s64 min_size, s32 mapping_count = 3, error *err = nullptr);

s64 get_system_pagesize();
