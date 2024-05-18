
#define t1_use_stdlib 1
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

define_test(ceil_multiple_rounds_up_to_multiple)
{
    assert_equal(ceil_multiple(0, 5), 0);
    assert_equal(ceil_multiple(1, 5), 5);
    assert_equal(ceil_multiple(2, 5), 5);
    assert_equal(ceil_multiple(3, 5), 5);
    assert_equal(ceil_multiple(4, 5), 5);
    assert_equal(ceil_multiple(5, 5), 5);
    assert_equal(ceil_multiple(6, 5), 10);
}

define_test(ceil_multiple2_rounds_up_to_multiple_of_2)
{
    assert_equal(ceil_multiple2(0, 8), 0);
    assert_equal(ceil_multiple2(1, 8), 8);
    assert_equal(ceil_multiple2(2, 8), 8);
    assert_equal(ceil_multiple2(3, 8), 8);
    assert_equal(ceil_multiple2(4, 8), 8);
    assert_equal(ceil_multiple2(5, 8), 8);
    assert_equal(ceil_multiple2(6, 8), 8);
    assert_equal(ceil_multiple2(7, 8), 8);
    assert_equal(ceil_multiple2(8, 8), 8);
    assert_equal(ceil_multiple2(9, 8), 16);

    assert_equal(ceil_multiple2(0, 1024), 0);
    assert_equal(ceil_multiple2(1, 1024), 1024);
    assert_equal(ceil_multiple2(1023, 1024), 1024);
    assert_equal(ceil_multiple2(1024, 1024), 1024);
    assert_equal(ceil_multiple2(1025, 1024), 2048);
    
    assert_equal(ceil_multiple2(2047, 1024), 2048);
    assert_equal(ceil_multiple2(2048, 1024), 2048);
    assert_equal(ceil_multiple2(2049, 1024), 3072);
}

define_test(floor_multiple_rounds_down_to_multiple)
{
    assert_equal(floor_multiple(0, 5), 0);
    assert_equal(floor_multiple(1, 5), 0);
    assert_equal(floor_multiple(2, 5), 0);
    assert_equal(floor_multiple(3, 5), 0);
    assert_equal(floor_multiple(4, 5), 0);
    assert_equal(floor_multiple(5, 5), 5);
    assert_equal(floor_multiple(6, 5), 5);
    assert_equal(floor_multiple(7, 5), 5);
    assert_equal(floor_multiple(8, 5), 5);
    assert_equal(floor_multiple(9, 5), 5);
    assert_equal(floor_multiple(10, 5), 10);
    assert_equal(floor_multiple(11, 5), 10);
}

define_test(floor_multiple2_rounds_down_to_multiple_of_2)
{
    assert_equal(floor_multiple2(0, 8),  0);
    assert_equal(floor_multiple2(1, 8),  0);
    assert_equal(floor_multiple2(2, 8),  0);
    assert_equal(floor_multiple2(3, 8),  0);
    assert_equal(floor_multiple2(4, 8),  0);
    assert_equal(floor_multiple2(5, 8),  0);
    assert_equal(floor_multiple2(6, 8),  0);
    assert_equal(floor_multiple2(7, 8),  0);
    assert_equal(floor_multiple2(8, 8),  8);
    assert_equal(floor_multiple2(9, 8),  8);
    assert_equal(floor_multiple2(10, 8), 8);
    assert_equal(floor_multiple2(11, 8), 8);
    assert_equal(floor_multiple2(12, 8), 8);
    assert_equal(floor_multiple2(13, 8), 8);
    assert_equal(floor_multiple2(14, 8), 8);
    assert_equal(floor_multiple2(15, 8), 8);
    assert_equal(floor_multiple2(16, 8), 16);
    assert_equal(floor_multiple2(17, 8), 16);

    assert_equal(floor_multiple2(0, 1024), 0);
    assert_equal(floor_multiple2(1, 1024), 0);
    assert_equal(floor_multiple2(1023, 1024), 0);
    assert_equal(floor_multiple2(1024, 1024), 1024);
    assert_equal(floor_multiple2(1025, 1024), 1024);
    
    assert_equal(floor_multiple2(2047, 1024), 1024);
    assert_equal(floor_multiple2(2048, 1024), 2048);
    assert_equal(floor_multiple2(2049, 1024), 2048);
}

define_default_test_main();
