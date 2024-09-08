
/* debug.hpp

Debugging utility.

The breakpoint() macro may be used to interrupt execution, with
the option to continue execution afterwards.
breakpoint(N) breaks only if the Nth breakpoint is enabled using
enable_breakpoint(N) (up to 15).
Breakpoints may be disabled using disable_breakpoint(N).

The debug_breakpoint() macro is does the same as breakpoint() on a
debug build, but does nothing on a differnent type of build.

Example usage of breakpoint(N):
    int often_called_function(int n)
    {
        breakpoint(1); // only breaks when breakpoint "1" is enabled
        if (n > 0)
            return 1;
        return 0;
    }

    void some_function()
    {
        for (int i = 0; i < 500; ++i)
        {
            if (i == 400)
                enable_breakpoint(1);
            if (i == 403)
                disable_breakpoint(1);

            often_called_function(i);
        }
    }
*/

#pragma once

#include "shl/architecture.hpp"

#if __GNUC__
#  if Architecture == ARCH_x86_64
#    define _breakpoint() asm ("int3; nop")
#  elif Architecture == ARCH_aarch64
#    define _breakpoint() asm ("brk #0x01")
#  endif
#elif _MSC_VER
#  define _breakpoint() __debugbreak()
#else
#  define _breakpoint() ((void)0)
#endif // compiler

#ifndef GET_MACRO1
#  define GET_MACRO1(_1, _2, NAME, ...) NAME
#endif

extern int _breakpoint_flags[16];

#define _toggled_breakpoint(N)  {if (_breakpoint_flags[N]) _breakpoint();} while(0)
#define enable_breakpoint(N)    {_breakpoint_flags[N] = 1;} while(0)
#define disable_breakpoint(N)   {_breakpoint_flags[N] = 0;} while(0)
#define breakpoint(...) GET_MACRO1(,##__VA_ARGS__, _toggled_breakpoint, _breakpoint)(__VA_ARGS__)

#ifndef NDEBUG
#  define debug_breakpoint(...) breakpoint(__VA_ARGS__)
#else
#  define debug_breakpoint() ((void)0)
#endif // ifndef NDEBUG
