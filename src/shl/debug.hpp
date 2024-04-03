
/* debug.hpp

Debugging utility.

The breakpoint() macro may be used to interrupt execution when debugging, with
the option to continue execution afterwards.
*/

#pragma once

#ifndef NDEBUG

#include "shl/architecture.hpp"

#if __GNUC__

#  if Architecture == ARCH_x86_64
#    define breakpoint() asm ("int3; nop")
#  elif Architecture == ARCH_aarch64
#    define breakpoint() asm ("brk #0x01")
#  endif

#elif _MSC_VER // Compiler check
#  define breakpoint() __debugbreak()
#endif

#else

#  define breakpoint() ((void)0)

#endif // ifndef NDEBUG
