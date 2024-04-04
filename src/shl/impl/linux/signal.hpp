
// Linux signals

#pragma once

#include "shl/number_types.hpp"

extern "C" sys_int kill(int pid, int status);

#ifndef SIGHUP
#  define SIGHUP  1
#  define SIGINT  2
#  define SIGQUIT 3
#  define SIGILL  4
#  define SIGTRAP 5
#  define SIGABRT 6
#  define SIGFPE  8
#  define SIGKILL 9
#  define SIGSEGV 11
#  define SIGPIPE 13
#  define SIGALRM 14
#  define SIGTERM 15
#endif

