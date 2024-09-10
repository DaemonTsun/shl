
#pragma once

/* platform.hpp

Defines simpler platform preprocessor constants for the platform the
program is being compiled on.

Also defines the sys_char type which is char16_t on Windows (if UNICODE is set),
and char on anything else.

For compiler information, see <shl/compiler.hpp>
*/

#if defined(__linux__)
#   define Linux 1
#   define Windows 0
#   define Mac 0
    typedef char sys_char;
#   define SYS_CHAR(x) x

#elif defined(_WIN32) || defined(_WIN64)

#   define Linux 0
#   define Windows 1
#   define Mac 0

#if defined(UNICODE)
    typedef char16_t sys_char;
#   define SYS_CHAR(x) u##x
#   define SYS_NATIVE_CHAR(x) L##x
#else
    typedef char sys_char;
#   define SYS_CHAR(x) x
#endif

#elif defined(__APPLE__)
#   define Linux 0
#   define Windows 0
#   define Mac 1

    typedef char sys_char;
#   define SYS_CHAR(x) x
#endif

#ifndef SYS_NATIVE_CHAR
#   define SYS_NATIVE_CHAR(...) SYS_CHAR(__VA_ARGS__)
#endif
