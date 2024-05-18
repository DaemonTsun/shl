
/* statx.hpp

Linux statx struct and syscall.
*/

#pragma once

#include "shl/number_types.hpp"

struct statx;

extern "C" sys_int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *out);

struct statx_timestamp
{
    s64    tv_sec;
    u32    tv_nsec;
    s32    padding;
};

struct statx
{
    u32    stx_mask;
    u32    stx_blksize;
    u64    stx_attributes;
    u32    stx_nlink;
    u32    stx_uid;
    u32    stx_gid;
    u16    stx_mode;
    u16    padding1[1];
    u64    stx_ino;
    u64    stx_size;
    u64    stx_blocks;
    u64    stx_attributes_mask;
    statx_timestamp stx_atime;
    statx_timestamp stx_btime;
    statx_timestamp stx_ctime;
    statx_timestamp stx_mtime;
    u32    stx_rdev_major;
    u32    stx_rdev_minor;
    u32    stx_dev_major;
    u32    stx_dev_minor;
    u64    stx_mnt_id;
    u32    stx_dio_mem_align;
    u32    stx_dio_offset_align;
    u64    _reserved[12];
};

// these are high bits, lower bits are attributes
#ifndef S_IFMT
#  define S_IFMT      00170000
#  define S_IFSOCK    00140000
#  define S_IFLNK     00120000
#  define S_IFREG     00100000
#  define S_IFBLK     00060000
#  define S_IFDIR     00040000
#  define S_IFCHR     00020000
#  define S_IFIFO     00010000
#endif

#ifndef S_ISUID
#  define S_ISUID     00004000
#  define S_ISGID     00002000
#  define S_ISVTX     00001000
#endif

#ifndef S_ISREG
#  define S_ISREG(m)    (((m) & S_IFMT) == S_IFREG)
#  define S_ISDIR(m)    (((m) & S_IFMT) == S_IFDIR)
#  define S_ISFIFO(m)   (((m) & S_IFMT) == S_IFIFO)
#  define S_ISLNK(m)    (((m) & S_IFMT) == S_IFLNK)
#  define S_ISCHR(m)    (((m) & S_IFMT) == S_IFCHR)
#  define S_ISBLK(m)    (((m) & S_IFMT) == S_IFBLK)
#  define S_ISSOCK(m)   (((m) & S_IFMT) == S_IFSOCK)
#endif

#ifndef S_IRUSR
#  define S_IRWXU 00700
#  define S_IRUSR 00400
#  define S_IWUSR 00200
#  define S_IXUSR 00100

#  define S_IRWXG 00070
#  define S_IRGRP 00040
#  define S_IWGRP 00020
#  define S_IXGRP 00010

#  define S_IRWXO 00007
#  define S_IROTH 00004
#  define S_IWOTH 00002
#  define S_IXOTH 00001
#endif

#ifndef STATX_TYPE
#  define STATX_TYPE          0x0001u
#  define STATX_MODE          0x0002u
#  define STATX_NLINK         0x0004u
#  define STATX_UID           0x0008u
#  define STATX_GID           0x0010u
#  define STATX_ATIME         0x0020u
#  define STATX_MTIME         0x0040u
#  define STATX_CTIME         0x0080u
#  define STATX_INO           0x0100u
#  define STATX_SIZE          0x0200u
#  define STATX_BLOCKS        0x0400u
#  define STATX_BASIC_STATS   0x07ffu
#  define STATX_BTIME         0x0800u
#  define STATX_MNT_ID        0x1000u
#  define STATX_DIOALIGN      0x2000u
#endif

#ifndef STATX_ATTR_COMPRESSED
#  define STATX_ATTR_COMPRESSED   0x00000004
#  define STATX_ATTR_IMMUTABLE    0x00000010
#  define STATX_ATTR_APPEND       0x00000020
#  define STATX_ATTR_NODUMP       0x00000040
#  define STATX_ATTR_ENCRYPTED    0x00000800
#  define STATX_ATTR_AUTOMOUNT    0x00001000
#  define STATX_ATTR_MOUNT_ROOT   0x00002000
#  define STATX_ATTR_VERITY       0x00100000
#  define STATX_ATTR_DAX          0x00200000
#endif
