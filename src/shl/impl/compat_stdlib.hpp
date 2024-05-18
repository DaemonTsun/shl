
// Include this, then include C standard library headers,
// then include combat_stdlib_end.hpp

#define printf      __builtin_printf
#define rename      __builtin_rename
#define renameat    __builtin_renameat
#define renameat2   __builtin_renameat2
#define access      __builtin_access
#define close       __builtin_close
#define open        __builtin_open
#define read        __builtin_read
#define write       __builtin_write
#define pread64     __builtin_pread64
#define pwrite64    __builtin_pwrite64
#define chown       __builtin_chown
#define fchown      __builtin_fchown
#define fchownat    __builtin_fchownat
#define chdir       __builtin_chdir
#define fchdir      __builtin_fchdir
#define getcwd      __builtin_getcwd
#define link        __builtin_link
#define linkat      __builtin_linkat
#define symlink     __builtin_symlink
#define symlinkat   __builtin_symlinkat
#define readlink    __builtin_readlink
#define readlinkat  __builtin_readlinkat
#define unlink      __builtin_unlink
#define unlinkat    __builtin_unlinkat
#define rmdir       __builtin_rmdir
#define chroot      __builtin_chroot
#define fsync       __builtin_fsync
#define fdatasync   __builtin_fdatasync
#define sync        __builtin_sync
#define truncate    __builtin_truncate
#define ftruncate   __builtin_ftruncate
