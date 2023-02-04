
#pragma once

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
    return (((1 << (1 + to)) - 1) ^ ((1 << (from)) - 1));
}

// extracts a range of bits of a given value val in range from-to.
template<typename T, typename TBit>
constexpr inline T bitrange(const T &val, TBit from, TBit to)
{
    return static_cast<T>((val & bitmask(from, to)) >> from);
}

