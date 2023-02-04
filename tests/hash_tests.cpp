
#include <iostream>

#include <t1/t1.hpp>
#include "shl/hash.hpp"

define_test(hash_data_hashes_data)
{
    assert_equal(hash_data("hello", 5), 2762169579135187400ULL); // arbitrary
    assert_equal(hash_data("hello", 5), 2762169579135187400ULL);
    assert_equal(hash_data("hello", 5), 2762169579135187400ULL);

    assert_equal(hash_data("hell", 4), 11807067753061997435ULL);
    assert_equal(hash_data("hell", 4), 11807067753061997435ULL);
    assert_equal(hash_data("hell", 4), 11807067753061997435ULL);
}

define_test(hash_raw_hashes_raw_data)
{
    u32 a = 50; 
    u32 b = 50; 

    assert_equal(hash_raw(&a), 3824579390589417690ULL);
    assert_equal(hash_raw(&b), 3824579390589417690ULL);

    u64 c = 50; 

    assert_equal(hash_raw(&c), 5070505279687853236ULL);
}

define_default_test_main();
