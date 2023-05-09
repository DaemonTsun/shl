
#pragma once

struct timespan
{
    long seconds;
    long nanoseconds;
};

void get_time(timespan *t);

void get_timespan_difference(const timespan *start, const timespan *end, timespan *out);
double get_seconds_difference(const timespan *start, const timespan *end);

void sleep(const timespan *t);
void sleep(double seconds);
void sleep_ms(int milliseconds);
