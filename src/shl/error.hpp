
#pragma once

/* error.hpp

Defines the error struct, a lightweight alternative to std::exception.

format_error_message(fmt, ...) is similar to snprintf, but returns a pointer to the
formatted string. The pointer points to static memory and only one message
can be formatted at a time.

The throw_error(fmt, ...) macro throws error with the given format message
and the __FILE__ and __LINE__ information.

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
    const char *what;

#ifndef NDEBUG
    const char    *file;
    unsigned long  line;
#endif
};

const char *format_error_message(const char *format, ...);

#ifndef NDEBUG
#define throw_error(FMT, ...) \
    throw error{.what = format_error_message(FMT __VA_OPT__(,) __VA_ARGS__), .file = __FILE__, .line = __LINE__}

#define set_error(ERR, MSG) \
    do { if ((ERR) != nullptr) { *(ERR) = error{.what = MSG, .file = __FILE__, .line = __LINE__}; } } while (0)

#else
// release

#define throw_error(FMT, ...) \
    throw error{.what = format_error_message(FMT __VA_OPT__(,) __VA_ARGS__)}

#define set_error(ERR, MSG) \
    do { if ((ERR) != nullptr) { ERR->what = MSG; } } while (0)

#endif

#define format_error(ERR, FMT, ...) \
    set_error(ERR, format_error_message(FMT __VA_OPT__(,) __VA_ARGS__))
