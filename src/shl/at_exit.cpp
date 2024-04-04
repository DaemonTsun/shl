
#include <stdlib.h>

#include "shl/array.hpp"
#include "shl/at_exit.hpp"

typedef array<exit_function_t> _exit_function_container;

_exit_function_container *_get_exit_functions(bool free_functions);

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
