
// dprint: exlusively include print.hpp if debugging and define debugging prints
// v1.0
#pragma once

#ifndef NDEBUG
#include "shl/print.hpp"

#define dprint(...) print(__VA_ARGS__)
#define dprintln(...) println(__VA_ARGS__)
#define dprint_error(...) print_error(__VA_ARGS__)
#define dprintln_error(...) println_error(__VA_ARGS__)

#ifdef TRACE

#define trace(...) println('[', __FILE__, ':', __LINE__, ']' __VA_OPT__(,) __VA_ARGS__)
#define tprint(...) print(__VA_ARGS__)
#define tprintln(...) println(__VA_ARGS__)
#define tprint_error(...) print_error(__VA_ARGS__)
#define tprintln_error(...) println_error(__VA_ARGS__)

#else // TRACE

#define trace(...)
#define tprint(...)
#define tprintln(...)
#define tprint_error(...)
#define tprintln_error(...)

#endif // TRACE

#else // NDEBUG

#define dprint(...)
#define dprintln(...)
#define dprint_error(...)
#define dprintln_error(...)

#define trace(...)
#define tprint(...)
#define tprintln(...)
#define tprint_error(...)
#define tprintln_error(...)

#endif // NDEBUG
