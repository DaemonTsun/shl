
#pragma once

/* random.hpp

Random number generation.

Defines random number generators and some distributions.

Example:

    pcg64 gen;
    init(&gen, generate_seed(), generate_seed());

    u64 rand_int = next_random_int(&gen);
    u64 rand_int2 = next_bounded_int(&gen, 10, 200);

    double rand_decimal = next_random_decimal(&gen);
    double rand_decimal2 = next_bounded_decimal(&gen, -20, 20);

    double weights[4] = {0.25, 0.25, 0.05, 0.45}
    discrete_distribution dist;
    init(&dist, weigts, 4);

    u64 distributed_int = distribute(&gen, dist);


Generators:
    
    mt19937 - mersenne twister, probably don't use anymore.
    pcg64   - 64 bit PCG generator, preferred.


Distributions:

    discrete_distribution   - weighted integer generation
    normal_distribution     - normal (Gaussian) distribution


Functions:

generate_seed()
    returns a 64 bit unsigned integer using the platforms preferred way
    of generating random numbers, e.g. /dev/urandom on Linux.
    If the platform RNG failed, the time is used instead.
    This is considered a "heavy" function and shouldn't be the primary
    way of generating random numbers, but it may be used to generate seeds
    for pRNGs.

seed_rng(seed[, extra])
    Seeds the thread local pRNG with the given values.

next_random_int()
    Uses the thread local pRNG to generate a 64 bit unsigned integer.

next_random_int(*Gen)
    Uses the given generator Gen to generate a 64 bit unsigned integer.

next_bounded_int(Max) 
    Uses the thread local pRNG to generate a 64 bit unsigned integer smaller than Max.

next_bounded_int(*Gen, Max) 
    Uses the given generator Gen to generate a 64 bit unsigned integer smaller than Max.

next_bounded_int(Min, Max) 
    Uses the thread local pRNG to generate a 64 bit unsigned integer greater or equal
    to min and smaller than Max.

next_bounded_int(*Gen, Min, Max) 
    Uses the given generator Gen to generate a 64 bit unsigned integer greater or equal
    to min and smaller than Max.


next_random_decimal()
    Uses the thread local pRNG to generate a double.

next_random_decimal(*Gen)
    Uses the given generator Gen to generate a double.

next_bounded_decimal(Max) 
    Uses the thread local pRNG to generate a double smaller than Max.

next_bounded_decimal(*Gen, Max) 
    Uses the given generator Gen to generate a double smaller than Max.

next_bounded_decimal(Min, Max) 
    Uses the thread local pRNG to generate a double greater or equal
    to min and smaller or equal to Max.

next_bounded_decimal(*Gen, Min, Max) 
    Uses the given generator Gen to generate a double greater or equal
    to min and smaller or equal to Max.

Distribution functions:

init(discrete_distribution *dist, Weights, WeightCount)
    Initializes the discrete distribution dist with Weights (a pointer to doubles)
    and WeightCount.

init(normal_distribution *dist, Mean, Deviation)
    Initializes the normal distribution dist with Mean and Deviation.

distribute(discrete_distribution dist)
    Uses the thread local pRNG to generate a random 64 bit unsigned integer
    based on the weights of the discrete distribution dist,
    e.g. if Weights = {0.5, 0.25, 0.25}, there's a:
    - 50% chance to return 0
    - 25% chance to return 1
    - 25% chance to return 2

distribute(*Gen, discrete_distribution dist)
    Uses the generator Gen to generate a random 64 bit unsigned integer
    based on the weights of dist. See above.

distribute(normal_distribution dist)
    Uses the thread local pRNG to generate a random double based on
    the mean and deviation of dist.

distribute(*Gen, normal_distribution dist)
    Uses the generator Gen to generate a random double based on
    the mean and deviation of dist.
*/

#include "shl/number_types.hpp"

u64 generate_seed();

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

struct normal_distribution
{
	double mean;
	double deviation;
};

void init(normal_distribution *dist, double mean, double deviation);

// uses thread local rng
double distribute(normal_distribution dist);
double distribute(mt19937 *gen, normal_distribution dist);
double distribute(pcg64 *gen, normal_distribution dist);
