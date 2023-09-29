
#pragma once

/* platform.hpp
 *
 * Defines simpler platform preprocessor constants for the platform the
 * program is being compiled on.
 *
 * For compiler information, see <shl/compiler.hpp>
 */

#if defined(__linux__)
#define Linux 1
#define Windows 0
#define Mac 0
#elif defined(_WIN32) || defined(_WIN64)
#define Linux 0
#define Windows 1
#define Mac 0
#elif defined(__APPLE__)
#define Linux 0
#define Windows 0
#define Mac 1
#endif
