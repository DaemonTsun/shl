
/* fs.hpp

Linux filesystem syscalls, structs and constants.
*/

#pragma once

#include "shl/impl/linux/syscalls.hpp"
#include "shl/number_types.hpp"

// mode to check with access
#ifndef F_OK
#  define F_OK  0
#  define X_OK  1
#  define W_OK  2
#  define R_OK  4
#endif

#if defined(SYS_access)
sys_int access(const char *path, int mode);
#endif
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

#if defined(SYS_rename)
sys_int rename(const char *oldpath, const char *newpath);
#endif
sys_int renameat(int oldfd, const char *oldpath, int newfd, const char *newpath);

// flags for renameat2
#ifndef RENAME_NOREPLACE
#  define RENAME_NOREPLACE  1
#  define RENAME_EXCHANGE   2
#  define RENAME_WHITEOUT   4
#endif

sys_int renameat2(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags);

#if defined(SYS_mkdir)
sys_int mkdir(const char *path, int mode);
#endif
sys_int mkdirat(int fd, const char *path, int mode);

#if defined(SYS_rmdir)
sys_int rmdir(const char *path);
#endif

#if defined(SYS_link)
sys_int link(const char *path, const char *newpath);
#endif
sys_int linkat(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags);
#if defined(SYS_unlink)
sys_int unlink(const char *path);
#endif
sys_int unlinkat(int fd, const char *path, int flags);

#if defined(SYS_symlink)
sys_int symlink(const char *target, const char *symlinkpath);
#endif
sys_int symlinkat(const char *target, int symlinkfd, const char *symlinkpath);

#if defined(SYS_readlink)
sys_int readlink(const char *path, char *buf, sys_int buf_size);
#endif
sys_int readlinkat(int fd, const char *path, char *buf, sys_int buf_size);

#if defined(SYS_chmod)
sys_int chmod(const char *path, int mode);
#endif
sys_int fchmod(int fd, int mode);
sys_int fchmodat(int fd, const char *path, int mode, int flags);

#if defined(SYS_chown)
sys_int chown(const char *path, int owner, int group);
#endif
sys_int fchown(int fd, int owner, int group);
sys_int fchownat(int fd, const char *path, int owner, int group, int flags);

sys_int umask(int mask);
sys_int chroot(const char *path);

#ifndef UTIME_NOW
#  define UTIME_NOW     ((1l << 30) - 1l)
#  define UTIME_OMIT    ((1l << 30) - 2l)
#endif
sys_int utimensat(int fd, const char *path, void *times, int flags);
