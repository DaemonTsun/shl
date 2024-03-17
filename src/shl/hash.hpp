
#pragma once

/* hash.hpp

Defines the hash_t type (32 bit unsigned integer) and functions to calculate hashes.

Example:

    int myint = 10;
    hash_t h1 = hash(&myint); // for some builtin types, see below

    float myvec[3] = {0.3f, 0.7f, 1.f};
    hash_t h2 = hash_data(myvec, sizeof(float) * 3); // for any pointer

The hash_data(data, size) function calculates the hash of arbitrary data.
hash_raw(*T) is the same as hash_data, but using a type T and its size instead of
a pointer and size.

There are default overloads of the hash(*x) for many types in shl,
including number types, strings, array and others.

Default hashing algorithm used in hash_data is murmur3.
Implementation of murmur3 is in murmur_hash.hpp.
 */

#include "shl/number_types.hpp"

typedef u32 hash_t;

hash_t hash_data(const void *data, u64 size);
hash_t hash_data(const void *data, u64 size, u32 seed);

template<typename T>
hash_t hash_raw(const T *t)
{
    return hash_data(t, sizeof(T));
}

template<typename T>
using hash_function = hash_t (*)(const T*);

hash_t hash(const char *string); // string, NOT char. please do not use.
hash_t hash(const wchar_t *string); // string, NOT wchar_t. please do not use.
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

hash_t operator""_h(const char    *str, u64 size);
hash_t operator""_h(const wchar_t *str, u64 size);
