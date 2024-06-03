
// mt19937 credit https://github.com/skeeto/scratch/blob/master/mt19937/mt19937_64.h

#include "shl/assert.hpp"
#include "shl/platform.hpp"
#include "shl/time.hpp"
#include "shl/random.hpp"

#if Linux
#include "shl/file_stream.hpp" // for reading /dev/urandom
#endif

#include <math.h> // sqrt, log

#define max_u64_double 18446744073709551615.0

u64 generate_seed()
{
    // We probably could cache these resources so they're not loaded
    // each time, or even make a "hardware_rng" generator, but
    // most of the time you only need a hardware generator to seed
    // pseudo RNGs instead, so for now the API can stay like this.

    u64 ret = 0;

#if Windows
    // TODO: use RtlGenRandom or something
#else
    file_stream randstream;

    if (init(&randstream, "/dev/urandom", OPEN_MODE_READ))
    {
        bool ok = read(&randstream, &ret, sizeof(u64));
        free(&randstream);

        if (ok)
            return ret;
        // else: use time below
    }
#endif

    // fallback: if platform hardware rng didn't work, maybe time will.
    timespan t;
    get_time(&t);
    
    ret = (u64)(t.seconds ^ t.nanoseconds);

    return ret;
}

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

#define define_next_bounded_int(TGen)    \
u64 next_bounded_int(TGen *gen, u64 max) \
{                                        \
    assert(max > 0);                     \
                                         \
    u64 nmax = (-max) % max;             \
                                         \
    while (true)                         \
    {                                    \
        u64 ret = next_random_int(gen);  \
                                         \
        if (ret >= nmax)                 \
            return ret % max;            \
    }                                    \
}

define_next_bounded_int(mt19937);
define_next_bounded_int(pcg64);

u64 next_bounded_int(u64 min, u64 max)
{
    return next_bounded_int(_get_rng_pointer(), min, max);
}

#define define_next_bounded_int2(TGen)                      \
u64 next_bounded_int(TGen *gen, u64 min, u64 max)           \
{                                                           \
    assert(min <= max);                                     \
                                                            \
    if (min == max)                                         \
        return min;                                         \
                                                            \
    return next_bounded_int(gen, (max - min) + 1) + min;    \
}

define_next_bounded_int2(mt19937);
define_next_bounded_int2(pcg64);

// range [0, 1]
double next_random_decimal()
{
    return next_random_decimal(_get_rng_pointer());
}

#define define_next_random_decimal(TGen)          \
double next_random_decimal(TGen *gen)             \
{                                                 \
    return ((double)next_random_int(gen)) / max_u64_double; \
}

define_next_random_decimal(mt19937);
define_next_random_decimal(pcg64);

double next_bounded_decimal(double max)
{
    return next_bounded_decimal(_get_rng_pointer(), max);
}

#define define_next_bounded_decimal(TGen)           \
double next_bounded_decimal(TGen *gen, double max)  \
{                                                   \
    return next_random_decimal(gen) * max;          \
}

define_next_bounded_decimal(mt19937);
define_next_bounded_decimal(pcg64);

double next_bounded_decimal(double min, double max)
{
    return next_bounded_decimal(_get_rng_pointer(), min, max);
}

#define define_next_bounded_decimal2(TGen)                      \
double next_bounded_decimal(TGen *gen, double min, double max)  \
{                                                               \
    assert(min <= max);                                         \
    return next_bounded_decimal(gen, max - min) + min;          \
}

define_next_bounded_decimal2(mt19937);
define_next_bounded_decimal2(pcg64);

// DISTRIBUTIONS
void init(discrete_distribution *dist, double *weights, u64 weight_count)
{
    assert(dist != nullptr);

    dist->weights = weights;
    dist->weight_count = weight_count;
    dist->weight_sum = 0;

    for (u64 i = 0; i < weight_count; ++i)
        dist->weight_sum += weights[i];
}

u64 distribute(discrete_distribution dist)
{
    return distribute(_get_rng_pointer(), dist);
}

// rand_float is within [0, dist.weight_sum]
static inline u64 _discrete_distribute_random_float(double rand_float, discrete_distribution dist)
{
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
        if (rand_float <= acc)
            return i;
    }

    return dist.weight_count - 1;
}

#define define_discrete_distribute(TGen)                                        \
u64 distribute(TGen *gen, discrete_distribution dist)                           \
{                                                                               \
    double rand_within_sum = next_bounded_decimal(gen, dist.weight_sum);        \
                                                                                \
    return _discrete_distribute_random_float(rand_within_sum, dist); \
}

define_discrete_distribute(mt19937);
define_discrete_distribute(pcg64);

void init(normal_distribution *dist, double mean, double deviation)
{
    dist->mean = mean;
    dist->deviation = deviation;
}

// uses thread local rng
double distribute(normal_distribution dist)
{
    return distribute(_get_rng_pointer(), dist);
}

#define define_normal_distribute(TGen)                  \
double distribute(TGen *gen, normal_distribution dist)  \
{                                                       \
    double u = 0.0;                                     \
    double v = 0.0;                                     \
    double s = 2.0;                                     \
                                                        \
    while (s >= 1 || s == 0.0)                          \
    {                                                   \
        u = next_bounded_decimal(gen, -1.0, 1.0);       \
        v = next_bounded_decimal(gen, -1.0, 1.0);       \
        s = u*u + v*v;                                  \
    }                                                   \
                                                        \
    v = u * sqrt((-2 * log(s)) / s);                    \
                                                        \
    return dist.mean + dist.deviation * v;              \
}

define_normal_distribute(mt19937);
define_normal_distribute(pcg64);
