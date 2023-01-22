
#pragma once

template<typename T>
struct underlying_type { typedef __underlying_type(T) type; };

template<typename T>
struct remove_reference { typedef T type; };

template<typename T>
struct remove_reference<T&> { typedef T type; };

template<typename T>
struct remove_reference<T&&> { typedef T type; };

template<typename T>
inline T&& forward(typename remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}

template<typename T>
inline T&& forward(typename remove_reference<T>::type&& t)
{
    return static_cast<T&&>(t);
}
