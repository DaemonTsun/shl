
#define t1_use_stdlib 1
#include <t1/t1.hpp>

#include "shl/endian.hpp"

define_test(endianness_test)
{
    u32 value = 0x12345678;

    u32 big_endian_value = force_big_endian(value);

    u8 *values = (u8*)&big_endian_value;

    assert_equal(values[0], 0x12);
    assert_equal(values[1], 0x34);
    assert_equal(values[2], 0x56);
    assert_equal(values[3], 0x78);
}

define_default_test_main();
