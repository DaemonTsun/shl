
#include "shl/random.hpp"
#include "t1/t1.hpp"

#define LU(X) U64_LIT(X)

#define SEED64 LU(0x3243f6a8885a308d)

#define BOUND_TEST_COUNT 100000

// lol these work for these tests its fine
#define MAX_DOUBLE 18446744073709551615.0
#define MIN_DOUBLE -18446744073709551615.0

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
    u64 max_value_found = 0;

#define test_int_bound(Bound)\
    bound = Bound;\
    max_value_found = 0;\
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
        \
        if (val > max_value_found) max_value_found = val;\
    }\
    \
    printf("In range [0, %llu[, max value found: %llu\n", Bound, max_value_found);

    test_int_bound(1);
    test_int_bound(100);
    test_int_bound(256);
    test_int_bound(4096);
    test_int_bound(1ul << 32ul);
}

define_test(next_bounded_int_gets_int_within_bound2)
{
    seed_rng(LU(0x853c49e6748fea9b));

    u64 val;
    u64 lower_bound = 0;
    u64 upper_bound = 0;

    u64 min_value_found = max_value(u64);
    u64 max_value_found = 0;

#define test_int_upper_lower_bound(Lower, Upper)\
    lower_bound = Lower;\
    upper_bound = Upper;\
    min_value_found = max_value(u64);\
    max_value_found = 0;\
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
        \
        if (val < min_value_found) min_value_found = val;\
        if (val > max_value_found) max_value_found = val;\
    }\
    \
    printf("In range [%llu, %llu], min & max values found: %llu, %llu\n", Lower, Upper, min_value_found, max_value_found);

    test_int_upper_lower_bound(0, 1);
    test_int_upper_lower_bound(0, 100);
    test_int_upper_lower_bound(0, 256);
    test_int_upper_lower_bound(0, 4096);

    test_int_upper_lower_bound(1, 1);
    test_int_upper_lower_bound(1, 100);
    test_int_upper_lower_bound(1, 256);
    test_int_upper_lower_bound(1, 4096);

    test_int_upper_lower_bound(100, 100);
    test_int_upper_lower_bound(100, 256);
    test_int_upper_lower_bound(100, 4096);
}

define_test(next_bounded_decimal_gets_decimal_within_bound)
{
    seed_rng(LU(0x853c49e6748fea9b));

    double val;
    double bound = 0;
    double min_value_found = MAX_DOUBLE;
    double max_value_found = MIN_DOUBLE;

#define test_decimal_bound(Bound)\
    bound = Bound;\
    min_value_found = MAX_DOUBLE;\
    max_value_found = MIN_DOUBLE;\
\
    for (int i = 0; i < BOUND_TEST_COUNT; ++i)\
    {\
        val = next_bounded_decimal(bound);\
\
        if (bound >= 0)\
        {\
            if (val > bound)\
            {\
                assert_less_or_equal(val, bound);\
                break;\
            }\
        }\
        else\
        {\
            if (val < bound)\
            {\
                assert_greater_or_equal(val, bound);\
                break;\
            }\
        }\
        \
        if (val < min_value_found) min_value_found = val;\
        if (val > max_value_found) max_value_found = val;\
    }\
    \
    if (Bound >= 0)\
        printf("In range [0, %f], min & max value found: %f, %f\n", Bound, min_value_found, max_value_found);\
    else\
        printf("In range [%f, 0], min & max value found: %f, %f\n", Bound, min_value_found, max_value_found);

    test_decimal_bound(1.0);
    test_decimal_bound(100.0);
    test_decimal_bound(256.0);
    test_decimal_bound(4096.0);
    test_decimal_bound((double)(1ul << 32ul));

    test_decimal_bound(-1.0);
}

define_test(next_bounded_decimal_gets_decimal_within_bound2)
{
    seed_rng(LU(0x853c49e6748fea9b));

    double val;
    double lower = 0;
    double upper = 0;
    double min_value_found = MAX_DOUBLE;
    double max_value_found = MIN_DOUBLE;

#define test_decimal_upper_lower_bound(Lower, Upper)\
    lower = Lower;\
    upper = Upper;\
    min_value_found = MAX_DOUBLE;\
    max_value_found = MIN_DOUBLE;\
\
    for (int i = 0; i < BOUND_TEST_COUNT; ++i)\
    {\
        val = next_bounded_decimal(lower, upper);\
\
        if (val < Lower)\
        {\
            assert_greater_or_equal(val, Lower);\
            break;\
        }\
        \
        if (val > Upper)\
        {\
            assert_less_or_equal(val, Upper);\
            break;\
        }\
        \
        if (val < min_value_found) min_value_found = val;\
        if (val > max_value_found) max_value_found = val;\
    }\
    \
    printf("In range [%f, %f], min & max value found: %f, %f\n", Lower, Upper, min_value_found, max_value_found);

    test_decimal_upper_lower_bound(0.0, 1.0);
    test_decimal_upper_lower_bound(0.0, 100.0);
    test_decimal_upper_lower_bound(0.0, 256.0);
    test_decimal_upper_lower_bound(0.0, 4096.0);

    test_decimal_upper_lower_bound(1.0, 1.0);
    test_decimal_upper_lower_bound(1.0, 100.0);
    test_decimal_upper_lower_bound(1.0, 256.0);
    test_decimal_upper_lower_bound(1.0, 4096.0);

    test_decimal_upper_lower_bound(100.0, 100.0);
    test_decimal_upper_lower_bound(100.0, 256.0);
    test_decimal_upper_lower_bound(100.0, 4096.0);

    test_decimal_upper_lower_bound(-100.0, 0.0);
    test_decimal_upper_lower_bound(-100.0, -50.0);
    test_decimal_upper_lower_bound(-100.0, 50.0);
}

define_default_test_main()
