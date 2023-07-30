
#include <t1/t1.hpp>

#include "shl/compare.hpp"

define_test(clamp_clamps_between_two_values)
{
    int min = 10;
    int max = 200;

    assert_equal(Clamp(50, min, max), 50);
    assert_equal(Clamp(5, min, max), 10);
    assert_equal(Clamp(500, min, max), 200);
}

define_test(compare_clamp_ascending_compares_between_two_values)
{
    int min = 10;
    int max = 200;

    assert_equal(compare_clamp_ascending(50, min, max), 0);
    assert_equal(compare_clamp_ascending(5, min, max), -1);
    assert_equal(compare_clamp_ascending(500, min, max), 1);
}

define_default_test_main();
