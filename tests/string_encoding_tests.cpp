
#include "t1/t1.hpp"
#include "shl/string_encoding.hpp"
#include "shl/print.hpp"

define_test(utf8_decode_decodes_utf8_into_32bit_codepoints)
{
    const char *input = u8"彁\0\0\0";
    u32 cp = 0;
    int error = 0;
    const char *next = utf8_decode(input, &cp, &error);

    assert_equal(cp, (u32)L"彁"[0]); // 0x5f41
    assert_equal(cp, (u32)0x00005f41);
    assert_equal(error, 0);
    assert_equal(next, input + 3); // 彁 is 3 bytes long in UTF-8
}

define_default_test_main();
