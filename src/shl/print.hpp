#pragma once

/* print utility header
 * v1.0
 *
 * defines the common print(...) function
 */

#include <iostream>

template<typename OStreamT, typename T>
OStreamT &fprint(OStreamT &_os, T &&t)
{
    _os << t;
    return _os;
}

template<typename OStreamT, typename T, typename... Ts>
OStreamT &fprint(OStreamT &_os, T &&t, Ts &&...ts)
{
    _os << t;
    return fprint(_os, std::forward<Ts>(ts)...);
}

template<typename T>
void print(T &&t)
{
    fprint(std::cout, std::forward<T>(t));
}

template<typename T, typename... Ts>
void print(T &&t, Ts &&...ts)
{
    fprint(std::cout, std::forward<T>(t), std::forward<Ts>(ts)...);
}

inline void println()
{
    std::cout << std::endl;
}

template<typename T>
void println(T &&t)
{
    fprint(std::cout, std::forward<T>(t));
    std::cout << std::endl;
}

template<typename T, typename... Ts>
void println(T &&t, Ts &&...ts)
{
    fprint(std::cout, std::forward<T>(t), std::forward<Ts>(ts)...);
    std::cout << std::endl;
}

template<typename T>
void print_error(T &&t)
{
    fprint(std::cerr, std::forward<T>(t));
}

template<typename T, typename... Ts>
void print_error(T &&t, Ts &&...ts)
{
    fprint(std::cerr, std::forward<T>(t), std::forward<Ts>(ts)...);
}

inline void println_error()
{
    std::cerr << std::endl;
}

template<typename T>
void println_error(T &&t)
{
    fprint(std::cerr, std::forward<T>(t));
    std::cout << std::endl;
}

template<typename T, typename... Ts>
void println_error(T &&t, Ts &&...ts)
{
    fprint(std::cerr, std::forward<T>(t), std::forward<Ts>(ts)...);
    std::cout << std::endl;
}
