
#ifndef NDEBUG
#include "shl/io.hpp"
#include "shl/assert.hpp"

void _print_assert_msg(const char *msg, int len)
{
    io_write(stderr_handle(), msg, len);
}
#endif
