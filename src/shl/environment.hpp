
/* environment.hpp

Environment variables.

Functions:

get_environment_variable(Name[, *err])
    Returns a pointer to the contents of the environment variable Name, or
    nullptr if it was not found. On Linux, Name is case-sensitive.

set_environment_variable(Name, Value, Overwrite = true[, *err])
    Sets the environment variable Name to Value, overwriting existing variables if
    Overwrite is true.
    If Value is nullptr, unsets Name.
    Returns whether or not the function succeeded.
*/

#pragma once

#include "shl/error.hpp"
#include "shl/platform.hpp"

const sys_char *get_environment_variable(const sys_char *name, error *err = nullptr);
const sys_char *get_environment_variable(const sys_char *name, s64 name_len, error *err = nullptr);
bool set_environment_variable(const sys_char *name, const sys_char *value, bool overwrite = true, error *err = nullptr);
