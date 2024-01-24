
#pragma once

/* type_functions.hpp

Type function header. Defines some type template functions because stl is bad.

All functions may be used at compile time.

Functions:

underlying_type(T)  gets the underlying type of T if there is one, e.g. for
                    enums.

remove_reference(T) gets the type T without references.
remove_pointer(T)   gets the type T without pointers or const pointers.
remove_const(T)     gets the type T without const.

is_same(T1, T2)     is true if T1 and T2 are the same type, false otherwise.
is_signed(T)        is true if T is a signed number type.
forward<T>(x)       "forwards" x.
*/

// update 01.10.2023: MSVC incorrectly handles __underlying_type, so we need
// an explicit using type = __underlying_type (typedef doesn't work).

template<typename T> struct _underlying_type { using type = __underlying_type(T); };
#define underlying_type(T) _underlying_type<T>::type

template<typename T> struct _remove_reference         { typedef T type; };
template<typename T> struct _remove_reference<T&>     { typedef T type; };
template<typename T> struct _remove_reference<T&&>    { typedef T type; };
#define remove_reference(T) _remove_reference<T>::type

template<typename T> struct _remove_pointer           { typedef T type; };
template<typename T> struct _remove_pointer<T*>       { typedef T type; };
template<typename T> struct _remove_pointer<T* const> { typedef T type; };
#define remove_pointer(T) _remove_pointer<T>::type

template<typename T> struct _remove_const          { typedef T type; };
template<typename T> struct _remove_const<const T> { typedef T type; };
#define remove_const(T) _remove_const<T>::type

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

template<bool Cond, typename T1, typename T2>
inline constexpr decltype(auto) _inline_const_if(T1 &&a, T2 &&b)
{
    if constexpr (Cond)
        return forward<T1>(a);
    else
        return forward<T2>(b);
}

#define inline_const_if(Cond, If, Else)\
    _inline_const_if<Cond>(If, Else)
