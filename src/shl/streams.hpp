
/* steams.hpp
 *
 * utility header providing memory and file streams
 */

#pragma once

#include "shl/memory_stream.hpp"
#include "shl/file_stream.hpp"

u64 read_entire_file(const char *path, memory_stream *out);
u64 read_entire_file(file_stream *stream, memory_stream *out);
