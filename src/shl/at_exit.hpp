
#pragma once

/* at_exit.hpp

Register functions to be executed when exiting.
Uses standard library atexit() to call all of the functions registered with
register_exit_function in order of registration.

*/

typedef void (*exit_function_t)();

bool register_exit_function(exit_function_t func);
bool unregister_exit_function(exit_function_t func);
