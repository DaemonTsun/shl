
// TODO: windows version
#include "shl/platform.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include <filesystem>
#include <unordered_map>
#include <stdexcept>

#if Windows
    #include <windows.h>
#endif

#include "shl/hash.hpp"
#include "shl/filesystem_watcher.hpp"

namespace fs = std::filesystem;

#define absolute_canonical(X)\
    fs::absolute(fs::canonical(X))

namespace std
{
    template <>
    struct hash<fs::path>
    {
        std::size_t operator()(const fs::path &k) const
        {
            const char *cstr = k.c_str();
            return hash_data(cstr, strlen(cstr));
        }
    };
}

struct watched_file
{
    fs::path path;
#if Linux
    int fd;
#endif
};

struct watched_directory
{
    fs::path path;
#if Linux
    int fd;
#endif

    std::unordered_map<fs::path, watched_file> watched_files;
    bool only_watch_files;
};

struct filesystem_watcher
{
    pthread_t thread;
    int       thread_id;
    bool      running;
    watcher_callback_f callback;

    std::unordered_map<fs::path, watched_directory> watched_directories;

#if Linux
    int inotify_fd;
    u32 timeout; // ms
#endif
};

#define print_watcher_error(MSG, ...)\
    fprintf(stderr, "filesystem_watcher %d: " MSG, __LINE__ __VA_OPT__(,) __VA_ARGS__);

// linux specific watcher
#if Linux
#include <sys/types.h>
#include <sys/inotify.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#define BUF_LEN (10 * (sizeof(inotify_event) + NAME_MAX + 1))

watcher_event_type mask_to_event(u32 mask)
{
    watcher_event_type ret = watcher_event_type::None;

    if (mask & IN_CREATE) set(ret, watcher_event_type::Created);
    if (mask & IN_MODIFY) set(ret, watcher_event_type::Modified);
    if (mask & (IN_DELETE | IN_DELETE_SELF)) set(ret, watcher_event_type::Removed);
    
    // not accurate, but as long as we're not emitting move events, this works
    if (mask & IN_MOVED_TO) set(ret, watcher_event_type::Created);
    if (mask & IN_MOVED_FROM) set(ret, watcher_event_type::Removed);
    
    return ret;
}

// _process_event and _watcher_process run in another thread
void _process_event(filesystem_watcher *watcher, inotify_event *event)
{
    watcher_event_type type = watcher_event_type::None;
    const char *path = nullptr;

    // TODO: remove
    if (event->mask == IN_IGNORED) return;
    if (event->mask == IN_Q_OVERFLOW) return;
    if (event->mask == IN_UNMOUNT) return;

    /*
    if (event->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (event->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (event->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (event->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (event->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (event->mask & IN_CREATE)        printf("IN_CREATE ");
    if (event->mask & IN_DELETE)        printf("IN_DELETE ");
    if (event->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (event->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (event->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (event->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (event->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (event->mask & IN_OPEN)          printf("IN_OPEN ");

    if (event->len > 0)
        printf("   name = %s\n", event->name);
    */

    for (auto &dp : watcher->watched_directories)
    {
        watched_directory *dir = &dp.second;

        if (dir->fd == event->wd)
        {
            if (dir->only_watch_files)
            {
                if (event->len <= 0)
                    // we only care for modified files
                    continue;

                fs::path fpath = dir->path / event->name;

                auto it2 = dir->watched_files.find(fpath);
                
                if (it2 == dir->watched_files.end())
                    continue;

                path = it2->second.path.c_str();
                type = mask_to_event(event->mask);
            }
            else
            {
                // TODO: directory events
            }
        }
        else
        {
            watched_file *file = nullptr;

            for (auto &fp : dir->watched_files)
                if (fp.second.fd == event->wd)
                {
                    file = &fp.second;
                    break;
                }

            if (file == nullptr)
                continue;

            watcher_event_type type = mask_to_event(event->mask);
            path = file->path.c_str();
            break;
        }
    }


    if (type == watcher_event_type::None || path == nullptr)
        return;

    watcher->callback(path, type);
}

void *_watcher_process(void *threadarg)
{
    filesystem_watcher *watcher = reinterpret_cast<filesystem_watcher*>(threadarg);
    u8 buffer[BUF_LEN];
    s64 length;

    while (watcher->running)
    {
        // poll for events
        pollfd pfd;
        pfd.fd = watcher->inotify_fd;
        pfd.events = POLLIN;
        pfd.revents = 0;

        u32 ret = poll(&pfd, 1, watcher->timeout);

        if (ret == 0)
            continue;

        if (ret < 0)
        {
            print_watcher_error("poll failed %s\n", strerror(errno));
            break;
        }

        length = read(watcher->inotify_fd, buffer, BUF_LEN);

        if(length < 0)
        {
            print_watcher_error("inotify read failed\n");
            break;
        }

        s64 i = 0;

        while(i < length)
        {
            inotify_event *event = reinterpret_cast<inotify_event*>(buffer + i);

            _process_event(watcher, event);

            i += sizeof(inotify_event) + event->len;
        }
    }

    return nullptr;
}
/*
#elif Windows
*/
#else
    #error "Unsupported"
#endif

void create_filesystem_watcher(filesystem_watcher **out, watcher_callback_f callback)
{
    assert(out != nullptr);
    assert(callback != nullptr);

    filesystem_watcher *watcher = new filesystem_watcher;

    watcher->thread_id = -1;
    watcher->running = false;

    watcher->callback = callback;

#if Linux
    watcher->inotify_fd = inotify_init1(O_NONBLOCK);

    if(watcher->inotify_fd < 0)
        throw std::runtime_error("could not initialize inotify");

    watcher->timeout = 50; // ms
#endif

    *out = watcher;
}

void watch_file(filesystem_watcher *watcher, const char *path)
{
    // TODO: event filter
    assert(watcher != nullptr);
    assert(path != nullptr);

#if Linux
    fs::path fpath = path;

    if (!fs::is_regular_file(fpath))
        throw std::runtime_error("not a file");

    fpath = absolute_canonical(fpath);
    fs::path parent_path = absolute_canonical(fpath.parent_path());
    
    auto it = watcher->watched_directories.find(parent_path);
    watched_directory *watched_parent = nullptr;

    if (it != watcher->watched_directories.end())
        watched_parent = &it->second;
    else
    {
        watched_parent = &watcher->watched_directories[parent_path];
        watched_parent->path = parent_path;
        watched_parent->only_watch_files = true;
        watched_parent->fd = inotify_add_watch(watcher->inotify_fd, parent_path.c_str(), IN_ALL_EVENTS);

        if (watched_parent->fd < 0)
            throw std::runtime_error("could not watch parent directory");
    }

    assert(watched_parent != nullptr);

    auto it2 = watched_parent->watched_files.find(fpath);
    
    if (it2 != watched_parent->watched_files.end())
        return;

    watched_file *watched = &watched_parent->watched_files[fpath];
    watched->path = fpath;
    watched->fd = inotify_add_watch(watcher->inotify_fd, path, IN_ALL_EVENTS);

    if (watched->fd < 0)
        throw std::runtime_error("could not watch file");
#else
    #error "Unsupported"
#endif
}

void unwatch_file(filesystem_watcher *watcher, const char *path)
{
    assert(watcher != nullptr);
    assert(path != nullptr);

#if Linux
    fs::path fpath = absolute_canonical(path);
    fs::path parent_path = absolute_canonical(fpath.parent_path());

    auto it = watcher->watched_directories.find(parent_path);

    if (it == watcher->watched_directories.end())
        return;

    watched_directory *watched_parent = &it->second;

    auto it2 = watched_parent->watched_files.find(fpath);
    
    if (it2 == watched_parent->watched_files.end())
        return;

    watched_file *watched = &it2->second;
    inotify_rm_watch(watcher->inotify_fd, watched->fd);

    watched_parent->watched_files.erase(it2);

    if (watched_parent->only_watch_files
     && watched_parent->watched_files.empty())
    {
        inotify_rm_watch(watcher->inotify_fd, watched_parent->fd);
        watcher->watched_directories.erase(it);
    }
#else
    #error "Unsupported"
#endif
}

void start_filesystem_watcher(filesystem_watcher *watcher)
{
    assert(watcher != nullptr);

    if (watcher->running)
        return;

    watcher->thread_id = pthread_create(
        &watcher->thread,
        NULL,
        _watcher_process,
        watcher);

    if(watcher->thread_id != 0)
        throw std::runtime_error("could not start thread");

    watcher->running = true;
}

void stop_filesystem_watcher(filesystem_watcher *watcher)
{
    assert(watcher != nullptr);

    if (!watcher->running)
        return;

    watcher->running = false;
    void *ret;
    pthread_join(watcher->thread, &ret);
}

void destroy_filesystem_watcher(filesystem_watcher *watcher)
{
    assert(watcher != nullptr);

    stop_filesystem_watcher(watcher);

#if Linux
    close(watcher->inotify_fd);
#endif

    delete watcher;
}
