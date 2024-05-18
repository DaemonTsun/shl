
#define t1_use_stdlib 1
#include "t1/t1.hpp"
#include "shl/assert.hpp"

define_test(assert_test)
{
    [[maybe_unused]] volatile int x = 5;
    assert(x > 4);
}

define_default_test_main();
