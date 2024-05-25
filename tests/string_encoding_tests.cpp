
#include "t1/t1.hpp"
#include "shl/string_encoding.hpp"
#include "shl/print.hpp"

define_test(utf8_decode_decodes_utf8_to_32bit_codepoints)
{
    // make sure string is a multiple of 4 bytes long.
    // in this case 彁 is 3 bytes long, + the null terminating character = 4
    const char *input = u8"彁";
    u32 cp = 0;
    int error = 0;
    const char *next = utf8_decode(input, &cp, &error);

    assert_equal(cp, (u32)L'彁'); // 0x5f41
    assert_equal(cp, (u32)0x00005f41);
    assert_equal(error, 0);
    assert_equal(next, input + 3); // 彁 is 3 bytes long in UTF-8
}

define_test(utf8_decode_decodes_utf8_to_32bit_codepoints2)
{
    char input[] = {(char)0xd0, (char)0x9f};
    u32 cp = 0;
    int error = 0;
    const char *next = utf8_decode(input, &cp, &error);

    assert_equal(cp, (u32)L'П');
    assert_equal(cp, (u32)0x041f);
    assert_equal(error, 0);
    assert_equal(next, input + 2);
}

define_test(utf8_decode_string_decodes_utf8_to_32bit_string)
{
    const char *input = u8"hello 彁 Привет\0\0\0";
    u32 conv[32] = {0};

    s64 count = utf8_decode_string(input, strlen(input), conv, 31);

    assert_not_equal(count, -1);
    assert_equal(count, 14);
    assert_equal(conv[6], (u32)L'彁');
    assert_equal(conv[8], (u32)L'П');

    // note: utf8_decode_string does not place a null terminating character at the
    // end of the decoding, this assert shows that the function did not write
    // over the count.
    assert_equal(conv[14], (u32)L'\0');
}

define_test(utf16_decode_decodes_utf16_to_32bit_codepoints)
{
    // Dear C++ people:
    // why is u8"abc" a UTF-8  literal
    // but     u"abc" a UTF-16 literal?
    // That does not make sense.
    const u16 *input = (const u16*)u"彁";
    u32 cp = 0;
    int error = 0;
    const u16 *next = utf16_decode(input, &cp, &error);

    assert_equal(cp, (u32)L'彁'); // 0x5f41
    assert_equal(cp, (u32)0x00005f41);
    assert_equal(error, 0);
    assert_equal(next, input + 1); // 彁 is 2 bytes (1 u16) long in UTF-16
}

define_test(utf16_decode_string_decodes_utf16_to_32bit_string)
{
    const u16 *input = (const u16*)u"hello 彁 Привет\0\0\0";
    u32 conv[32] = {0};

    s64 count = utf16_decode_string(input, 22, conv, 31);

    assert_not_equal(count, -1);
    assert_equal(count, 14);
    assert_equal(conv[6], (u32)L'彁');
    assert_equal(conv[8], (u32)L'П');

    // note: utf8_decode_string does not place a null terminating character at the
    // end of the decoding, this assert shows that the function did not write
    // over the count.
    assert_equal(conv[14], (u32)L'\0');
}

define_test(utf8_decode_string_safe_decodes_utf8_to_32bit_string)
{
    const char *input = u8"hello 彁 Привет";
    u32 conv[32] = {0};

    s64 count = utf8_decode_string_safe(input, strlen(input), conv, 31);

    assert_not_equal(count, -1);
    assert_equal(count, 14);
    assert_equal(conv[6], (u32)L'彁');
    assert_equal(conv[8], (u32)L'П');
    assert_equal(conv[14], (u32)L'\0');
}

define_test(utf16_decode_string_safe_decodes_utf16_to_32bit_string)
{
    const u16 *input = (const u16*)u"hello 彁 Привет";
    u32 conv[32] = {0};

    s64 count = utf16_decode_string_safe(input, 22, conv, 31);

    assert_not_equal(count, -1);
    assert_equal(count, 14);
    assert_equal(conv[6], (u32)L'彁');
    assert_equal(conv[8], (u32)L'П');

    // note: utf8_decode_string does not place a null terminating character at the
    // end of the decoding, this assert shows that the function did not write
    // over the count.
    assert_equal(conv[14], (u32)L'\0');
}

define_default_test_main();
