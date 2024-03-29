
#pragma once

/* defer.hpp by https://stackoverflow.com/a/42060129

allows the use of defer { ... }; blocks in code.

example:

string a = "hello"_s;
defer { free(&a); };

tprint("% world!\n", &a);

// no need to free here since the free is deferred
*/

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable : 4623) // implicitly deleted constructor
#pragma warning(disable : 4626) // implicitly deleted assignment operator
#endif
struct _defer {};
template <class F> struct _deferrer
{
    F f;
    ~_deferrer()
    {
        f();
    }

    // so that defer can be used in "if constexpr" on either side.
    // UPDATE: MSVC doesn't support this. I'm not surprised.
    // constexpr operator bool() { return true; }
};
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop) 
#endif

template <class F>
_deferrer<F> operator*(_defer, F f)
{
    return {f};
}

#ifndef defer
#define _DEFER2(X) zz_defer##X
#define _DEFER(X) _DEFER2(X)
#define defer auto _DEFER(__LINE__) = _defer{} * [&]()
#endif
