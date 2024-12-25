
#include <stdarg.h>
#include <stddef.h> // size_t

#include "shl/platform.hpp"
#include "shl/error.hpp"
#include "shl/defer.hpp"
#include "shl/memory.hpp"
#include "shl/ring_buffer.hpp"
#include "shl/exit.hpp"

#if Windows
#include <windows.h>
#endif

#define ERROR_RING_BUFFER_MIN_SIZE 4096
#define ERROR_TRACES_COUNT          128

struct error_buffer
{
    ring_buffer buffer;
    s64 offset;

#ifndef NDEBUG
    s64 next_trace_index;
    error traces[ERROR_TRACES_COUNT];
#endif
};

static error_buffer *_get_error_buffer()
{
    static error_buffer _buf{};

    static_exec  {
        fill_memory(&_buf, 0);
        init(&_buf.buffer, ERROR_RING_BUFFER_MIN_SIZE, 2);
    };
    static_defer { free(&_buf.buffer); };

    return &_buf;
}

// TODO: get rid of this
extern "C"
{
char *strerror(int code);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
}

const char *errno_error_message(int error_code)
{
    return strerror(error_code);
}

const char *windows_error_message(int error_code)
{
#if Windows
    error_buffer *_buf = _get_error_buffer();

    if (_buf == nullptr)
        return nullptr;

    int count = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL,
                               error_code,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               _buf->buffer.data + _buf->offset,
                               (u32)_buf->buffer.size - 1, NULL);

    char *ret = _buf->buffer.data + _buf->offset;

    _buf->offset += count;
    _buf->buffer.data[_buf->offset] = '\0';
    _buf->offset = (_buf->offset + 1) & (_buf->buffer.size - 1);

    return ret;
#else
    // Nothing on other platforms.
    // Use _errno_error_message(error_code) to get a errno error message.
    (void)(error_code);
    return nullptr;
#endif
}

static const char *_vformat_error_message(const char *format, va_list args)
{
    error_buffer *_buf = _get_error_buffer();

    if (_buf == nullptr)
        return nullptr;

    s64 count = ::vsnprintf(_buf->buffer.data + _buf->offset, _buf->buffer.size - 1, format, args);
    char *ret = _buf->buffer.data + _buf->offset;

    _buf->offset += count;
    _buf->buffer.data[_buf->offset] = '\0';
    _buf->offset = (_buf->offset + 1) & (_buf->buffer.size - 1);

    return ret;
}

const char *format_error_message(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    const char *ret = _vformat_error_message(format, argptr);
    va_end(argptr);

    return ret;
}

#ifndef NDEBUG
// this is only defined in debug builds
static error *_get_next_error_trace()
{
    error_buffer *buf = _get_error_buffer();

    error *ret = buf->traces + buf->next_trace_index;
    buf->next_trace_index = (buf->next_trace_index + 1) & (ERROR_TRACES_COUNT - 1);

    return ret;
}

// this is only defined in debug builds
void _set_error(error *err, int error_code, const char *message,
                const char *file, const char *function, unsigned long line)
{
    if (err == nullptr)
        return;

    error *next = _get_next_error_trace();

    if (err->trace != nullptr)
    {
        error *last = (error*)err->trace;

        int i = 0;

        while (last->trace != nullptr && i < ERROR_TRACES_COUNT)
        {
            last = (error*)last->trace;
            i += 1;
        }

        if (i >= ERROR_TRACES_COUNT)
            last->trace = nullptr;

        last->trace = next;
    }

    fill_memory(next, 0);
    next->error_code = error_code;
    next->what = message;
    next->file = file;
    next->function = function;
    next->line = line;
    next->trace = nullptr;

    if (err->trace == nullptr)
    {
        *err = *next;
        err->trace = next;
    }
}
#endif
