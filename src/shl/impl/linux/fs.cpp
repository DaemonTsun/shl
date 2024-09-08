
#include "shl/impl/linux/fs.hpp"

#if defined(SYS_access)
sys_int access(const char *path, int mode)
{
    return (sys_int)linux_syscall2(SYS_access,
                                   (void*)path,
                                   (void*)(sys_int)mode);
}
#endif

sys_int faccessat2(int fd, const char *path, int mode, int flags)
{
    return (sys_int)::linux_syscall4(SYS_faccessat2,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)mode,
        (void*)(sys_int)flags);
}

sys_int truncate(const char *path, sys_int size)
{
    return (sys_int)linux_syscall2(SYS_truncate,
        (void*)path,
        (void*)size);
}

sys_int ftruncate(int fd, sys_int size)
{
    return (sys_int)linux_syscall2(SYS_ftruncate,
        (void*)(sys_int)fd,
        (void*)size);
}

sys_int getdents64(int fd, void *buf, sys_int buf_size)
{
    return (sys_int)linux_syscall3(SYS_getdents64,
        (void*)(sys_int)fd,
        buf,
        (void*)buf_size);
}

sys_int getcwd(char *buf, sys_int buf_size)
{
    return (sys_int)linux_syscall2(SYS_getcwd,
        buf,
        (void*)buf_size);
}

sys_int chdir(const char *path)
{
    return (sys_int)linux_syscall1(SYS_chdir,
        (void*)path);
}

sys_int fchdir(int fd)
{
    return (sys_int)linux_syscall1(SYS_fchdir,
        (void*)(sys_int)fd);
}

#if defined(SYS_rename)
sys_int rename(const char *oldpath, const char *newpath)
{
    return (sys_int)::linux_syscall2(SYS_rename,
        (void*)oldpath,
        (void*)newpath);
}
#endif

sys_int renameat(int oldfd, const char *oldpath, int newfd, const char *newpath)
{
    return (sys_int)::linux_syscall4(SYS_renameat,
        (void*)(sys_int)oldfd,
        (void*)oldpath,
        (void*)(sys_int)newfd,
        (void*)newpath);
}

sys_int renameat2(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags)
{
    return (sys_int)::linux_syscall5(SYS_renameat2,
        (void*)(sys_int)oldfd,
        (void*)oldpath,
        (void*)(sys_int)newfd,
        (void*)newpath,
        (void*)(sys_int)flags);
}

#if defined(SYS_mkdir)
sys_int mkdir(const char *path, int mode)
{
    return (sys_int)::linux_syscall2(SYS_mkdir,
        (void*)path,
        (void*)(sys_int)mode);
}
#endif

sys_int mkdirat(int fd, const char *path, int mode)
{
    return (sys_int)::linux_syscall3(SYS_mkdirat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)mode);
}

#if defined(SYS_rmdir)
sys_int rmdir(const char *path)
{
    return (sys_int)::linux_syscall1(SYS_rmdir,
        (void*)path);
}
#endif

#if defined(SYS_link)
sys_int link(const char *path, const char *newpath)
{
    return (sys_int)::linux_syscall2(SYS_link,
        (void*)path,
        (void*)newpath);
}
#endif

sys_int linkat(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags)
{
    return (sys_int)::linux_syscall5(SYS_linkat,
        (void*)(sys_int)oldfd,
        (void*)oldpath,
        (void*)(sys_int)newfd,
        (void*)newpath,
        (void*)(sys_int)flags);
}

#if defined(SYS_unlink)
sys_int unlink(const char *path)
{
    return (sys_int)::linux_syscall1(SYS_unlink,
        (void*)path);
}
#endif

sys_int unlinkat(int fd, const char *path, int flags)
{
    return (sys_int)::linux_syscall3(SYS_unlinkat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)flags);
}

#if defined(SYS_symlink)
sys_int symlink(const char *target, const char *symlinkpath)
{
    return (sys_int)::linux_syscall2(SYS_symlink,
        (void*)target,
        (void*)symlinkpath);
}
#endif

sys_int symlinkat(const char *target, int symlinkfd, const char *symlinkpath)
{
    return (sys_int)::linux_syscall3(SYS_symlinkat,
        (void*)target,
        (void*)(sys_int)symlinkfd,
        (void*)symlinkpath);
}

#if defined(SYS_readlink)
sys_int readlink(const char *path, char *buf, sys_int buf_size)
{
    return (sys_int)::linux_syscall3(SYS_readlink,
        (void*)path,
        (void*)buf,
        (void*)buf_size);
}
#endif

sys_int readlinkat(int fd, const char *path, char *buf, sys_int buf_size)
{
    return (sys_int)::linux_syscall4(SYS_readlinkat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)buf,
        (void*)buf_size);
}

#if defined(SYS_chmod)
sys_int chmod(const char *path, int mode)
{
    return (sys_int)::linux_syscall2(SYS_chmod,
        (void*)path,
        (void*)(sys_int)mode);
}
#endif

sys_int fchmod(int fd, int mode)
{
    return (sys_int)::linux_syscall2(SYS_fchmod,
        (void*)(sys_int)fd,
        (void*)(sys_int)mode);
}

sys_int fchmodat(int fd, const char *path, int mode, int flags)
{
    return (sys_int)::linux_syscall4(SYS_fchmodat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)mode,
        (void*)(sys_int)flags);
}

#if defined(SYS_chown)
sys_int chown(const char *path, int owner, int group)
{
    return (sys_int)::linux_syscall3(SYS_chown,
        (void*)path,
        (void*)(sys_int)owner,
        (void*)(sys_int)group);
}
#endif

sys_int fchown(int fd, int owner, int group)
{
    return (sys_int)::linux_syscall3(SYS_fchown,
        (void*)(sys_int)fd,
        (void*)(sys_int)owner,
        (void*)(sys_int)group);
}

sys_int fchownat(int fd, const char *path, int owner, int group, int flags)
{
    return (sys_int)::linux_syscall5(SYS_fchownat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)owner,
        (void*)(sys_int)group,
        (void*)(sys_int)flags);
}

sys_int umask(int mask)
{
    return (sys_int)::linux_syscall1(SYS_umask,
        (void*)(sys_int)mask);
}

sys_int chroot(const char *path)
{
    return (sys_int)::linux_syscall1(SYS_chroot,
        (void*)path);
}

sys_int utimensat(int fd, const char *path, void *times, int flags)
{
    return (sys_int)::linux_syscall4(SYS_utimensat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)times,
        (void*)(sys_int)flags);
}
