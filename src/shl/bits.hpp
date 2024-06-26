
#pragma once

/* bits.hpp

Collection of mostly mathematical functions for bit manipulation,
sometimes optimized for powers of two.
See tests/bits_tests.cpp for examples of every function.

Functions:

const_log2(x)   log2 of x
bit_log<N>(x)   number of times x can be shifted right N times

floor_exp2(x)   rounds down to nearest power of 2
floor_exp<N>(x) rounds down to nearest power of N
floor_exp(x, N) rounds down to nearest power of N

ceil_exp2(x)    rounds up to nearest power of 2
ceil_exp<N>(x)  rounds up to nearest power of N
ceil_exp(x, N)  rounds up to nearest power of N

rotl(x, N)      rotates x N times to the left
rotr(x, N)      rotates x N times to the right

bitmask(From, To)   bitmask from From to To, e.g. bitmask(1, 4) = 00011110
bitmask_between_values(From, To) bitmask between binary values

bitrange(X, From, To) gets the bits of X in the range From to To

popcnt(x) number of bits set to 1 in a 32 bit unsigned integer
clz(x)    counts the number of leading zeroes in a 32 bit unsigned integer
ctz(x)    counts the number of trailing zeroes in a 32 bit unsigned integer

 */

#include "shl/number_types.hpp"

template<typename T>
constexpr inline T const_log2(T x)
{
    T ret = 0;

    while (x >>= 1)
        ret++;

    return ret;
}

template<unsigned N, typename T>
constexpr inline T bit_log(T x)
{
    if constexpr (N == 1)
        return const_log2(x);
    else
    {
        T ret = 0;

        while (x >>= N)
            ret++;

        return ret;
    }
}

template<typename T>
constexpr inline T floor_exp2(T x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    if constexpr (sizeof(T) >= 2) x |= x >> 8;
    if constexpr (sizeof(T) >= 4) x |= x >> 16;
    if constexpr (sizeof(T) >= 8) x |= x >> 32;
    return x - (x >> 1);
}

template<u32 N, typename T>
constexpr inline T floor_exp(T x)
{
    if constexpr (N == 2)
        return floor_exp2(x);

    constexpr T Shift = bit_log<1>(N);

    T ret = 1;

    while (x >>= Shift)
        ret <<= Shift;

    return ret;
}

template<typename T>
constexpr inline T floor_exp(T x, u32 n)
{
    if (n == 2)
        return floor_exp2(x);

    T Shift = bit_log<1>(n);
    T ret = 1;

    while (x >>= Shift)
        ret <<= Shift;

    return ret;
}

template<typename T>
constexpr inline T ceil_exp2(T x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    if constexpr (sizeof(T) >= 2) x |= x >> 8;
    if constexpr (sizeof(T) >= 4) x |= x >> 16;
    if constexpr (sizeof(T) >= 8) x |= x >> 32;
    x++;

    return x;
}

template<u32 N, typename T>
constexpr inline T ceil_exp(T x)
{
    if constexpr (N == 2)
        return ceil_exp2(x);

    constexpr T Shift = bit_log<1>(N);

    T ret = 1;
    T prev = 0;

    while (ret < x)
    {
        ret <<= Shift;

        if (ret < prev)
            break;

        prev = ret;
    }

    return ret;
}

template<typename T>
constexpr inline T ceil_exp(T x, u32 n)
{
    if (n == 2)
        return ceil_exp2(x);

    T Shift = bit_log<1>(n);
    T ret = 1;
    T prev = 0;

    while (ret < x)
    {
        ret <<= Shift;

        if (ret < prev)
            break;

        prev = ret;
    }

    return ret;
}

template<typename T1, typename T2>
constexpr inline T1 rotl(const T1 &a, T2 places)
{
    T2 mask = (8 * sizeof(a)) - 1;

    places &= mask;
    return (a << places) | (a >> ((-places) & mask));
}

template<typename T1, typename T2>
constexpr inline T1 rotr(const T1 &a, T2 places)
{
    T2 mask = (8 * sizeof(a)) - 1;

    places &= mask;
    return (a >> places) | (a << ((-places) & mask));
}

// generates a bitmask from bit 'from' to bit 'to'.
// e.g. bitmask(1, 4) == 00011110
template<typename T>
constexpr inline T bitmask(T from, T to)
{
    T upper = (sizeof(T) * 8 - 1 <= to)   ? max_value(T) : ((1 << (1 + to)) - 1);
    T lower = (sizeof(T) * 8 - 1 <= from) ? max_value(T) : ((1 << (from))   - 1);
    return upper ^ lower;
}

// generates bitmask between binary values from and to.
// e.g. bitmask_between_values(4, 8)  == 00001100
//      bitmask_between_values(4, 32) == 00111100
template<typename T>
constexpr inline T bitmask_between_values(T from, T to)
{
    // is there a better way to do this??
    if (from == 0 && to == 0)
        return 0;

    T max = (T)(-1); // must be unsigned
    T from_mask = from == 0 ? 0 : (floor_exp2(from) - 1);
    T to_mask   = to   == 0 ? 0 : (max ^ (ceil_exp2(to + 1) - 1));
    return (max - from_mask) - to_mask;
}

// extracts a range of bits of a given value val in range from-to.
template<typename T, typename TBit>
constexpr inline T bitrange(const T &val, TBit from, TBit to)
{
    return static_cast<T>((val & bitmask((u32)from, (u32)to)) >> from);
}

template<typename T>
constexpr inline bool is_pow2(T val)
{
    return (val != 0) && ((val & (val - 1)) == 0);
}

static constexpr inline u32 popcnt(u32 x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;
}

static constexpr inline u32 clz(u32 x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return 32 - popcnt(x);
}

static constexpr inline u32 ctz(u32 x)
{
    return popcnt((x & -x) - 1);
}
