
#pragma once

/* string_manip.hpp
 * string manipulation functions
 * v1.0
 *
 *
 * to_string(...)           converts the arguments to string
 * to_wstring(...)          converts the arguments to wstring
 * to_basic_string<CharT>(...) converts the arguments to basic_string<CharT>
 */

#include <string>
#include <sstream>

#include "shl/type_functions.hpp"


namespace internal
{
    template<typename T>
    using stream_string = std::basic_string<typename T::char_type, typename T::traits_type, typename T::allocator_type>;

    template<typename StringStreamT, typename T>
    stream_string<StringStreamT> _str(StringStreamT &_ss, T &&t)
    {
        _ss << t;
        return _ss.str();
    }

    template<typename StringStreamT, typename T, typename... Ts>
    stream_string<StringStreamT> _str(StringStreamT &_ss, T &&t, Ts &&...ts)
    {
        _ss << t;
        return _str(_ss, forward<Ts>(ts)...);
    }
}

template<typename CharT, typename T>
std::basic_string<CharT> to_basic_string(T &&t)
{
    using namespace internal;
    std::basic_stringstream<CharT> _ss;
    _ss << t;
    return _ss.str();
}

template<typename CharT, typename T, typename... Ts>
std::basic_string<CharT> to_basic_string(T &&t, Ts &&...ts)
{
    using namespace internal;
    std::basic_stringstream<CharT> _ss;
    return _str(_ss, forward<T>(t), forward<Ts>(ts)...);
}

template<typename T>
std::string to_string(T &&t)
{
    return to_basic_string<std::string::value_type>(forward<T>(t));
}

template<typename T, typename... Ts>
std::string to_string(T &&t, Ts &&...ts)
{
    return to_basic_string<std::string::value_type>(forward<T>(t), forward<Ts>(ts)...);
}

template<typename T>
std::wstring to_wstring(T &&t)
{
    return to_basic_string<std::wstring::value_type>(forward<T>(t));
}

template<typename T, typename... Ts>
std::wstring to_wstring(T &&t, Ts &&...ts)
{
    return to_basic_string<std::wstring::value_type>(forward<T>(t), forward<Ts>(ts)...);
}
