
/* inotify.hpp

Linux inotify syscalls.
 */
#pragma once

#include "shl/number_types.hpp"
#include "shl/macros.hpp"

#ifndef IN_NONBLOCK
#  define IN_NONBLOCK 0x800
#  define IN_CLOEXEC  0x80000
#endif

// inotify_add_watch flags & inotify_event masks
#ifndef IN_ACCESS
#  define IN_ACCESS         0x0001 // file was accessed
#  define IN_MODIFY         0x0002 // file was modified
#  define IN_ATTRIB         0x0004 // metadata changed
#  define IN_CLOSE_WRITE    0x0008 // writtable file was closed
#  define IN_CLOSE_NOWRITE  0x0010 // unwrittable file closed
#  define IN_CLOSE          (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) // close
#  define IN_OPEN           0x0020 // file was opened
#  define IN_MOVED_FROM     0x0040 // file was moved from x
#  define IN_MOVED_TO       0x0080 // file was moved to y
#  define IN_MOVE           (IN_MOVED_FROM | IN_MOVED_TO) // moves
#  define IN_CREATE         0x0100 // subfile was created
#  define IN_DELETE         0x0200 // subfile was deleted
#  define IN_DELETE_SELF    0x0400 // self was deleted
#  define IN_MOVE_SELF      0x0800 // self was moved

#  define IN_ALL_EVENTS     (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE  \
              | IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM          \
              | IN_MOVED_TO | IN_CREATE | IN_DELETE              \
              | IN_DELETE_SELF | IN_MOVE_SELF)
#endif

#ifndef IN_UNMOUNT
#  define IN_UNMOUNT    0x2000 // fs was unmounted
#  define IN_Q_OVERFLOW 0x4000 // event queued overflowed
#  define IN_IGNORED    0x8000 // file was ignored?
#endif

struct inotify_event
{
    int watched_fd;
    u32 mask; // events and errors
    u32 cookie; // ???
    u32 name_length; // number of bytes after this that contain the name of the watched path
    // name is directly after name_length
};

static inline const char *inotify_event_name(inotify_event *ev)
{
    return (const char*)(ev) + offset_of(inotify_event, name_length) + sizeof(u32);
}

#define IN_ONLYDIR      0x01000000 // only watch directories
#define IN_DONT_FOLLOW  0x02000000 // do not follow a symlink
#define IN_EXCL_UNLINK  0x04000000
#define IN_MASK_CREATE  0x10000000 // only create watches
#define IN_MASK_ADD     0x20000000 // add to the mask of an already existing watch
#define IN_ISDIR        0x40000000 // event happened on a directory
#define IN_ONESHOT      0x80000000 // only send event once

sys_int inotify_init();
sys_int inotify_init1(int flags);
sys_int inotify_add_watch(int watcher_fd, const char *path, u32 mask);
sys_int inotify_rm_watch(int watcher_fd, int watched_fd);
