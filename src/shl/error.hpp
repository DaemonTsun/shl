
#pragma once

struct error
{
    const char *what;
};

const char *format_error(const char *format, ...);


#define MACRO_TO_STRING(x) #x
#define throw_error(FMT, ...) \
    throw error{format_error(__FILE__ " " MACRO_TO_STRING(__LINE__) ": " FMT __VA_OPT__(,) __VA_ARGS__)}
