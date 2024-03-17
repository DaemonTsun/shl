
#pragma once

/* math.hpp

Math function header.

modulo(a, b)     calculates a % b, but also accepts floating point numbers.

wrap_number(X, Min, Max)     wraps a number X between Min and Max, e.g.
                             wrap_number(12, 0, 9) -> 2.

ceil_multiple(x, N)     rounds x up to the next multiple of N
ceil_multiple2(x, N)    optimized version of ceil_multiple where N
                        is a power of 2.
*/


template<typename T1, typename T2>
inline constexpr auto modulo(T1 a, T2 b)
{
    auto rem = decltype(a / b)((long long)(a / b));
    return a - rem * b;
}

template<typename TVal, typename TMin, typename TMax>
inline constexpr auto wrap_number(TVal val, TMin min, TMax max)
{
    auto range = (max - min) + (TVal)1;
    return modulo(modulo((val - min), range) + range, range) + min;
}

template<typename T1, typename T2>
inline constexpr auto ceil_multiple(T1 x, T2 multiple)
{
    if (multiple == 0)
        return x;

    auto rest = modulo(x, multiple);

    if (rest == 0)
        return x;

    return (x + multiple) - rest;
}

// if multiple is power of 2
template<typename T1, typename T2>
inline constexpr auto ceil_multiple2(T1 x, T2 multiple)
{
    if (multiple == 0)
        return x;

    return (x + (multiple - 1)) & (-multiple);
}

template<typename T1, typename T2>
inline constexpr auto floor_multiple(T1 x, T2 multiple)
{
    if (multiple == 0)
        return x;

    auto rest = modulo(x, multiple);

    if (rest == 0)
        return x;

    return ((x + multiple) - rest) - multiple;
}

// if multiple is power of 2
template<typename T1, typename T2>
inline constexpr auto floor_multiple2(T1 x, T2 multiple)
{
    return x & (-multiple);
}
