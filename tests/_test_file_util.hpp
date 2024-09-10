
#pragma once

// utility for test
#include "shl/string.hpp"

static const sys_char *get_executable_path()
{
    static sys_char pth[4096] = {0};
#if Windows
    auto len = GetModuleFileName(nullptr, (sys_native_char*)pth, 4095);

    assert(len > 0 && len < 4095);
    pth[len] = '\0';

    return pth;
#else

    if (readlink("/proc/self/exe", pth, 4095) < 0)
        return nullptr;
    
    return pth;
#endif
}

static sys_string get_filepath(const sys_char *file)
{
    sys_string ret{};

    const sys_char *exep = get_executable_path();

    if (exep == nullptr)
        return ret;

    string_set(&ret, exep);

    s64 idx = string_last_index_of(ret, SYS_CHAR('/'));

#if Windows
    if (idx < 0)
        idx = string_last_index_of(ret, SYS_CHAR('\\'));
#endif

    if (idx < 0)
        idx = ret.size;

    ret.size = idx;

#if Windows
    string_append(&ret, SYS_CHAR("\\"));
#else
    string_append(&ret, SYS_CHAR("/"));
#endif
    string_append(&ret, file);

    return ret;
}
