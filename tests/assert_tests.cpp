
#include "t1/t1.hpp"
#include "shl/assert.hpp"

define_test(assert_test)
{
    assert(5 > 4);
}

define_default_test_main();
