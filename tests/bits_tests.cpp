
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
    assert_equal(bitmask<u8>(0, 7), 0b11111111);
}

define_test(bitmask_between_values_gets_bitmask_between_values)
{
    assert_equal(bitmask_between_values<u32>(0, 2),   0b00000011u);
    assert_equal(bitmask_between_values<u32>(0, 1),   0b00000001u);
    assert_equal(bitmask_between_values<u32>(0, 0),   0b00000000u);
    assert_equal(bitmask_between_values<u32>(4, 8),   0b00001100u);
    assert_equal(bitmask_between_values<u32>(4, 32),  0b00111100u);
    assert_equal(bitmask_between_values<u32>(0, 128), 0b11111111u);
}

define_test(bitrange_gets_bitrange)
{
    assert_equal(bitrange(0x00ff, 0, 0),  0x01);
    assert_equal(bitrange(0x00ff, 0, 1),  0x03);
    assert_equal(bitrange(0x00ff, 0, 3),  0x0f);
    assert_equal(bitrange(0x00ff, 0, 7),  0xff);
    assert_equal(bitrange(0x01ff, 0, 7),  0xff);
    assert_equal(bitrange(0x00ff, 0, 8),  0xff);
    assert_equal(bitrange(0x01ff, 1, 8),  0xff);
    assert_equal(bitrange(0x0ff0, 4, 11), 0xff);
    assert_equal(bitrange(0xff00, 8, 15), 0xff);
    assert_equal(bitrange(0xabcdef01, 0, 30), 0x2bcdef01u);
    assert_equal(bitrange(0xabcdef01, 0, 31), 0xabcdef01u);
}

define_test(floor_exp2_gets_previous_power_of_2)
{
    assert_equal(floor_exp2(0), 0);
    assert_equal(floor_exp2(1), 1);
    assert_equal(floor_exp2(2), 2);
    assert_equal(floor_exp2(3), 2);
    assert_equal(floor_exp2(4), 4);
    assert_equal(floor_exp2(5), 4);
    assert_equal(floor_exp2(7), 4);
    assert_equal(floor_exp2(8), 8);
    assert_equal(floor_exp2(9), 8);
    assert_equal(floor_exp2<u32>(1u << 31), 1u << 31);
    assert_equal(floor_exp2<u64>(1ull << 63), 1ull << 63);
}

define_test(ceil_exp2_gets_next_power_of_2)
{
    assert_equal(ceil_exp2(0), 0);
    assert_equal(ceil_exp2(1), 1);
    assert_equal(ceil_exp2(2), 2);
    assert_equal(ceil_exp2(3), 4);
    assert_equal(ceil_exp2(4), 4);
    assert_equal(ceil_exp2(5), 8);
    assert_equal(ceil_exp2(7), 8);
    assert_equal(ceil_exp2(8), 8);
    assert_equal(ceil_exp2(9), 16);
    assert_equal(ceil_exp2<u32>(1u << 31), 1u << 31);
    assert_equal(ceil_exp2<u64>(1ull << 63), 1ull << 63);
}

define_test(ceil_exp_gets_next_power_of_N)
{
    assert_equal(ceil_exp<8>(0), 1);
    assert_equal(ceil_exp<8>(1), 1);
    assert_equal(ceil_exp<8>(2), 8);
    assert_equal(ceil_exp<8>(6), 8);
    assert_equal(ceil_exp<8>(7), 8);
    assert_equal(ceil_exp<8>(8), 8);
    assert_equal(ceil_exp<8>(9), 64);

    assert_equal(ceil_exp(0, 8), 1);
    assert_equal(ceil_exp(1, 8), 1);
    assert_equal(ceil_exp(2, 8), 8);
    assert_equal(ceil_exp(6, 8), 8);
    assert_equal(ceil_exp(7, 8), 8);
    assert_equal(ceil_exp(8, 8), 8);
    assert_equal(ceil_exp(9, 8), 64);
}

define_test(bit_log_gets_integer_log)
{
    assert_equal(bit_log<1>(0), 0); // 0000
    assert_equal(bit_log<1>(1), 0); // 0001
    assert_equal(bit_log<1>(2), 1); // 0010
    assert_equal(bit_log<1>(4), 2); // 0100
    assert_equal(bit_log<1>(8), 3); // 0100
}

define_test(is_pow2_checks_if_number_is_pow2)
{
    assert_equal(is_pow2(0), false);
    assert_equal(is_pow2(1), true);
    assert_equal(is_pow2(2), true);
    assert_equal(is_pow2(3), false);
    assert_equal(is_pow2(4), true);
    assert_equal(is_pow2(5), false);
    assert_equal(is_pow2(6), false);
    assert_equal(is_pow2(7), false);
    assert_equal(is_pow2(8), true);
}

define_default_test_main();
