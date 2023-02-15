
#pragma once

/* hash.hpp
 *
 * defines the hash_t type (64 bit unsigned integer) and functions to calculate hashes.
 *
 * the hash_data(data, size) function maybe used to calculate the hash of arbitrary data.
 * hash_raw(T) is the same as hash_data, but using a type T and its size instead of
 * a pointer and size.
 *
 * default hashing algorithm used in hash_data is murmur2.
 */

#include "shl/number_types.hpp"

typedef u64 hash_t;

#define DEFAULT_MURMUR2_SEED 0xc70f6907ul

hash_t hash_data(const void *data, u64 size);
hash_t hash_data(const void *data, u64 size, u64 seed);

template<typename T>
hash_t hash_raw(const T *t)
{
    return hash_data(t, sizeof(T));
}
