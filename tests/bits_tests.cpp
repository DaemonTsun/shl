
#include <t1/t1.hpp>

#include "shl/number_types.hpp"
#include "shl/bits.hpp"

define_test(rotl_rotates_left)
{
    u8 val      = 0b00000100;
    u8 expected = 0b00001000;

    assert_equal(rotl(val, 1), expected);
}

define_test(rotl_rotates_left2)
{
    u8 val      = 0b01000000;
    u8 expected = 0b00000001;

    assert_equal(rotl(val, 2), expected);
}

define_test(rotr_rotates_right)
{
    u8 val      = 0b00000100;
    u8 expected = 0b00000010;

    assert_equal(rotr(val, 1), expected);
}

define_test(rotr_rotates_right2)
{
    u8 val      = 0b00000010;
    u8 expected = 0b10000000;

    assert_equal(rotr(val, 2), expected);
}

define_test(bitmask_gets_bitmask)
{
    assert_equal(bitmask<u8>(0, 2), 0b00000111);
    assert_equal(bitmask<u8>(0, 1), 0b00000011);
    assert_equal(bitmask<u8>(0, 0), 0b00000001);
    assert_equal(bitmask<u8>(2, 3), 0b00001100);
}

define_test(bitrange_gets_bitrange)
{
    assert_equal(bitrange(0x00ff, 0, 0),  0x1);
    assert_equal(bitrange(0x00ff, 0, 1),  0x3);
    assert_equal(bitrange(0x00ff, 0, 3),  0xf);
    assert_equal(bitrange(0x00ff, 0, 7),  0xff);
    assert_equal(bitrange(0x01ff, 0, 7),  0xff);
    assert_equal(bitrange(0x00ff, 0, 8),  0xff);
    assert_equal(bitrange(0x01ff, 1, 8),  0xff);
    assert_equal(bitrange(0x0ff0, 4, 11), 0xff);
    assert_equal(bitrange(0xff00, 8, 15), 0xff);
}

define_test(next_exp2_gets_next_power_of_2)
{
    assert_equal(ceil_exp2(0), 1);
    assert_equal(ceil_exp2(1), 1);
    assert_equal(ceil_exp2(2), 2);
    assert_equal(ceil_exp2(3), 4);
    assert_equal(ceil_exp2(4), 4);
    assert_equal(ceil_exp2(5), 8);
    assert_equal(ceil_exp2(7), 8);
    assert_equal(ceil_exp2(8), 8);
    assert_equal(ceil_exp2(9), 16);
    assert_equal(ceil_exp2<u32>(1u << 31), 1u << 31);
}

define_default_test_main();
