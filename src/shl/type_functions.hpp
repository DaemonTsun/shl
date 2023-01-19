
#pragma once

template <class T>
struct remove_reference { typedef T type; };

template <class T>
struct remove_reference<T&> { typedef T type; };

template <class T>
struct remove_reference<T&&> { typedef T type; };

template <class T>
inline T&& forward(typename remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}

template <class T>
inline T&& forward(typename remove_reference<T>::type&& t)
{
    return static_cast<T&&>(t);
}
