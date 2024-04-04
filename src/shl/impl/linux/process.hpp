
// Linux syscalls for processes, e.g. fork, dup, ...

#pragma once

extern "C"
{
int fork();
int execve(char *path, char **argv, char **env);

int getpid();
int getppid();

int dup(int oldfd);
int dup2(int oldfd, int newfd);
int dup3(int oldfd, int newfd, int flags);
}
