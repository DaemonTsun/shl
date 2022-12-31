
// TODO: windows version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include <map>
#include <stdexcept>

#if Windows
    #include <windows.h>
#endif

#include "shl/platform.hpp"
#include "shl/filesystem_watcher.hpp"

struct filesystem_watcher
{
    pthread_t  thread;
    int        thread_id;
    bool       running;
    watcher_callback_f callback;

#if Linux
    int inotify_fd;
    u32 timeout; // ms

    std::map<int, const char *> watched_files;
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
    if (mask & IN_DELETE) set(ret, watcher_event_type::Removed);
    
    return ret;
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
            // TODO: sometimes files are "modified" by copying a temporary
            // file over them. right now this fires event type
            // IN_DELETE_SELF, but maybe we should emit Modified instead.
            inotify_event *event = reinterpret_cast<inotify_event*>(buffer + i);
            watcher_event_type type = mask_to_event(event->mask);

            if (type != watcher_event_type::None)
            {
                const char *pth = watcher->watched_files[event->wd];
                watcher->callback(pth, type);
            }

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
    assert(watcher != nullptr);
    assert(path != nullptr);
    
#if Linux
    for (auto &p : watcher->watched_files)
    {
        if (strcmp(path, p.second) == 0)
            return;
    }

    // TODO: event filter
    int wd = inotify_add_watch(watcher->inotify_fd, path, IN_ALL_EVENTS);

    if (wd < 0)
        throw std::runtime_error("could not watch file");
        
    watcher->watched_files[wd] = path;
#else
    #error "Unsupported"
#endif
}

void unwatch_file(filesystem_watcher *watcher, const char *path)
{
    assert(watcher != nullptr);
    assert(path != nullptr);

#if Linux
    int wd = -1;

    for (auto &p : watcher->watched_files)
    {
        if (strcmp(path, p.second) == 0)
        {
            wd = p.first;
            break;
        }
    }

    if (wd == -1)
        return;

    inotify_rm_watch(watcher->inotify_fd, wd);

    watcher->watched_files.erase(wd);
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
