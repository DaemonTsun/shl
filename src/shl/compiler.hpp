
#pragma once

/* compiler.hpp
 *
 * Detects the used compiler and sets preprocessor constants. 
 * For the used operating system used during compilation, see <shl/platform.hpp>.
 */

#if defined(__GNUC__)
#define GNU 1
#define Clang 0
#define MSVC 0
#elif defined(__clang__)
#define GNU 0
#define Clang 1
#define MSVC 0
#elif defined(_MSC_VER)
#define GNU 0
#define Clang 0
#define MSVC 1
#endif

