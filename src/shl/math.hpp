
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
