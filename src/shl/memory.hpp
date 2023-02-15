
#pragma once

#include "shl/number_types.hpp"

void *allocate_memory(u64 size);
void *reallocate_memory(void *ptr, u64 size);
void free_memory(void *ptr);
