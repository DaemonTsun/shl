
/* steams.hpp

utility header providing memory and file streams

read_entire_file(path or file_stream, memory_stream) reads an entire file
and returns the size of the file read.
 */

#pragma once

#include "shl/memory_stream.hpp"
#include "shl/file_stream.hpp"
#include "shl/string.hpp"
#include "shl/error.hpp"

bool read_entire_file(const char *path, memory_stream *out, error *err = nullptr);
bool read_entire_file(file_stream *stream, memory_stream *out, error *err = nullptr);
bool read_entire_file(const char *path, string *out, error *err = nullptr);
bool read_entire_file(file_stream *stream, string *out, error *err = nullptr);
