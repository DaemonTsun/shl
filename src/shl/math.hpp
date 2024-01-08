
/*
 * math function header
 *
 * modulo(a, b)     calculates a % b, but also accepts floating point numbers.
 *
 * wrap_number(X, Min, Max)     wraps a number X between Min and Max, e.g.
 *                              wrap_number(12, 0, 9) -> 2.
 */

#pragma once

template<typename T1, typename T2>
auto modulo(T1 a, T2 b)
{
    auto rem = decltype(a / b)((long long)(a / b));
    return a - rem * b;
}

template<typename TVal, typename TMin, typename TMax>
auto wrap_number(TVal val, TMin min, TMax max)
{
    auto range = (max - min) + (TVal)1;
    return modulo(modulo((val - min), range) + range, range) + min;
}

template<typename T1, typename T2>
auto ceil_multiple(T1 x, T2 multiple)
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
auto ceil_multiple2(T1 x, T2 multiple)
{
    if (multiple == 0)
        return x;

    return (x + (multiple - 1)) & (-multiple);
}
