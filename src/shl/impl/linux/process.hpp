
// Linux syscalls for processes, e.g. fork, ...

#pragma once

#include "shl/impl/linux/syscalls.hpp"

#if defined(SYS_fork)
int fork();
#endif
int execve(char *path, char **argv, char **env);

int getpid();
int getppid();
