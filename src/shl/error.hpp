
#pragma once

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
