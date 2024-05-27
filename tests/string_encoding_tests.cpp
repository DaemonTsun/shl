
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

define_test(utf8_encode_encodes_codepoint_as_utf8)
{
    u32 input = L'彁';
    char conv[5] = {0};
    fill_memory(conv, (char)0x7f, 5);

    s64 count = utf8_encode(input, conv);

    assert_not_equal(count, 0);
    assert_equal(count, 3);
    assert_equal(conv[0], (char)0xe5);
    assert_equal(conv[1], (char)0xbd);
    assert_equal(conv[2], (char)0x81);

    // outside of written data
    assert_equal(conv[3], (char)0x7f);
}

define_test(utf8_encode_encodes_codepoint_as_utf8_2)
{
    u32 input = 0x0001f34c; // 🍌 (banana)
    char conv[5] = {0};
    fill_memory(conv, (char)0x7f, 5);

    s64 count = utf8_encode(input, conv);

    assert_not_equal(count, 0);
    assert_equal(count, 4);
    assert_equal(conv[0], (char)0xf0);
    assert_equal(conv[1], (char)0x9f);
    assert_equal(conv[2], (char)0x8d);
    assert_equal(conv[3], (char)0x8c);

    // outside of written data
    assert_equal(conv[4], (char)0x7f);
}

define_test(utf16_encode_encodes_codepoint_as_utf16)
{
    u32 input = L'彁';
    u16 conv[3] = {0x7f7f, 0x7f7f, 0x7f7f};

    s64 count = utf16_encode(input, conv);

    assert_not_equal(count, 0);
    assert_equal(count, 1);
    assert_equal(conv[0], (u16)0x5f41);

    // outside of written data
    assert_equal((u16)conv[1], (u16)0x7f7f);
}

define_test(utf16_encode_encodes_codepoint_as_utf16_2)
{
    u32 input = 0x0001f34c;
    u16 conv[3] = {0x7f7f, 0x7f7f, 0x7f7f};

    s64 count = utf16_encode(input, conv);

    assert_not_equal(count, 0);
    assert_equal(count, 2);
    assert_equal(conv[0], (u16)0xd83c);
    assert_equal(conv[1], (u16)0xdf4c);

    // outside of written data
    assert_equal((u16)conv[2], (u16)0x7f7f);
}

define_test(utf8_encode_string_encodes_codepoints_as_utf8_string)
{
    const char *_text = u8"hello 彁 Привет";
    u32 input[32] = {0};
    s64 input_length = utf8_decode_string_safe(_text, strlen(_text), input, 31);

    char conv[32] = {0};

    s64 count = utf8_encode_string(input, input_length, conv, 32);

    assert_equal(count, 22);
    assert_equal(conv[6], (char)0xe5);
    assert_equal(conv[7], (char)0xbd);
    assert_equal(conv[8], (char)0x81);
    assert_equal(conv[9], ' ');
    assert_equal(conv[22], '\0');

    fill_memory(conv, 0, 32);

    // 8 = one less character than is required for 彁, meaning the character
    // will not be written at all.
    count = utf8_encode_string(input, input_length, conv, 8);

    assert_equal(count, 6);
    assert_equal(conv[4], 'o');
    assert_equal(conv[5], ' ');
    assert_equal(conv[6], '\0');
}

define_test(utf16_encode_string_encodes_codepoints_as_utf8_string)
{
    const u16 *_text = (const u16*)u"hello 彁 Привет";
    u32 input[32] = {0};
    s64 input_length = utf16_decode_string_safe(_text, 14, input, 31);

    u16 conv[32] = {0};

    s64 count = utf16_encode_string(input, input_length, conv, 32);

    assert_equal(count, 14);
    assert_equal(conv[6], (u16)0x5f41);
    assert_equal(conv[7], ' ');
    assert_equal(conv[14], '\0');
}

define_test(utf8_to_utf16_encodes_utf8_string_to_utf16)
{
    const char *input = u8"hello 彁 Привет";
    u16 conv[32] = {0};

    s64 count = utf8_to_utf16(input, 22, conv, 32);

    assert_equal(count, 14);
    assert_equal(conv[14], 0);
    assert_equal(memcmp(conv, u"hello 彁 Привет", 14 * 2), 0);
}

define_test(utf16_to_utf8_encodes_utf16_string_to_utf8)
{
    const u16 *input = (const u16*)u"hello 彁 Привет";
    char conv[32] = {0};

    s64 count = utf16_to_utf8(input, 14, conv, 32);

    assert_equal(count, 22);
    assert_equal(conv[22], 0);
    assert_equal(memcmp(conv, u8"hello 彁 Привет", 22), 0);
}

define_test(utf16_bytes_required_from_utf8_returns_numbers_of_utf16_bytes_required_to_store_utf8_string)
{
    assert_equal(utf16_bytes_required_from_utf8(u8"hello 彁 Привет", 22), 14 * 2);
    assert_equal(utf16_bytes_required_from_utf8(u8"hello 🍌 Привет", 23), 15 * 2);
    assert_equal(utf16_bytes_required_from_utf8(u8"", 0), 0);
    assert_equal(utf16_bytes_required_from_utf8(u8"", 255), 0);
}

define_test(utf8_bytes_required_from_utf16_returns_numbers_of_utf8_bytes_required_to_store_utf16_string)
{
    assert_equal(utf8_bytes_required_from_utf16((const u16*)u"hello 彁 Привет", 14), 22);
    assert_equal(utf8_bytes_required_from_utf16((const u16*)u"hello 🍌 Привет", 15), 23);
    assert_equal(utf8_bytes_required_from_utf16((const u16*)u"", 0), 0);
    assert_equal(utf8_bytes_required_from_utf16((const u16*)u"", 255), 0);
}

define_default_test_main();
