
#include "shl/platform.hpp"

#if Windows
#include <windows.h>
#elif Linux
#include <time.h>
#include <unistd.h>
#endif

#include "shl/time.hpp"

void sleep(double seconds)
{
    sleep_ms((int)(seconds * 1000));
}

void sleep_ms(int milliseconds)
{
    #ifdef WIN32
        Sleep(milliseconds);

    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);

    #else
        usleep(milliseconds * 1000);

    #endif
}
