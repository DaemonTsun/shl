
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
 *
 * NULL_HASH is a constant value that will never get generated.
 */

#include "shl/number_types.hpp"

typedef u64 hash_t;

// a hash that will never be generated
#define NULL_HASH -1

#define DEFAULT_MURMUR2_SEED 0xc70f6907ul

hash_t hash_data(const void *data, u64 size);
hash_t hash_data(const void *data, u64 size, u64 seed);

template<typename T>
hash_t hash_raw(const T *t)
{
    return hash_data(t, sizeof(T));
}

template<typename T>
using hash_function = hash_t (*)(const T*);

hash_t hash(const char *string); // string, NOT char
hash_t hash(const wchar_t *string); // string, NOT wchar_t
hash_t hash(const bool *v);
hash_t hash(const u8  *v);
hash_t hash(const u16 *v);
hash_t hash(const u32 *v);
hash_t hash(const u64 *v);
hash_t hash(const s8  *v);
hash_t hash(const s16 *v);
hash_t hash(const s32 *v);
hash_t hash(const s64 *v);
hash_t hash(const float *v);
hash_t hash(const double *v);
hash_t hash(const long double *v);
hash_t hash(const void **v);
