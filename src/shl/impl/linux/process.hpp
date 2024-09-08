
// Linux syscalls for processes, e.g. fork, ...

#pragma once

#include "shl/impl/linux/syscalls.hpp"

#if defined(SYS_fork)
int fork();
#endif
int execve(char *path, char **argv, char **env);

// prefer to use this, some architectures (e.g. aarch64) don't support fork
int clone_fork();

int getpid();
int getppid();
