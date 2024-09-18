
#pragma once

/* exit.hpp

Exit the thread or process, or register functions to be executed when exiting.
(currently) Uses standard library atexit() to call all of the functions registered with
register_exit_function in order of registration.

*/

typedef void (*exit_function_t)();

[[noreturn]] void exit_thread(int status);
[[noreturn]] void exit_process(int status);

bool register_exit_function(exit_function_t func);
bool unregister_exit_function(exit_function_t func);
