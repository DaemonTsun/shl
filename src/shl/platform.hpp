
#pragma once

/* platform.hpp

Defines simpler platform preprocessor constants for the platform the
program is being compiled on.

Also defines the sys_char type which is wchar_t on Windows, char on
anything else.

For compiler information, see <shl/compiler.hpp>
*/

#if defined(__linux__)
#define Linux 1
#define Windows 0
#define Mac 0
typedef char sys_char;
#define SYS_CHAR(x) x
#elif defined(_WIN32) || defined(_WIN64)
#define Linux 0
#define Windows 1
#define Mac 0

#if defined(UNICODE)
typedef wchar_t sys_char;
#define SYS_CHAR(x) L##x
#else
typedef char sys_char;
#define SYS_CHAR(x) x
#endif

#elif defined(__APPLE__)
#define Linux 0
#define Windows 0
#define Mac 1
typedef char sys_char;
#define SYS_CHAR(x) x
#endif
