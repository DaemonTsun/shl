
/* debug.hpp

Debugging utility.

The breakpoint() macro may be used to interrupt execution, with
the option to continue execution afterwards.

The debug_breakpoint() macro is does the same as breakpoint() on a
debug build, but does nothing on a differnent type of build.
*/

#pragma once

#include "shl/architecture.hpp"

#if __GNUC__
#  if Architecture == ARCH_x86_64
#    define breakpoint() asm ("int3; nop")
#  elif Architecture == ARCH_aarch64
#    define breakpoint() asm ("brk #0x01")
#  endif
#elif _MSC_VER
#  define breakpoint() __debugbreak()
#else
#  define breakpoint() ((void)0)
#endif // compiler

#ifndef NDEBUG
#  define debug_breakpoint() breakpoint()
#else
#  define debug_breakpoint() ((void)0)
#endif // ifndef NDEBUG
