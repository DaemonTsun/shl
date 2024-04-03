
/* architecture.hpp

CPU Architecture header.

Defines the Architecture preprocessor constant to hold the architecture of the system.
Individual architecture values may be accessed with the ARCH_xxx preprocessor constants,
where "xxx" is the value returned by "uname -m" on modern Linux systems, for instance
on a 64 bit Intel / AMD CPU, Architecture == ARCH_x86_64 will evaulate to true.
*/

#pragma once

#define ARCH_x86        1
#define ARCH_x86_64     2
#define ARCH_aarch64    3

#if defined(__x86_64__) || defined(_M_X64)
#define Architecture ARCH_x86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define Architecture ARCH_x86
#elif defined(__aarch64__) || defined(_M_ARM64)
#define Architecture ARCH_aarch64
/*
#elif defined(mips) || defined(__mips__) || defined(__mips)
#define Architecture ARCH_mips
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
#define Architecture ARCH_powerpc
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
#define Architecture ARCH_powerpc64
*/
#else
#define Architecture 0
#endif
