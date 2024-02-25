
#include "shl/print.hpp"

int main(int argc, char **argv)
{
    tprint("hello %\n", "world");
    tprint(stdout_handle(), "hello %\n", "world");
    tprint(stderr_handle(), "hello %\n", "world");

    tprint(L"wide hello %\n", L"world");
}
