
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "shl/error.hpp"

#define MESSAGE_SIZE 2048

// TODO: ring buffer
const char *vformat_error_message(const char *format, va_list args)
{
    static char *_error = nullptr;

    if (_error == nullptr)
        _error = reinterpret_cast<char*>(calloc(MESSAGE_SIZE, sizeof(char)));

    vsnprintf(_error, MESSAGE_SIZE, format, args);
    return _error;
}

const char *format_error_message(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    const char *ret = vformat_error_message(format, argptr);
    va_end(argptr);

    return ret;
}
