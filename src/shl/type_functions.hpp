
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
template<typename T> using  _remove_reference_t = typename _remove_reference<T>::type;
#define remove_reference(T) _remove_reference<T>

template<typename T> struct _remove_pointer           { typedef T type; };
template<typename T> struct _remove_pointer<T*>       { typedef T type; };
template<typename T> struct _remove_pointer<T* const> { typedef T type; };
template<typename T> using  _remove_pointer_t = typename _remove_pointer<T>::type;
#define remove_pointer(T) _remove_pointer_t<T>

template<typename T1, typename T2> struct _is_same { static constexpr bool value = false; };
template<typename T>               struct _is_same<T, T> { static constexpr bool value = true;  };
template<typename T1, typename T2> inline constexpr bool _is_same_v = _is_same<T1, T2>::value;
#define is_same(T1, T2) _is_same<T1, T2>::value

template<typename T>
inline T&& forward(typename remove_reference(T)::type& t)
{
    return static_cast<T&&>(t);
}

template<typename T>
inline T&& forward(typename remove_reference(T)::type&& t)
{
    return static_cast<T&&>(t);
}
