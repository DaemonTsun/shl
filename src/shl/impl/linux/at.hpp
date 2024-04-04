
// AT_* constants

#ifndef AT_FDCWD
#  define AT_FDCWD               -100
#  define AT_SYMLINK_NOFOLLOW    0x0100
#  define AT_REMOVEDIR           0x0200
#  define AT_SYMLINK_FOLLOW      0x0400
#  define AT_NO_AUTOMOUNT        0x0800
#  define AT_EMPTY_PATH          0x1000
#  define AT_STATX_SYNC_TYPE     0x6000
#  define AT_STATX_SYNC_AS_STAT  0x0000
#  define AT_STATX_FORCE_SYNC    0x2000
#  define AT_STATX_DONT_SYNC     0x4000
#  define AT_RECURSIVE           0x8000
#  define AT_EACCESS             0x0200
#endif
