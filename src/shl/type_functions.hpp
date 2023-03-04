
#pragma once

/* type_functions.hpp
 *
 * defines some type template functions because stl is bad.
 */

template<typename T> struct _underlying_type { typedef __underlying_type(T) type; };
#define underlying_type(T) __underlying_type(T)

template<typename T> struct _remove_reference         { typedef T type; };
template<typename T> struct _remove_reference<T&>     { typedef T type; };
template<typename T> struct _remove_reference<T&&>    { typedef T type; };
#define remove_reference(T) _remove_reference<T>::type

template<typename T> struct _remove_pointer           { typedef T type; };
template<typename T> struct _remove_pointer<T*>       { typedef T type; };
template<typename T> struct _remove_pointer<T* const> { typedef T type; };
#define remove_pointer(T) _remove_pointer<T>::type

template<typename T1, typename T2> struct _is_same { static constexpr bool value = false; };
template<typename T>               struct _is_same<T, T> { static constexpr bool value = true;  };
#define is_same(T1, T2) _is_same<T1, T2>::value

template<typename T> struct _is_signed { static constexpr bool value = (T(-1) < T(0)); };
#define is_signed(T) _is_signed<T>::value

template<typename T>
inline T&& forward(typename remove_reference(T)& t)
{
    return static_cast<T&&>(t);
}

template<typename T>
inline T&& forward(typename remove_reference(T)&& t)
{
    return static_cast<T&&>(t);
}

template <bool Cond, typename T1, typename T2>
inline constexpr decltype(auto) _inline_const_if(T1 &&a, T2 &&b)
{
    if constexpr (Cond)
        return forward<T1>(a);
    else
        return forward<T2>(b);
}

#define inline_const_if(Cond, If, Else)\
    _inline_const_if<Cond>(If, Else)
