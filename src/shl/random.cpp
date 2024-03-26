
// credit https://github.com/skeeto/scratch/blob/master/mt19937/mt19937_64.h

#include "shl/random.hpp"

void init(mt19937 *gen, u64 seed)
{
    assert(gen != nullptr);

    gen->i = 312;
    gen->v[0] = seed;

    for (s32 i = 1; i < 312; i++)
        gen->v[i] = U64_LIT(0x5851f42d4c957f2d) * (gen->v[i-1] ^ (gen->v[i-1] >> 62)) + i;
}

u64 mt19937_next(mt19937 *gen)
{
    assert(gen != nullptr);

    if (gen->i >= 312)
    {
        for (int i = 0; i < 312; i++)
        {
            u64 x = (gen->v[ i       ] & U64_LIT(0xffffffff80000000)) +
                    (gen->v[(i+1)%312] & U64_LIT(0x000000007fffffff));

            u64 a = (x >> 1) ^ ((x & 1) * U64_LIT(0xb5026f5aa96619e9));

            gen->v[i] = gen->v[(i+156)%312] ^ a;
        }

        gen->i = 0;
    }

    u64 y = gen->v[gen->i++];
    y = y ^ (y >> 29 & U64_LIT(0x5555555555555555));
    y = y ^ (y << 17 & U64_LIT(0x71d67fffeda60000));
    y = y ^ (y << 37 & U64_LIT(0xfff7eee000000000));
    y = y ^ (y >> 43);

    return y;
}

void init(pcg64 *gen, u64 seed, u64 inc)
{
    assert(gen != nullptr);

    gen->state = seed;
    gen->inc = inc | 1u;
}

u64 pcg64_next(pcg64 *gen)
{
    u64 oldstate = gen->state;
    gen->state = gen->state * U64_LIT(6364136223846793005) + gen->inc;

    u64 word = ((oldstate >> ((oldstate >> 59u) + 5u)) ^ oldstate) * U64_LIT(12605985483714917081);
    
    return (word >> 43u) ^ word;
}

static pcg64 *_get_rng_pointer()
{
    // TODO: this should really be replaced with an rng inside an extended
    // program_context of sorts, but it's fine for now.
    thread_local pcg64 _rng;

    return &_rng;
}

void seed_rng(u64 seed)
{
    seed_rng(seed, U64_LIT(0xda3e39cb94b95bdb));
}

void seed_rng(u64 seed, [[maybe_unused]] u64 extra)
{
    init(_get_rng_pointer(), seed, extra);
}

u64 next_random_int()
{
    return next_random_int(_get_rng_pointer());
}

u64 next_random_int(mt19937 *gen)
{
    return mt19937_next(gen);
}

u64 next_random_int(pcg64 *gen)
{
    return pcg64_next(gen);
}

u64 next_bounded_int(u64 max)
{
    return next_bounded_int(_get_rng_pointer(), max);
}

u64 next_bounded_int(u64 min, u64 max)
{
    return next_bounded_int(_get_rng_pointer(), min, max);
}

// range [0, 1]
double next_random_decimal()
{
    return next_random_decimal(_get_rng_pointer());
}

double next_bounded_decimal(double max)
{
    return next_bounded_decimal(_get_rng_pointer(), max);
}

double next_bounded_decimal(double min, double max)
{
    return next_bounded_decimal(_get_rng_pointer(), min, max);
}

