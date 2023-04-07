
#pragma once

/* defer.hpp by https://stackoverflow.com/a/42060129

allows the use of defer { ... }; blocks in code.

example:

string a = "hello"_cs;
defer { free(&a); }

tprint("% world!\n", &a);

// no need to free here since the free is deferred
 */

struct _defer {};
template <class F> struct _deferrer
{
    F f;
    ~_deferrer()
    {
        f();
    }
};

template <class F>
_deferrer<F> operator*(_defer, F f)
{
    return {f};
}

#define _DEFER2(X) zz_defer##X
#define _DEFER(X) _DEFER2(X)
#define defer auto _DEFER(__LINE__) = _defer{} * [&]()
