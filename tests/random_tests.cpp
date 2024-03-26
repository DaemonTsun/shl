
#include "shl/random.hpp"
#include "t1/t1.hpp"

#define LU(X) U64_LIT(X)

#define SEED64 LU(0x3243f6a8885a308d)

#define BOUND_TEST_COUNT 100000

define_test(mt19937_tests)
{
    // https://raw.githubusercontent.com/skeeto/scratch/master/mt19937/test.c

    #define SEED64 LU(0x3243f6a8885a308d)

    static const u64 expect64[30] = {
        LU(0x26a7369ee2256e41), LU(0x186de3f44b532d15), LU(0x62500624544ebcda),
        LU(0xe478a1a226320ab4), LU(0xee22d006d61f2c16), LU(0x185482bc8950e61e),
        LU(0x0b23c80f8d5bfe03), LU(0x0af714afffb33e0c), LU(0xf1fe2bc6442bc212),
        LU(0xa9b7b7222783f83b), LU(0x77062658d2d7157b), LU(0x397a77f42d70dda5),
        LU(0xc8b53a80c049a117), LU(0x67e291b153b5622a), LU(0x5fd35eff20394f02),
        LU(0xcb0fdecf949d2a0b), LU(0x7962ad34a07a6f3c), LU(0x1f0474c3700f422d),
        LU(0x4d06eb84ee50437d), LU(0xc7029e939a097c6c), LU(0xf2342f5c595fbcd9),
        LU(0xeac83e27f17ba5ef), LU(0x5e341371b0137e98), LU(0x11a3222701322d44),
        LU(0x3e49ff6df85fec79), LU(0x1f4b7c3e2853d7f1), LU(0x4059a5fa31961b26),
        LU(0x751cdb491e6987d4), LU(0x7d2b2fa3721c1c06), LU(0xd1dc12918a3a2f86),
    };

    mt19937 mt;
    init(&mt, SEED64);

    for (int i = 0; i < 30; i += 1)
    {
        u64 got = mt19937_next(&mt);
        u64 expected = expect64[i];

        assert_equal(got, expected);
    }
}

define_test(next_random_int_gets_next_int)
{
    mt19937 mt;
    init(&mt, SEED64);

    assert_equal(next_random_int(&mt), LU(0x26a7369ee2256e41));
    assert_equal(next_random_int(&mt), LU(0x186de3f44b532d15));
    assert_equal(next_random_int(&mt), LU(0x62500624544ebcda));
}

template<typename T>
T abs(T x)
{
    return x < ((T)0) ? -x : x;
}

define_test(next_random_decimal_gets_next_decimal)
{
    mt19937 mt;
    init(&mt, SEED64);

#define assert_almost_equal(X, Y) assert_less(abs(abs(X) - abs(Y)), 0.0000001)

    assert_almost_equal(next_random_decimal(&mt), 0.150988973417);
    assert_almost_equal(next_random_decimal(&mt), 0.0954267951);
    assert_almost_equal(next_random_decimal(&mt), 0.3840335692);
}

define_test(seed_rng_seeds_thread_local_rng)
{
    seed_rng(LU(0x853c49e6748fea9b));

    assert_equal(next_random_int(), LU(13499640503520517996));
    assert_equal(next_random_int(), LU(6775828892400545392));
    assert_equal(next_random_int(), LU(4981977414620091798));
}

define_test(next_bounded_int_gets_int_within_bound)
{
    seed_rng(LU(0x853c49e6748fea9b));

    u64 val;
    u64 bound = 0;

#define test_bound(Bound)\
    bound = Bound;\
\
    for (int i = 0; i < BOUND_TEST_COUNT; ++i)\
    {\
        val = next_bounded_int(bound);\
\
        if (val >= bound)\
        {\
            assert_less(val, bound);\
            break;\
        }\
    }

    test_bound(1);
    test_bound(100);
    test_bound(256);
    test_bound(4096);
    test_bound(1ul << 32ul);
}

define_test(next_bounded_int_gets_int_within_bound2)
{
    seed_rng(LU(0x853c49e6748fea9b));

    u64 val;
    u64 lower_bound = 0;
    u64 upper_bound = 0;

#define test_upper_lower_bound(Lower, Upper)\
    lower_bound = Lower;\
    upper_bound = Upper;\
\
    for (int i = 0; i < BOUND_TEST_COUNT; ++i)\
    {\
        val = next_bounded_int(lower_bound, upper_bound);\
\
        if (val < lower_bound)\
        {\
            assert_greater_or_equal(val, lower_bound);\
            break;\
        }\
        else if (val > upper_bound)\
        {\
            assert_less_or_equal(val, upper_bound);\
            break;\
        }\
    }

    test_upper_lower_bound(0, 1);
    test_upper_lower_bound(0, 100);
    test_upper_lower_bound(0, 256);
    test_upper_lower_bound(0, 4096);

    test_upper_lower_bound(1, 1);
    test_upper_lower_bound(1, 100);
    test_upper_lower_bound(1, 256);
    test_upper_lower_bound(1, 4096);

    test_upper_lower_bound(100, 100);
    test_upper_lower_bound(100, 256);
    test_upper_lower_bound(100, 4096);
}

define_default_test_main()
