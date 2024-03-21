
#include "shl/print.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    tprint("hello %\n", "world");
    tprint(stdout_handle(), "hello %\n", "world");
    tprint(stderr_handle(), "hello %\n", "world");

    tprint(L"wide hello %\n", L"world");
}
