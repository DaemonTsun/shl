
// Linux syscalls for processes, e.g. fork, ...

#pragma once

extern "C"
{
int fork();
int execve(char *path, char **argv, char **env);

int getpid();
int getppid();
}
