
#define t1_use_stdlib 1
#include "shl/murmur_hash.hpp"
#include <t1/t1.hpp>

define_test(file_hash_returns_file_hash)
{
    assert_equal(__FILE_HASH__, __FILE_HASH__);
}

define_test(line_hash_returns_line_hash)
{
    assert_equal(__LINE_HASH__, __LINE_HASH__);

    u32 hash1 = __LINE_HASH__;
    u32 hash2 = __LINE_HASH__;
    // not same line
    assert_not_equal(hash1,
                     hash2);
}

define_default_test_main();
