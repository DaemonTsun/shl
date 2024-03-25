
// credit https://github.com/skeeto/scratch/blob/master/mt19937/mt19937_64.h

#include "shl/assert.hpp"
#include "shl/random.hpp"

void init(mt19937 *gen, u64 seed)
{
    assert(gen != nullptr);
    gen->i = 312;
    gen->v[0] = seed;

    for (s32 i = 1; i < 312; i++)
        gen->v[i] = LONG_UINT_LIT(0x5851f42d4c957f2d) * (gen->v[i-1] ^ (gen->v[i-1] >> 62)) + i;
}

u64 mt19937_next(mt19937 *gen)
{
    assert(gen != nullptr);

    if (gen->i >= 312)
    {
        for (int i = 0; i < 312; i++)
        {
            u64 x = (gen->v[ i       ] & LONG_UINT_LIT(0xffffffff80000000)) +
                    (gen->v[(i+1)%312] & LONG_UINT_LIT(0x000000007fffffff));

            u64 a = (x >> 1) ^ ((x & 1) * LONG_UINT_LIT(0xb5026f5aa96619e9));

            gen->v[i] = gen->v[(i+156)%312] ^ a;
        }

        gen->i = 0;
    }

    u64 y = gen->v[gen->i++];
    y = y ^ (y >> 29 & LONG_UINT_LIT(0x5555555555555555));
    y = y ^ (y << 17 & LONG_UINT_LIT(0x71d67fffeda60000));
    y = y ^ (y << 37 & LONG_UINT_LIT(0xfff7eee000000000));
    y = y ^ (y >> 43);

    return y;
}

static mt19937 *_get_rng_pointer()
{
    // TODO: this should really be replaced with an rng inside an extended
    // program_context of sorts, but it's fine for now.
    thread_local mt19937 _rng;

    return &_rng;
}

void seed_rng(u64 seed)
{
    init(_get_rng_pointer(), seed);
}

u64 next_random_int()
{
    return next_random_int(_get_rng_pointer());
}

u64 next_random_int(mt19937 *gen)
{
    return mt19937_next(gen);
}

// range [0, 1]
double next_random_decimal()
{
    return next_random_decimal(_get_rng_pointer());
}

double next_random_decimal(mt19937 *gen)
{
    u64 val = next_random_int(gen);
    return val / 18446744073709551615.0;
}
