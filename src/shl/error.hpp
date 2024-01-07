
#pragma once

/* error.hpp

Defines the error struct, a lightweight alternative to std::exception.

format_error_message(fmt, ...) is similar to snprintf, but returns a pointer to the
formatted string. The pointer points to static memory and only one message
can be formatted at a time.

The set_error(*err, message) macro writes the given error message to *err.
In debug, file and line information is also set in *err.
Does nothing if *err is nullptr.

format_error(*err, fmt, ...) is the same as set_error, but formats the error
message using format_error_message.

A typical use-case for this is to have an optional *error parameter in a function
and set the error if it's set, e.g.:

bool do_thing(int x, int y, error *err)
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

#ifndef NDEBUG
#define set_error(Err, Code, Msg) \
    do { if ((Err) != nullptr) { *(Err) = ::error{.error_code = Code, .what = Msg, .file = __FILE__, .line = __LINE__}; } } while (0)

#define set_errno_error(Err) \
    do { if ((Err) != nullptr) { int _errcode = errno; *(Err) = ::error{.error_code = _errcode, .what = ::strerror(_errcode), .file = __FILE__, .line = __LINE__}; } } while (0)

#else
// release
#define set_error(Err, Code, Msg) \
    do { if ((Err) != nullptr) { *(Err) = ::error{.error_code = Code, .what = Msg }; } while (0)

#define set_errno_error(Err) \
    do { if ((Err) != nullptr) { int _errcode = errno; *(Err) = ::error{.error_code = _errcode, .what = ::strerror(_errcode) }; } } while (0)

#endif

#define format_error(Err, Code, FMT, ...) \
    set_error(Err, Code, format_error_message(FMT __VA_OPT__(,) __VA_ARGS__))
