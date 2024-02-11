
#pragma once

/* steams.hpp

Utility header providing memory stream, file stream, pipe and generic
IO functionality and functions that work with multiple streams.

Functions:

read_entire_file(path or file_stream, memory_stream or string[, err])
    reads an entire file and returns whether the read was successful or not.

read_entire_io(handle, memory_stream or string[, err])
    reads the entire content of an IO handle and returns whether the read
    was successful or not.

read_entire_pipe(pipe, memory_stream or string[, err])
    reads the entire content of the read end of a pipe and returns whether
    the read was successful or not.

write(stream or io or pipe, String) writes the string String to the stream
    or io or pipe, whichever was passed, and returns the number of
    bytes written.
    String may be anything that can be converted to a const_string_base.
*/

#include "shl/io.hpp"
#include "shl/pipe.hpp"
#include "shl/memory_stream.hpp"
#include "shl/file_stream.hpp"
#include "shl/string.hpp"
#include "shl/error.hpp"

bool read_entire_file(const char *path, memory_stream *out, error *err = nullptr);
bool read_entire_file(const wchar_t *path, memory_stream *out, error *err = nullptr);
bool read_entire_file(file_stream *stream, memory_stream *out, error *err = nullptr);
bool read_entire_file(const char *path, string *out, error *err = nullptr);
bool read_entire_file(const wchar_t *path, string *out, error *err = nullptr);
bool read_entire_file(file_stream *stream, string *out, error *err = nullptr);

// these only return false on error, not if nothing was read, e.g. if at
// the end of a stream.
bool read_entire_io(io_handle h, memory_stream *out, error *err = nullptr);
bool read_entire_io(io_handle h, string *out, error *err = nullptr);

bool read_entire_pipe(pipe_t *p, memory_stream *out, error *err = nullptr);
bool read_entire_pipe(pipe_t *p, string *out, error *err = nullptr);

#define _io_const_string_body(Func, Handle, Str, Err)\
    -> decltype(Func(Handle, to_const_string(Str), Err))\
{\
    return Func(Handle, to_const_string(Str), Err);\
}

s64 _write(file_stream *stream, const_string  s, error *err);
s64 _write(file_stream *stream, const_wstring s, error *err);
s64 _write(memory_stream *stream, const_string  s, error *err);
s64 _write(memory_stream *stream, const_wstring s, error *err);
s64 _write(io_handle h, const_string  s, error *err);
s64 _write(io_handle h, const_wstring s, error *err);
s64 _write(pipe_t *p, const_string  s, error *err);
s64 _write(pipe_t *p, const_wstring s, error *err);

template<typename T> auto write(file_stream *stream, T str, error *err = nullptr)    _io_const_string_body(_write, stream, str, err)
template<typename T> auto write(memory_stream *stream, T str, error *err = nullptr)  _io_const_string_body(_write, stream, str, err)
template<typename T> auto write(io_handle h, T str, error *err = nullptr)            _io_const_string_body(_write, h, str, err)
template<typename T> auto write(pipe_t *p, T str, error *err = nullptr)                _io_const_string_body(_write, p, str, err)
