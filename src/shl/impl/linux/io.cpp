
#include "shl/impl/linux/syscalls.hpp"
#include "shl/impl/linux/io.hpp"

extern "C"
{
int open(const char *path, int flags, int mode)
{
    return (int)(sys_int)linux_syscall3(SYS_open,
        (void*)path,
        (void*)(sys_int)flags,
        (void*)(sys_int)mode);
}

int openat(int dir_fd, const char *path, int flags, int mode)
{
    return (int)(sys_int)linux_syscall4(SYS_openat,
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

sys_int readv(int fd, vec_buf *vbufs, sys_int count)
{
    return (sys_int)linux_syscall3(SYS_readv,
        (void*)(sys_int)fd,
        (void*)vbufs,
        (void*)count);
}

sys_int writev(int fd, vec_buf *vbufs, sys_int count)
{
    return (sys_int)linux_syscall3(SYS_writev,
        (void*)(sys_int)fd,
        (void*)vbufs,
        (void*)count);
}

sys_int preadv(int fd, vec_buf *vbufs, sys_int count, sys_int offset)
{
    return (sys_int)linux_syscall4(SYS_preadv,
        (void*)(sys_int)fd,
        (void*)vbufs,
        (void*)count,
        (void*)offset);
}

sys_int pwritev(int fd, vec_buf *vbufs, sys_int count, sys_int offset)
{
    return (sys_int)linux_syscall4(SYS_pwritev,
        (void*)(sys_int)fd,
        (void*)vbufs,
        (void*)count,
        (void*)offset);
}

sys_int preadv2(int fd, vec_buf *vbufs, sys_int count, sys_int offset, int flags)
{
    return (sys_int)linux_syscall5(SYS_preadv2,
        (void*)(sys_int)fd,
        (void*)vbufs,
        (void*)count,
        (void*)offset,
        (void*)(sys_int)flags);
}

sys_int pwritev2(int fd, vec_buf *vbufs, sys_int count, sys_int offset, int flags)
{
    return (sys_int)linux_syscall5(SYS_pwritev2,
        (void*)(sys_int)fd,
        (void*)vbufs,
        (void*)count,
        (void*)offset,
        (void*)(sys_int)flags);
}

sys_int lseek(int fd, sys_int offset, int origin)
{
    return (sys_int)linux_syscall3(SYS_lseek,
        (void*)(sys_int)fd,
        (void*)offset,
        (void*)(sys_int)origin);
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

sys_int copy_file_range(int in_fd, sys_int *in_offset, int out_fd, sys_int *out_offset, int flags)
{
    return (sys_int)::linux_syscall5(SYS_copy_file_range,
        (void*)(sys_int)in_fd,
        (void*)in_offset,
        (void*)(sys_int)out_fd,
        (void*)out_offset,
        (void*)(sys_int)flags);
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
} // extern C
