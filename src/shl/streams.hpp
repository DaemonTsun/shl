
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
s64 _write(pipe *p, const_string  s, error *err);
s64 _write(pipe *p, const_wstring s, error *err);

template<typename T> auto write(file_stream *stream, T str, error *err = nullptr)    _io_const_string_body(_write, stream, str, err)
template<typename T> auto write(memory_stream *stream, T str, error *err = nullptr)  _io_const_string_body(_write, stream, str, err)
template<typename T> auto write(io_handle h, T str, error *err = nullptr)            _io_const_string_body(_write, h, str, err)
template<typename T> auto write(pipe *p, T str, error *err = nullptr)                _io_const_string_body(_write, p, str, err)
