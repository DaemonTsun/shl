
#pragma once

struct error
{
    error();
    error(const char *what);
    error(const char *fmt, ...);

    const char *what;
};

const char *format_error(const char *format, ...);
