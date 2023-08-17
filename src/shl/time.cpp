
#include <assert.h>

#include "shl/platform.hpp"

#if Windows
#include <windows.h>
#elif Linux
#include <unistd.h>
#endif

#include <time.h>
#include "shl/time.hpp"

#define NANOSECONDS_IN_A_SECOND 1000000000l

void get_time(timespan *t)
{
    timespec _t;
	timespec_get(&_t, TIME_UTC);
    t->seconds = _t.tv_sec;
    t->nanoseconds = _t.tv_nsec;
}

void get_timespan_difference(const timespan *start, const timespan *end, timespan *out)
{
    assert(start != nullptr);
    assert(end != nullptr);
    assert(out != nullptr);

    long diff = end->nanoseconds - start->nanoseconds;

	if (diff < 0)
    {
		out->seconds = end->seconds - start->seconds - 1;
		out->nanoseconds = NANOSECONDS_IN_A_SECOND + diff;
	}
    else
    {
		out->seconds = end->seconds - start->seconds;
		out->nanoseconds = diff;
	}
}

inline double timespan_to_seconds(const timespan *t)
{
    double ret = t->seconds;
    ret += ((double)t->nanoseconds) / NANOSECONDS_IN_A_SECOND;

    return ret;
}

double get_seconds_difference(const timespan *start, const timespan *end)
{
    assert(start != nullptr);
    assert(end != nullptr);

    timespan diff;
    get_timespan_difference(start, end, &diff);

    return timespan_to_seconds(&diff);
}

void sleep(const timespan *t)
{
    assert(t != nullptr);
    
    sleep(timespan_to_seconds(t));
}

void sleep(double seconds)
{
    sleep_ms((int)(seconds * 1000));
}

void sleep_ms(int milliseconds)
{
#if Linux
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);

#elif Windows
    Sleep(milliseconds);

#else
    usleep(milliseconds * 1000);

#endif
}
