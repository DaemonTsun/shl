
#pragma once

/* steams.hpp

utility header providing memory and file streams

read_entire_file(path or file_stream, memory_stream) reads an entire file
and returns the size of the file read.
 */

#include "shl/io.hpp"
#include "shl/pipe.hpp"
#include "shl/memory_stream.hpp"
#include "shl/file_stream.hpp"
#include "shl/string.hpp"
#include "shl/error.hpp"

bool read_entire_file(const char *path, memory_stream *out, error *err = nullptr);
bool read_entire_file(file_stream *stream, memory_stream *out, error *err = nullptr);
bool read_entire_file(const char *path, string *out, error *err = nullptr);
bool read_entire_file(file_stream *stream, string *out, error *err = nullptr);

// these only return false on error, not if nothing was read, e.g. if at
// the end of a stream.
bool read_entire_io(io_handle h, memory_stream *out, error *err = nullptr);
bool read_entire_io(io_handle h, string *out, error *err = nullptr);

bool read_entire_pipe(pipe *p, memory_stream *out, error *err = nullptr);
bool read_entire_pipe(pipe *p, string *out, error *err = nullptr);

