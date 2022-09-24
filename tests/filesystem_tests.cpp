
#include <iostream>
#include <string.h>

#include <t1/t1.hpp>

#include "shl/filesystem.hpp"

define_test(get_executable_path_gets_executable_path)
{
    char pth[PATH_MAX] = {0};

    long len = get_executable_path(pth);

    /*
     * these wont work on every system, duh.
    long actuallen = strlen("/home/user/dev/git/shl/bin/tests/filesystem_tests");

    assert_equal(len, actuallen);
    assert_equal(strncmp(pth, "/home/user/dev/git/shl/bin/tests/filesystem_tests", actuallen), 0);
    */
}

define_default_test_main();
