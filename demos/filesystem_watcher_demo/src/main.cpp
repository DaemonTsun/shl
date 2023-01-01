
#include <stdio.h>
#include "shl/filesystem_watcher.hpp"

void callback(const char *path, watcher_event_type event)
{
    printf("%d %s\n", value(event), path);
}

int main(int argc, char **argv)
{
    filesystem_watcher *watcher;
    create_filesystem_watcher(&watcher, callback);

    for (int i = 1; i < argc; ++i)
        watch_file(watcher, argv[i]);

    start_filesystem_watcher(watcher);

    char c = '\0';

    while (true)
    {
        c = getchar();

        if (c == 'q' || c < 0)
            break;

        if (c == 'w')
            for (int i = 1; i < argc; ++i)
                watch_file(watcher, argv[i]);

        if (c == 'u')
            for (int i = 1; i < argc; ++i)
                unwatch_file(watcher, argv[i]);
    }

    destroy_filesystem_watcher(watcher);

    return 0;
}
