
// debug: debugging functions
// v1.1
#pragma once

#ifndef NDEBUG

#include <stdio.h>

#define TOSTRING(x) #x

#define debug(...) printf(__VA_ARGS__)

#ifdef TRACE
#define trace(...) printf("[" __FILE__ ":" TOSTRING(__LINE__) "]" __VA_OPT__(,) __VA_ARGS__)
#else // no TRACE
#define trace(...)
#endif // end TRACE
#else // no NDEBUG

#define debug(...)
#define trace(...)

#endif // end NDEBUG
