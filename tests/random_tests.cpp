
#include "shl/random.hpp"
#include "t1/t1.hpp"

#define LU(X) LONG_UINT_LIT(X)

#define SEED64 LU(0x3243f6a8885a308d)

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

define_test(seed_rng_seeds_thread_local_rng)
{
    seed_rng(SEED64);

    assert_equal(next_random_int(), LU(0x26a7369ee2256e41));
    assert_equal(next_random_int(), LU(0x186de3f44b532d15));
    assert_equal(next_random_int(), LU(0x62500624544ebcda));
}

define_test(next_random_int_gets_next_int_from_thread_local_rng)
{
    seed_rng(SEED64);

    assert_equal(next_random_int(), LU(0x26a7369ee2256e41));
    assert_equal(next_random_int(), LU(0x186de3f44b532d15));
    assert_equal(next_random_int(), LU(0x62500624544ebcda));
}

template<typename T>
T abs(T x)
{
    return x < ((T)0) ? -x : x;
}

#include "shl/file_stream.hpp"

define_test(next_random_decimal_gets_next_decimal_from_thread_local_rng)
{
    seed_rng(SEED64);

#define assert_almost_equal(X, Y) assert_less(abs(abs(X) - abs(Y)), 0.0000001)

    assert_almost_equal(next_random_decimal(), 0.150988973417);
    assert_almost_equal(next_random_decimal(), 0.0954267951);
    assert_almost_equal(next_random_decimal(), 0.3840335692);
}

define_default_test_main()
