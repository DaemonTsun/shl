
/* assert.hpp

Assert header.

When not compiling a debug build (i.e. NDEBUG is set), defines the

    assert(...)

macro to do nothing.

When compiling a debug build (NDEBUG is NOT set), defines the

    assert(Cond[, Msg])

which breaks at the assert when Cond does not evaluate to true.

By default, assert will print in color on non-Windows operating systems, which
can be disabled by defining the NO_ASSERT_COLORS preprocessor constant.
*/
#pragma once

#ifdef NDEBUG
#define assert(...) ((void)0)
#else

extern void _print_assert_msg(const char *msg, int len);

#ifndef STRINGIFY
#define STRINGIFY2(x) #x
#define STRINGIFY(x)  STRINGIFY2(x)
#endif

#if defined(_MSC_VER) || defined(NO_ASSERT_COLORS)
#define _A_TERM_COLOR_RED   ""
#define _A_TERM_COLOR_BLUE  ""
#define _A_TERM_COLOR_RESET ""
#else
#define _A_TERM_COLOR_RED   "\033[31m"
#define _A_TERM_COLOR_BLUE  "\033[36m"
#define _A_TERM_COLOR_RESET "\033[0m"
#endif

#if __GNUC__
#define _do_break() __builtin_trap()
#elif _MSC_VER
#define _do_break() __debugbreak()
#endif

#if defined(ASSERT_PRINT_MESSAGE)
#define assert(c, ...) \
    {\
        if (!(c))\
        {\
            _print_assert_msg("[" _A_TERM_COLOR_BLUE __FILE__ ":" STRINGIFY(__LINE__) _A_TERM_COLOR_RESET "] assert failed:\n\tassert(" _A_TERM_COLOR_RED STRINGIFY(c) _A_TERM_COLOR_RESET __VA_OPT__(", ") #__VA_ARGS__ ")\n\n",\
                     sizeof("[" _A_TERM_COLOR_BLUE __FILE__ ":" STRINGIFY(__LINE__) _A_TERM_COLOR_RESET "] assert failed:\n\tassert(" _A_TERM_COLOR_RED STRINGIFY(c) _A_TERM_COLOR_RESET __VA_OPT__(", ") #__VA_ARGS__ ")\n\n"));\
            _do_break();\
        }\
    } while (0);
#else
#define assert(c, ...) \
    {\
        if (!(c)) _do_break();\
    } while (0);
#endif

#endif
