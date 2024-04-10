
// TODO: find out how to get pagesize, like actually.
// https://codebrowser.dev/glibc/include/asm-generic/param.h.html#10
#include <unistd.h>
#include "shl/impl/linux/sysinfo.hpp"

s64 get_pagesize()
{
    return (s64)sysconf(_SC_PAGESIZE);
}
