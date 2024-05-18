
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/fs.hpp"

extern "C"
{
sys_int read(int fd, void *buf, sys_int size)
{
    return (sys_int)linux_syscall3(SYS_read,
        (void*)(sys_int)fd,
        buf,
        (void*)size);
}

sys_int write(int fd, const void *buf, sys_int size)
{
    return (sys_int)linux_syscall3(SYS_write,
        (void*)(sys_int)fd,
        (void*)buf,
        (void*)size);
}

sys_int open(const char *path, int flags, int mode)
{
    return (sys_int)linux_syscall3(SYS_open,
        (void*)path,
        (void*)(sys_int)flags,
        (void*)(sys_int)mode);
}

sys_int openat(int dir_fd, const char *path, int flags, int mode)
{
    return (sys_int)linux_syscall4(SYS_openat,
        (void*)(sys_int)dir_fd,
        (void*)path,
        (void*)(sys_int)flags,
        (void*)(sys_int)mode);
}

sys_int close(int fd)
{
    return (sys_int)linux_syscall1(SYS_close,
                                   (void*)(sys_int)fd);
}

sys_int lseek(int fd, sys_int offset, int origin)
{
    return (sys_int)linux_syscall3(SYS_lseek,
        (void*)(sys_int)fd,
        (void*)offset,
        (void*)(sys_int)origin);
}

sys_int pread64(int fd, void *buf, sys_int size, sys_int offset)
{
    return (sys_int)linux_syscall4(SYS_pread64,
        (void*)(sys_int)fd,
        buf,
        (void*)size,
        (void*)offset);
}

sys_int pwrite64(int fd, const void *buf, sys_int size, sys_int offset)
{
    return (sys_int)linux_syscall4(SYS_pwrite64,
        (void*)(sys_int)fd,
        (void*)buf,
        (void*)size,
        (void*)offset);
}

sys_int access(const char *path, int mode)
{
    return (sys_int)linux_syscall2(SYS_access,
                                   (void*)path,
                                   (void*)(sys_int)mode);
}

sys_int faccessat2(int fd, const char *path, int mode, int flags)
{
    return (sys_int)::linux_syscall4(SYS_faccessat2,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)mode,
        (void*)(sys_int)flags);
}

int dup(int oldfd)
{
    return (int)(sys_int)linux_syscall1(SYS_dup,
                                        (void*)(sys_int)oldfd);
}

int dup2(int oldfd, int newfd)
{
    return (int)(sys_int)linux_syscall2(SYS_dup2,
                                        (void*)(sys_int)oldfd,
                                        (void*)(sys_int)newfd);
}

int dup3(int oldfd, int newfd, int flags)
{
    return (int)(sys_int)linux_syscall3(SYS_dup3,
        (void*)(sys_int)oldfd,
        (void*)(sys_int)newfd,
        (void*)(sys_int)flags);
}

sys_int sendfile(int out_fd, int in_fd, sys_int *offset, sys_int count)
{
    return (sys_int)::linux_syscall4(SYS_sendfile,
        (void*)(sys_int)out_fd,
        (void*)(sys_int)in_fd,
        (void*)offset,
        (void*)count);
}

sys_int fsync(int fd)
{
    return (sys_int)linux_syscall1(SYS_fsync,
        (void*)(sys_int)fd);
}

sys_int fdatasync(int fd)
{
    return (sys_int)linux_syscall1(SYS_fdatasync,
        (void*)(sys_int)fd);
}

sys_int sync()
{
    return (sys_int)linux_syscall(SYS_sync);
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

sys_int rename(const char *oldpath, const char *newpath)
{
    return (sys_int)::linux_syscall2(SYS_rename,
        (void*)oldpath,
        (void*)newpath);
}

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

sys_int mkdir(const char *path, int mode)
{
    return (sys_int)::linux_syscall2(SYS_mkdir,
        (void*)path,
        (void*)(sys_int)mode);
}

sys_int mkdirat(int fd, const char *path, int mode)
{
    return (sys_int)::linux_syscall3(SYS_mkdirat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)mode);
}

sys_int rmdir(const char *path)
{
    return (sys_int)::linux_syscall1(SYS_rmdir,
        (void*)path);
}

sys_int link(const char *path, const char *newpath)
{
    return (sys_int)::linux_syscall2(SYS_link,
        (void*)path,
        (void*)newpath);
}

sys_int linkat(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags)
{
    return (sys_int)::linux_syscall5(SYS_linkat,
        (void*)(sys_int)oldfd,
        (void*)oldpath,
        (void*)(sys_int)newfd,
        (void*)newpath,
        (void*)(sys_int)flags);
}

sys_int unlink(const char *path)
{
    return (sys_int)::linux_syscall1(SYS_unlink,
        (void*)path);
}

sys_int unlinkat(int fd, const char *path, int flags)
{
    return (sys_int)::linux_syscall3(SYS_unlinkat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)flags);
}

sys_int symlink(const char *target, const char *symlinkpath)
{
    return (sys_int)::linux_syscall2(SYS_symlink,
        (void*)target,
        (void*)symlinkpath);
}

sys_int symlinkat(const char *target, int symlinkfd, const char *symlinkpath)
{
    return (sys_int)::linux_syscall3(SYS_symlinkat,
        (void*)target,
        (void*)(sys_int)symlinkfd,
        (void*)symlinkpath);
}

sys_int readlink(const char *path, char *buf, sys_int buf_size)
{
    return (sys_int)::linux_syscall3(SYS_readlink,
        (void*)path,
        (void*)buf,
        (void*)buf_size);
}

sys_int readlinkat(int fd, const char *path, char *buf, sys_int buf_size)
{
    return (sys_int)::linux_syscall4(SYS_readlinkat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)buf,
        (void*)buf_size);
}

sys_int chmod(const char *path, int mode)
{
    return (sys_int)::linux_syscall2(SYS_chmod,
        (void*)path,
        (void*)(sys_int)mode);
}

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

sys_int chown(const char *path, int owner, int group)
{
    return (sys_int)::linux_syscall3(SYS_chown,
        (void*)path,
        (void*)(sys_int)owner,
        (void*)(sys_int)group);
}

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

sys_int mknod(const char *path, int mode, sys_int dev)
{
    return (sys_int)::linux_syscall3(SYS_mknod,
        (void*)path,
        (void*)(sys_int)mode,
        (void*)dev);
}

sys_int mknodat(int fd, const char *path, int mode, sys_int dev)
{
    return (sys_int)::linux_syscall4(SYS_mknodat,
        (void*)(sys_int)fd,
        (void*)path,
        (void*)(sys_int)mode,
        (void*)dev);
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

} // extern C
