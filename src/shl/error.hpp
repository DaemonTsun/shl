
#pragma once

/* error.hpp
 *
 * defines the error struct, a lightweight alternative to std::exception.
 *
 * format_error(fmt, ...) is similar to snprintf, but returns a pointer to the
 * formatted string. the pointer points to static memory and only one message
 * can be formatted at a time.
 *
 * the format_error(fmt, ...) macro throws error with the given format message
 * and the __FILE__ and __LINE__ information.
 */

struct error
{
    const char *what;
    const char *file;
    unsigned long line;
};

const char *format_error(const char *format, ...);


#define MACRO_TO_STRING2(x) #x
#define MACRO_TO_STRING(x) MACRO_TO_STRING2(x)
#define throw_error(FMT, ...) \
    throw error{format_error(FMT __VA_OPT__(,) __VA_ARGS__), __FILE__, __LINE__}
