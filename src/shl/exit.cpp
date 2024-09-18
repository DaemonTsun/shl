
#include <stdlib.h> // atexit

#include "shl/array.hpp"
#include "shl/exit.hpp"
#include "shl/platform.hpp"

#if Windows
#  include <windows.h>
#elif Linux
#  include "shl/impl/linux/exit.hpp"
#endif

// TODO: once we don't use libc anymore, add atexit callbacks to these.
[[noreturn]] void exit_thread(int status)
{
#if Windows
    ExitThread((DWORD)status);
#else
    exit(status);
#endif
}

[[noreturn]] void exit_process(int status)
{
#if Windows
    ExitProcess((DWORD)status);
#else
    exit_group(status);
#endif
}

typedef array<exit_function_t> _exit_function_container;

_exit_function_container *_get_exit_functions(bool free_functions);
int atexit(void (*function)(void));

static void _atexit_callback()
{
    _exit_function_container *funcs = _get_exit_functions(false);

    // funcp = pointer to function
    for_array(funcp, funcs)
        (*funcp)();

    _get_exit_functions(true);
}

static void _register_atexit_callback()
{
    static bool _registered = false;

    if (!_registered)
    {
        ::atexit(_atexit_callback);
        _registered = true;
    }
}

_exit_function_container *_get_exit_functions(bool free_functions)
{
    static _exit_function_container _funcs{};

    if (_funcs.data == nullptr && !free_functions)
    {
        init(&_funcs);
        _register_atexit_callback();
    }
    else if (_funcs.data != nullptr && free_functions)
    {
        free(&_funcs);
        return nullptr;
    }

    return &_funcs;
}

bool register_exit_function(exit_function_t func)
{
    _exit_function_container *funcs = _get_exit_functions(false);

    if (contains(funcs, func))
        return false;

    add_at_end(funcs, func);

    return true;
}

bool unregister_exit_function(exit_function_t func)
{
    _exit_function_container *funcs = _get_exit_functions(false);

    s64 idx = index_of(funcs, func);

    if (idx == -1)
        return false;

    remove_elements(funcs, idx, 1);
    return true;
}
