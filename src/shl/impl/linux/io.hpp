
/* io.hpp

Linux io syscalls, structs and constants.
*/

#pragma once

#include "shl/number_types.hpp"

extern "C"
{
// open flags
#ifndef O_RDONLY
#  define O_RDONLY    0x0
#  define O_WRONLY    0x1
#  define O_RDWR      0x2
#  define O_CREAT     0x40
#  define O_EXCL      0x80
#  define O_NOCTTY    0x100
#  define O_TRUNC     0x200
#  define O_APPEND    0x400
#  define O_NONBLOCK  0x800
#  define O_SYNC      0x101000
#  define O_FSYNC     0x101000
#  define O_DSYNC     0x1000
#  define O_ASYNC     0x2000
#  define O_DIRECT    0x4000
#  define O_LARGEFILE 0x8000
#  define O_DIRECTORY 0x10000
#  define O_NOFOLLOW  0x20000
#  define O_NOATIME   0x40000
#  define O_CLOEXEC   0x80000
#  define O_PATH      0x200000
#  define O_TMPFILE   (0x400000 | O_DIRECTORY)
#endif

// mode
#ifndef S_IREAD
#  define S_IREAD    0400
#  define S_IWRITE   0200
#  define S_IEXEC    0100
#endif

#ifndef S_ISUID
#  define S_ISUID     00004000
#  define S_ISGID     00002000
#  define S_ISVTX     00001000
#endif

#ifndef S_IRUSR
// user
#  define S_IRWXU 00700
#  define S_IRUSR 00400
#  define S_IWUSR 00200
#  define S_IXUSR 00100

// group
#  define S_IRWXG 00070
#  define S_IRGRP 00040
#  define S_IWGRP 00020
#  define S_IXGRP 00010

// other
#  define S_IRWXO 00007
#  define S_IROTH 00004
#  define S_IWOTH 00002
#  define S_IXOTH 00001
#endif

int open(const char *path, int flags, int mode);
int openat(int dir_fd, const char *path, int flags, int mode);
sys_int close(int fd);
sys_int read(int fd, void *buf, sys_int size);
sys_int write(int fd, const void *buf, sys_int size);

sys_int pread64(int fd, void *buf, sys_int size, sys_int offset);
sys_int pwrite64(int fd, const void *buf, sys_int size, sys_int offset);

struct vec_buf
{
    void *data;
    sys_int size;
};

sys_int readv(int fd, vec_buf *vbufs, sys_int count);
sys_int writev(int fd, vec_buf *vbufs, sys_int count);
sys_int preadv(int fd, vec_buf *vbufs, sys_int count, sys_int offset);
sys_int pwritev(int fd, vec_buf *vbufs, sys_int count, sys_int offset);

// flags for preadv2/pwritev2
#ifndef RWF_HIPRI
#  define RWF_HIPRI     0x01
#  define RWF_DSYNC     0x02
#  define RWF_SYNC      0x04
#  define RWF_NOWAIT    0x08
#  define RWF_APPEND    0x10
#endif

sys_int preadv2(int fd, vec_buf *vbufs, sys_int count, sys_int offset, int flags);
sys_int pwritev2(int fd, vec_buf *vbufs, sys_int count, sys_int offset, int flags);

#ifndef SEEK_SET
#  define SEEK_SET  0
#  define SEEK_CUR  1
#  define SEEK_END  2
#  define SEEK_DATA 3
#  define SEEK_HOLE 4
#endif

sys_int lseek(int fd, sys_int offset, int origin);

int dup(int oldfd);
int dup2(int oldfd, int newfd);
int dup3(int oldfd, int newfd, int flags); // can specify O_CLOEXEC

sys_int sendfile(int out_fd, int in_fd, sys_int *offset, sys_int count);
sys_int copy_file_range(int in_fd, sys_int *in_offset, int out_fd, sys_int *out_offset, int flags);
sys_int fsync(int fd);
sys_int fdatasync(int fd);
sys_int sync();
}
