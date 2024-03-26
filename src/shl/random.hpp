
#pragma once

/* random.hpp

Random number generation.

TODO: docs

*/

#include "shl/number_types.hpp"
#include "shl/assert.hpp"

#define max_u64_double 18446744073709551615.0

struct mt19937
{
    u64 v[312];
    s32 i;
};

void init(mt19937 *gen, u64 seed);
u64 mt19937_next(mt19937 *gen);

struct pcg64
{
    u64 state;
    u64 inc;
};

void init(pcg64 *gen, u64 seed, u64 inc);
u64 pcg64_next(pcg64 *gen);

// seeds the thread local rng
void seed_rng(u64 seed);
void seed_rng(u64 seed, u64 extra);

// range [0, max_value(u64)]
// uses the thread local rng
u64 next_random_int();
u64 next_random_int(mt19937 *gen);
u64 next_random_int(pcg64 *gen);

// range [0, bound[
u64 next_bounded_int(u64 max);

template<typename TGen>
auto next_bounded_int(TGen *gen, u64 bound)
    -> decltype(next_random_int(gen))
{
    assert(bound > 0);

    u64 nbound = (-bound) % bound;

    while (true)
    {
        u64 ret = next_random_int(gen);

        if (ret >= nbound)
            return ret % bound;
    }
}

// range [min, max]
u64 next_bounded_int(u64 min, u64 max);

template<typename TGen>
auto next_bounded_int(TGen *gen, u64 min, u64 max)
    -> decltype(next_random_int(gen))
{
    assert(min <= max);

    if (min == max)
        return min;

    return next_bounded_int(((max + 1) - min)) + min;
}

// range [0, 1]
double next_random_decimal();

template<typename TGen>
auto next_random_decimal(TGen *gen)
    -> decltype(next_random_int(gen) / max_u64_double) 
{
    return next_random_int(gen) / max_u64_double;
}

