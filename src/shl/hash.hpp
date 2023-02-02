
#pragma once

/* hash.hpp
 *
 * defines the hash_t type (64 bit unsigned integer) and functions to calculate hashes.
 *
 * the hash_data(data, size) function maybe used to calculate the hash of arbitrary data.
 * hash_raw(T) is the same as hash_data, but using a type T and its size instead of
 * a pointer and size.
 */

#include "shl/number_types.hpp"

typedef u64 hash_t;

// TODO: move to cpp and optimize with intrinsics
constexpr inline hash_t rotl(hash_t hsh, u64 places)
{
    u64 mask = (8 * sizeof(hsh)) - 1;

    places &= mask;
    return (hsh << places) | (hsh >> ((-places) & mask));
}

constexpr inline hash_t rotr(hash_t hsh, u64 places)
{
    u64 mask = (8 * sizeof(hsh)) - 1;

    places &= mask;
    return (hsh >> places) | (hsh << ((-places) & mask));
}

hash_t hash_data(const char *data, u64 size);

template<typename T>
hash_t hash_raw(const T *t)
{
    return hash_data(reinterpret_cast<const char *>(t), sizeof(T));
}
