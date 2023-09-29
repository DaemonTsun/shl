
#include <t1/t1.hpp>

#include "shl/math.hpp"

define_test(modulo_calculates_modulo)
{
    assert_equal(modulo(5, 2), 1);
    assert_equal(modulo(4, 2), 0);
    assert_equal(modulo(10.f, 4.f), 2.f);
}

define_test(wrap_number_wraps_a_number_within_range)
{
    assert_equal(wrap_number(0, 0, 0), 0);
    assert_equal(wrap_number(-10, 0, 360), 351);
    assert_equal(wrap_number(360,   0, 359), 0);
    assert_equal(wrap_number(720.f, 0.f, 359.f), 0.f);
    assert_equal(wrap_number(900.f, 0.f, 359.f), 180.f);
    assert_equal(wrap_number(1080.f,  0.f, 359.f), 0.f);
}

define_default_test_main();
