
#pragma once

/* error.hpp

Defines the error struct, a lightweight alternative to std::exception.

format_error_message(fmt, ...)
    Similar to snprintf, but returns a pointer to the formatted string.
    The pointer points to static ring buffer memory which may be overwritten
    with newer error messages.

set_error(*err, errcode, message)
    Writes the given error code and message to *err.
    In debug, file and line information is also set in *err.
    Does nothing if *err is nullptr.

format_error(*err, errcode, fmt, ...)
    Same as set_error, but formats the error message using format_error_message.

A typical pattern for this is to have an optional *error parameter in a function
and set the error if it's not nullptr (set_error and format_error check for nullptr),
e.g.:

bool do_thing(int x, int y, error *err = nullptr)
{
    if (x <= y)
    {
        format_error(err, "x (%d) must be greater than y (%d)", x, y);
        return false;
    }

    ...
}
*/

struct error
{
    int error_code;
    const char *what;

#ifndef NDEBUG
    const char    *file;
    unsigned long  line;
#endif
};

const char *format_error_message(const char *format, ...);

const char *_errno_error_message(int errcode);
const char *_windows_error_message(int errcode);

#ifndef NDEBUG
#define set_error(Err, Code, Msg) \
    do { if ((Err) != nullptr) { *(Err) = ::error{.error_code = Code, .what = Msg, .file = __FILE__, .line = __LINE__}; } } while (0)

#define set_errno_error(Err) \
    do { if ((Err) != nullptr) { int _errcode = errno; *(Err) = ::error{.error_code = _errcode, .what = ::_errno_error_message(_errcode), .file = __FILE__, .line = __LINE__}; } } while (0)

#define set_GetLastError_error(Err) \
    do { if ((Err) != nullptr) { int _errcode = (int)GetLastError(); *(Err) = ::error{.error_code = _errcode, .what = _windows_error_message(_errcode), .file = __FILE__, .line = __LINE__}; } } while (0)

#  if Windows
#define set_error_by_code(Err, Code) \
    do { if ((Err) != nullptr) { int _errcode = (int)(Code); *(Err) = ::error{.error_code = _errcode, .what = _windows_error_message(_errcode), .file = __FILE__, .line = __LINE__}; } } while (0)
#  else // Linux
#define set_error_by_code(Err, Code) \
    do { if ((Err) != nullptr) { int _errcode = (int)(Code); *(Err) = ::error{.error_code = _errcode, .what = ::_errno_error_message(_errcode), .file = __FILE__, .line = __LINE__}; } } while (0)
#  endif

#else
// release
#define set_error(Err, Code, Msg) \
    do { if ((Err) != nullptr) { *(Err) = ::error{.error_code = Code, .what = Msg }; }} while (0)

#define set_errno_error(Err) \
    do { if ((Err) != nullptr) { int _errcode = errno; *(Err) = ::error{.error_code = _errcode, .what = ::_errno_error_message(_errcode) }; } } while (0)

#define set_GetLastError_error(Err) \
    do { if ((Err) != nullptr) { int _errcode = (int)GetLastError(); *(Err) = ::error{.error_code = _errcode, .what = _windows_error_message(_errcode) }; } } while (0)

#  if Windows
#define set_error_by_code(Err, Code) \
    do { if ((Err) != nullptr) { int _errcode = (int)(Code); *(Err) = ::error{.error_code = _errcode, .what = _windows_error_message(_errcode) }; } } while (0)
#  else // Linux
#define set_error_by_code(Err, Code) \
    do { if ((Err) != nullptr) { int _errcode = (int)(Code); *(Err) = ::error{.error_code = _errcode, .what = ::_errno_error_message(_errcode) }; } } while (0)
#  endif

#endif

#define format_error(Err, Code, FMT, ...) \
    set_error(Err, Code, format_error_message(FMT __VA_OPT__(,) __VA_ARGS__))
