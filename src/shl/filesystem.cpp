
// v1.0
#include "shl/platform.hpp"
#include "shl/filesystem.hpp"

/*
#ifdef _WIN32
#include <windows.h>
#else
*/
#include <unistd.h>
// #endif

long get_executable_path(char *out)
{
#if Linux
    return readlink("/proc/self/exe", out, PATH_MAX);
/*
#elif Windows
    wchar_t path[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
*/
#else
    #error "unsupported"
#endif
}

std::string get_executable_path()
{
    char pth[PATH_MAX] = {0};

    long len = get_executable_path(pth); 

    if (len < 0)
        return "";

    return std::string(pth);
}
