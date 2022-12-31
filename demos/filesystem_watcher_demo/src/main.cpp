
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

    watch_file(watcher, "a");

    start_filesystem_watcher(watcher);

    while (getchar() != '\n')
        continue;

    destroy_filesystem_watcher(watcher);

    return 0;
}
