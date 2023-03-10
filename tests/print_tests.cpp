
#include "shl/print.hpp"

int main(int argc, char **argv)
{
    // too lazy to test by writing to a FILE*.

    tprint("hello %\n", "world");
    tprint(stdout, "hello %\n", "world");
    tprint(stderr, "hello %\n", "world");
}
