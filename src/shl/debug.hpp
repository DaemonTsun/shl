
/* debug.hpp

Debugging utility.

The breakpoint() macro may be used to interrupt execution when debugging, with
the option to continue execution afterwards.
*/

#pragma once

#ifndef NDEBUG

#if __GNUC__
// TODO: do other architectures
#define breakpoint() asm ("int3; nop")
#elif _MSC_VER
#define breakpoint() __debugbreak()
#endif

#else

#define breakpoint() ((void)0)

#endif // ifndef NDEBUG
