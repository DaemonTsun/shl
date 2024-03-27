
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
auto next_bounded_int(TGen *gen, u64 max) -> decltype(next_random_int(gen))
{
    assert(max > 0);

    u64 nmax = (-max) % max;

    while (true)
    {
        u64 ret = next_random_int(gen);

        if (ret >= nmax)
            return ret % max;
    }
}

// range [min, max]
u64 next_bounded_int(u64 min, u64 max);

template<typename TGen>
auto next_bounded_int(TGen *gen, u64 min, u64 max) -> decltype(next_random_int(gen))
{
    assert(min <= max);

    if (min == max)
        return min;

    return next_bounded_int((max - min) + 1) + min;
}

// range [0, 1]
double next_random_decimal();

template<typename TGen>
auto next_random_decimal(TGen *gen) -> decltype(next_random_int(gen) / max_u64_double) 
{
    return next_random_int(gen) / max_u64_double;
}

// range [0, max]
double next_bounded_decimal(double max);

template<typename TGen>
auto next_bounded_decimal(TGen *gen, double max) -> decltype(next_random_decimal(gen))
{
    return next_random_decimal(gen) * max;
}

// range [min, max]
double next_bounded_decimal(double min, double max);

template<typename TGen>
auto next_bounded_decimal(TGen *gen, double min, double max) -> decltype(next_random_decimal(gen))
{
    assert(min <= max);
    return next_bounded_decimal(gen, max - min) + min;
}

// DISTRIBUTIONS
struct discrete_distribution
{
    double *weights;
    u64 weight_count;
    double weight_sum;
};

discrete_distribution get_discrete_distribution(double *weights, u64 weight_count);

// uses thread local rng
u64 distribute(discrete_distribution dist);

template<typename TGen>
auto distribute(TGen *gen, discrete_distribution dist) -> decltype(next_bounded_decimal(gen, 0.0), U64_LIT(0))
{
    double x = next_bounded_decimal(gen, dist.weight_sum);
    double acc = 0.0;
    u64 i;

    // This could be optimized with a binary search of the calculated subtotals,
    // but like this implementation is fine until you run into very large weight
    // counts, at which point you'd have to wonder if discrete distribution is
    // even worth it.
    // Also this implementation allocates 0 (zero) memory and doesn't normalize
    // any values.
    for (i = 0; i < dist.weight_count; ++i)
    {
        acc += dist.weights[i];
        if (x <= acc)
            return i;
    }

    return dist.weight_count - 1;
}
