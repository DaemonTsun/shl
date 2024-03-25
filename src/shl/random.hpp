
#pragma once

/* random.hpp

Random number generation.

TODO: docs

*/

#include "shl/number_types.hpp"

struct mt19937 {
    u64 v[312];
    s32 i;
};

void init(mt19937 *gen, u64 seed);
u64 mt19937_next(mt19937 *gen);

// seeds the thread local rng
void seed_rng(u64 seed);

// range [0, max_value(u64)]
// uses the thread local rng
u64 next_random_int();
u64 next_random_int(mt19937 *gen);

// range [0, 1]
double next_random_decimal();
double next_random_decimal(mt19937 *gen);
