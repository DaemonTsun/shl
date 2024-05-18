
/* fs.hpp

Linux filesystem syscalls, structs and constants.
*/

#pragma once

#include "shl/number_types.hpp"

extern "C"
{
sys_int read(int fd, void *buf, sys_int size);
sys_int write(int fd, const void *buf, sys_int size);

// open flags
#ifndef O_RDONLY
#  define O_RDONLY    00
#  define O_WRONLY    01
#  define O_RDWR      02
#  define O_CREAT     0100
#  define O_EXCL      0200
#  define O_NOCTTY    0400
#  define O_TRUNC     01000
#  define O_APPEND    02000
#  define O_NONBLOCK  04000
#  define O_SYNC      04010000
#  define O_FSYNC     04010000
#  define O_DSYNC     010000
#  define O_ASYNC     020000
#  define O_DIRECT    040000
#  define O_LARGEFILE 0100000
#  define O_DIRECTORY 0200000
#  define O_NOFOLLOW  0400000
#  define O_NOATIME   01000000
#  define O_CLOEXEC   02000000
#  define O_PATH      010000000
#  define O_TMPFILE   (020000000 | O_DIRECTORY)
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

sys_int open(const char *path, int flags, int mode);
sys_int openat(int dir_fd, const char *path, int flags, int mode);
sys_int close(int fd);

#ifndef SEEK_SET
#  define SEEK_SET  0
#  define SEEK_CUR  1
#  define SEEK_END  2
#  define SEEK_DATA 3
#  define SEEK_HOLE 4
#endif

sys_int lseek(int fd, sys_int offset, int origin);
sys_int pread64(int fd, void *buf, sys_int size, sys_int offset);
sys_int pwrite64(int fd, const void *buf, sys_int size, sys_int offset);

// mode to check with access
#ifndef F_OK
#  define F_OK  0
#  define X_OK  1
#  define W_OK  2
#  define R_OK  4
#endif

sys_int access(const char *path, int mode);
// faccessat

// AT* definitions and flags
#ifndef AT_FDCWD
#  define AT_FDCWD              -100
#  define AT_SYMLINK_NOFOLLOW   0x100
#  define AT_REMOVEDIR          0x200
#  define AT_SYMLINK_FOLLOW     0x400
#  define AT_NO_AUTOMOUNT       0x800
#  define AT_EMPTY_PATH         0x1000
#  define AT_STATX_SYNC_AS_STAT 0x0000
#  define AT_STATX_FORCE_SYNC   0x2000
#  define AT_STATX_DONT_SYNC    0x4000
#  define AT_STATX_SYNC_TYPE    0x6000
#  define AT_RECURSIVE          0x8000

#  define AT_EACCESS            0x200
#endif

sys_int faccessat2(int fd, const char *path, int mode, int flags);

int dup(int oldfd);
int dup2(int oldfd, int newfd);
int dup3(int oldfd, int newfd, int flags); // can specify O_CLOEXEC

sys_int sendfile(int out_fd, int in_fd, sys_int *offset, sys_int count);
sys_int fsync(int fd);
sys_int fdatasync(int fd);
sys_int sync();

sys_int truncate(const char *path, sys_int size);
sys_int ftruncate(int fd, sys_int size);

#ifndef DT_REG
#  define DT_UNKNOWN    0
#  define DT_FIFO       1
#  define DT_CHR        2
#  define DT_DIR        4
#  define DT_BLK        6
#  define DT_REG        8
#  define DT_LNK        10
#  define DT_SOCK       12
#  define DT_WHT        14
#endif

// getdents
struct linux_dirent64
{
    u64  inode;
    s64  offset;
    u16  record_size;
    u8   type; // one of DT_ types
    // char name[]; // null terminated name is directly after type
};

sys_int getdents64(int fd, void *buf, sys_int buf_size);

sys_int getcwd(char *buf, sys_int buf_size);

sys_int chdir(const char *path);
sys_int fchdir(int fd);

sys_int rename(const char *oldpath, const char *newpath);
sys_int renameat(int oldfd, const char *oldpath, int newfd, const char *newpath);

// flags for renameat2
#ifndef RENAME_NOREPLACE
#  define RENAME_NOREPLACE  1
#  define RENAME_EXCHANGE   2
#  define RENAME_WHITEOUT   4
#endif

sys_int renameat2(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags);

sys_int mkdir(const char *path, int mode);
sys_int mkdirat(int fd, const char *path, int mode);
sys_int rmdir(const char *path);

sys_int link(const char *path, const char *newpath);
sys_int linkat(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags);
sys_int unlink(const char *path);
sys_int unlinkat(int fd, const char *path, int flags);

sys_int symlink(const char *target, const char *symlinkpath);
sys_int symlinkat(const char *target, int symlinkfd, const char *symlinkpath);

sys_int readlink(const char *path, char *buf, sys_int buf_size);
sys_int readlinkat(int fd, const char *path, char *buf, sys_int buf_size);

sys_int chmod(const char *path, int mode);
sys_int fchmod(int fd, int mode);
sys_int fchmodat(int fd, const char *path, int mode, int flags);

sys_int chown(const char *path, int owner, int group);
sys_int fchown(int fd, int owner, int group);
sys_int fchownat(int fd, const char *path, int owner, int group, int flags);

sys_int umask(int mask);
sys_int chroot(const char *path);

#ifndef UTIME_NOW
#  define UTIME_NOW     ((1l << 30) - 1l)
#  define UTIME_OMIT    ((1l << 30) - 2l)
#endif
sys_int utimensat(int fd, const char *path, void *times, int flags);
}
