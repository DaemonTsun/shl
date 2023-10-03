
#pragma once

/* error.hpp

Defines the error struct, a lightweight alternative to std::exception.

format_error(fmt, ...) is similar to snprintf, but returns a pointer to the
formatted string. The pointer points to static memory and only one message
can be formatted at a time.

The throw_error(fmt, ...) macro throws error with the given format message
and the __FILE__ and __LINE__ information.

The get_error(*err, fmt, ...) macro writes the given error message to *err.
Does nothing if *err is nullptr.
A typical use-case for this is to have an optional *error parameter in a function
and set the error if it's set, e.g.:

bool do_thing(int x, int y, error *err)
{
    if (x <= y)
    {
        get_error(err, "x (%d) must be greater than y (%d)", x, y);
        return false;
    }

    ...
}
*/

struct error
{
    const char *what;
    const char *file;
    unsigned long line;
};

const char *format_error(const char *format, ...);

#define throw_error(FMT, ...) \
    throw error{.what = format_error(FMT __VA_OPT__(,) __VA_ARGS__), .file = __FILE__, .line = __LINE__}

#define get_error(ERR, FMT, ...) \
    do { if (ERR != nullptr) { *ERR = error{.what = format_error(FMT __VA_OPT__(,) __VA_ARGS__), .file = __FILE__, .line = __LINE__}; } } while (0)
