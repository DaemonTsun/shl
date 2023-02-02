
#pragma once

/* platform.hpp
 *
 * defines simpler platform preprocessor constants for the platform the
 * program is being compiled on.
 */

#if defined(__linux__)
#define Linux 1
#elif defined(_WIN32) || defined(_WIN64)
#define Windows 1
#elif defined(__APPLE__)
#define Mac 1
#endif
