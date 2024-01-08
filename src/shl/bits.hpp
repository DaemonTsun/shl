
#pragma once

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

