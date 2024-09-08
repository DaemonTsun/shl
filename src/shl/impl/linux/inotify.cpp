
#include "shl/impl/linux/inotify.hpp"

#if defined(SYS_inotify_init)
sys_int inotify_init()
{
    return (sys_int)linux_syscall(SYS_inotify_init);
}
#endif

sys_int inotify_init1(int flags)
{
    return (sys_int)linux_syscall1(SYS_inotify_init1,
                                   (void*)(sys_int)flags);
}

sys_int inotify_add_watch(int watcher_fd, const char *path, u32 mask)
{
    return (sys_int)linux_syscall3(SYS_inotify_add_watch,
                                   (void*)(sys_int)watcher_fd,
                                   (void*)path,
                                   (void*)(sys_int)mask);
}

sys_int inotify_rm_watch(int watcher_fd, int watched_fd)
{
    return (sys_int)linux_syscall2(SYS_inotify_rm_watch,
                                   (void*)(sys_int)watcher_fd,
                                   (void*)(sys_int)watched_fd);
}

