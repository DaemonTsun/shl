
#pragma once

struct error
{
    const char *what;

    error();
    error(const char *what);
    error(const char *fmt, ...);
};

const char *format_error(const char *format, ...);

#define throw_error(FMT, ...)\
    throw error(__FILE__ " " #__LINE__ ": " FMT __VA_OPT__(,) __VA_ARGS__)
