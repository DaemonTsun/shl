
#pragma once

/* defer.hpp by https://stackoverflow.com/a/42060129

Allows the use of defer { ... }; blocks in code.

example:

string a = "hello"_s;
defer { free(&a); };

tprint("% world!\n", &a);

// no need to free here since the free is deferred


static_defer { Code; };
    Defers execution of Code to the end of the lifetime of a static object.

thread_local_defer { Code; }
    Defers execution of Code to the end of the lifetime of a thread_local object.

static_exec { Code; };
    Calls Code once as a static execution, as if Code were the constructor of a static object.

thread_local_exec { Code; };
    Calls Code once as a thread_local execution, as if Code were the constructor of a thread_local object.

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
#  define _DEFER2(X) zz_defer##X
#  define _DEFER(X) _DEFER2(X)
#  define defer auto _DEFER(__LINE__) = _defer{} * [&]()
#  define static_defer static auto _DEFER(__LINE__) = _defer{} * []()
#  define thread_local_defer thread_local auto _DEFER(__LINE__) = _defer{} * []()
#endif

// exec
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable : 4623) // implicitly deleted constructor
#pragma warning(disable : 4626) // implicitly deleted assignment operator
#endif
struct _d_execute {};
template <class F> struct _d_executer
{
    _d_executer(F f)
    {
        f();
    }
};
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop) 
#endif

template <class F>
_d_executer<F> operator*(_d_execute, F f)
{
    return _d_executer(f);
}

#ifndef static_exec
#  define _EXEC2(X) zz_exec##X
#  define _EXEC(X) _EXEC2(X)
#  define static_exec static auto _EXEC(__LINE__) = _d_execute{} * []()
#  define thread_local_exec thread_local auto _EXEC(__LINE__) = _d_execute{} * []()
#endif
