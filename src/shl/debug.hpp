
// debug: debugging functions
// v1.1
//
// sets preprocessor constant Debug to 1 if NDEBUG is not set
//
// provides debug(fmt, ...), same as printf but does nothing if Debug is set
//
// provides trace(fmt, ...), same as printf but does nothing if Debug is not and TRACE is not set
#pragma once

#ifndef NDEBUG
#define Debug 1
#endif

#if Debug

#include <stdio.h>

#define MACRO_TO_STRING2(x) #x
#define MACRO_TO_STRING(x) MACRO_TO_STRING2(x)

#define debug(fmt, ...) printf(fmt, __VA_OPT__(,) __VA_ARGS__)

#ifdef TRACE
#define trace(fmt, ...) printf("[" __FILE__ ":" MACRO_TO_STRING(__LINE__) "] " fmt __VA_OPT__(,) __VA_ARGS__)
#else // no TRACE
#define trace(...)
#endif // end TRACE
#else // no Debug

#define debug(...)
#define trace(...)

#endif // end Debug
