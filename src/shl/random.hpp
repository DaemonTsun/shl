
#pragma once

/* random.hpp

Random number generation.

TODO: docs

*/

#include "shl/number_types.hpp"

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
u64 next_bounded_int(mt19937 *gen, u64 max);
u64 next_bounded_int(pcg64 *gen, u64 max);

// range [min, max]
u64 next_bounded_int(u64 min, u64 max);
u64 next_bounded_int(mt19937 *gen, u64 min, u64 max);
u64 next_bounded_int(pcg64 *gen, u64 min, u64 max);

// range [0, 1]
double next_random_decimal();
double next_random_decimal(mt19937 *gen);
double next_random_decimal(pcg64 *gen);

// range [0, max]
double next_bounded_decimal(double max);
double next_bounded_decimal(mt19937 *gen, double max);
double next_bounded_decimal(pcg64 *gen, double max);

// range [min, max]
double next_bounded_decimal(double min, double max);
double next_bounded_decimal(mt19937 *gen, double min, double max);
double next_bounded_decimal(pcg64 *gen, double min, double max);

// DISTRIBUTIONS
struct discrete_distribution
{
    double *weights;
    u64 weight_count;
    double weight_sum;
};

void init(discrete_distribution *dist, double *weights, u64 weight_count);

// uses thread local rng
u64 distribute(discrete_distribution dist);
u64 distribute(mt19937 *gen, discrete_distribution dist);
u64 distribute(pcg64 *gen, discrete_distribution dist);

