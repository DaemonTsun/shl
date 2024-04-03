
#pragma once

#include "shl/number_types.hpp"

struct timespan
{
    s64 seconds;
    s64 nanoseconds; // might also be microseconds
};

void get_time(timespan *t);

void get_timespan_difference(const timespan *start, const timespan *end, timespan *out);
double get_seconds_difference(const timespan *start, const timespan *end);

void sleep(const timespan *t);
void sleep(double seconds);
void sleep_ms(int milliseconds);
