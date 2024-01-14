
#include <stdarg.h>
#include <stdio.h>

#include "shl/error.hpp"
#include "shl/ring_buffer.hpp"
#include "shl/at_exit.hpp"

#define ERROR_RING_BUFFER_MIN_SIZE 4096

struct error_buffer
{
    ring_buffer buffer;
    u64 offset;
};

void _error_buffer_cleanup();

error_buffer *_get_error_buffer(bool free_buffer = false)
{
    static error_buffer _buf{};

    if (free_buffer && _buf.buffer.data != nullptr)
    {
        free(&_buf.buffer);
        _buf.offset = 0;
    }
    else if (!free_buffer && _buf.buffer.data == nullptr)
    {
        if (!init(&_buf.buffer, ERROR_RING_BUFFER_MIN_SIZE, 2))
            return nullptr;

        ::register_exit_function(_error_buffer_cleanup);
    }

    return &_buf;
}

void _error_buffer_cleanup()
{
    _get_error_buffer(true);
}

const char *vformat_error_message(const char *format, va_list args)
{
    error_buffer *_buf = _get_error_buffer(false);

    if (_buf == nullptr)
        return nullptr;

    s64 count = ::vsnprintf(_buf->buffer.data + _buf->offset, _buf->buffer.size - 1, format, args);
    char *ret = _buf->buffer.data + _buf->offset;

    _buf->offset += count;
    _buf->buffer.data[_buf->offset] = '\0';

    while (_buf->offset >= _buf->buffer.size)
        _buf->offset -= _buf->buffer.size;

    return ret;
}

const char *format_error_message(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    const char *ret = vformat_error_message(format, argptr);
    va_end(argptr);

    return ret;
}
