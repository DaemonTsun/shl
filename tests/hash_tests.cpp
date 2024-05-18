
#include <iostream>
#include <string.h>

#include <t1/t1.hpp>
#include "shl/hash.hpp"

define_test(hash_data_hashes_data)
{
    hash_t hello = hash_data("hello", 5);

    assert_equal(hash_data("hello", 5), hello);
    assert_equal(hash_data("hello", 5), hello);
    assert_equal(hash_data("hello", 5), hello);

    assert_not_equal(hash_data("hell", 4), hello);
}

define_test(hash_raw_hashes_raw_data)
{
    u32 a = 50; 
    u32 b = 50; 

    hash_t a_hash = hash_raw(&a);
    hash_t b_hash = hash_raw(&b);

    assert_equal(a_hash, b_hash);
}

define_test(hash_hashes_basic_types)
{
    u8 u8a = 1;
    u8 u8b = 1;
    u8 u8c = 2;

    assert_equal(hash(&u8a), hash(&u8b));
    assert_not_equal(hash(&u8a), hash(&u8c));

    u16 u16a = 1;
    u16 u16b = 1;
    u16 u16c = 2;

    assert_equal(hash(&u16a), hash(&u16b));
    assert_not_equal(hash(&u16a), hash(&u16c));

    u32 u32a = 1;
    u32 u32b = 1;
    u32 u32c = 2;

    assert_equal(hash(&u32a), hash(&u32b));
    assert_not_equal(hash(&u32a), hash(&u32c));

    u64 u64a = 1;
    u64 u64b = 1;
    u64 u64c = 2;

    assert_equal(hash(&u64a), hash(&u64b));
    assert_not_equal(hash(&u64a), hash(&u64c));

    s8 s8a = 1;
    s8 s8b = 1;
    s8 s8c = 2;

    assert_equal(hash(&s8a), hash(&s8b));
    assert_not_equal(hash(&s8a), hash(&s8c));

    s16 s16a = 1;
    s16 s16b = 1;
    s16 s16c = 2;

    assert_equal(hash(&s16a), hash(&s16b));
    assert_not_equal(hash(&s16a), hash(&s16c));

    s32 s32a = 1;
    s32 s32b = 1;
    s32 s32c = 2;

    assert_equal(hash(&s32a), hash(&s32b));
    assert_not_equal(hash(&s32a), hash(&s32c));

    s64 s64a = 1;
    s64 s64b = 1;
    s64 s64c = 2;

    assert_equal(hash(&s64a), hash(&s64b));
    assert_not_equal(hash(&s64a), hash(&s64c));

    float floata = 1.f;
    float floatb = 1.f;
    float floatc = 2.f;

    assert_equal(hash(&floata), hash(&floatb));
    assert_not_equal(hash(&floata), hash(&floatc));

    double doublea = 1.0;
    double doubleb = 1.0;
    double doublec = 2.0;

    assert_equal(hash(&doublea), hash(&doubleb));
    assert_not_equal(hash(&doublea), hash(&doublec));

    long double long_doublea = 1.0L;
    long double long_doubleb = 1.0L;
    long double long_doublec = 2.0L;

    // valgrind complains about long doubles not being initialized if
    // we don't set all bytes explicitly here for some reason.
    memset(&long_doublea, 0, sizeof(long double));
    memset(&long_doubleb, 0, sizeof(long double));
    memset(&long_doublec, 0xff, sizeof(long double));

    assert_equal(hash(&long_doublea), hash(&long_doubleb));
    assert_not_equal(hash(&long_doublea), hash(&long_doublec));

    const void *ptr_a = &u8a;
    const void *ptr_b = &u8a;
    const void *ptr_c = &u8b;

    assert_equal(hash(&ptr_a), hash(&ptr_b));
    assert_not_equal(hash(&ptr_a), hash(&ptr_c));
}

define_default_test_main();
