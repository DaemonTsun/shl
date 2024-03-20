
#include "shl/string.hpp"
#include "shl/number_types.hpp"
#include "shl/environment.hpp"

#if Windows
#include <windows.h>
#else
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern char **environ;
#endif

const sys_char *get_environment_variable(const sys_char *name, error *err)
{
    s64 len = string_length(name);

    return get_environment_variable(name, len, err);
}

const sys_char *get_environment_variable(const sys_char *name, s64 name_len, [[maybe_unused]] error *err)
{
    if (name_len == 0)
        return nullptr;

#if Windows
    const sys_char *vars = GetEnvironmentStrings();

    if (vars == nullptr)
    {
        set_GetLastError_error(err);
        return nullptr;
    }

    while (*vars != '\0')
    {
        const sys_char *varline = vars;
        
        while (*vars != '=' && *vars != '\0')
            vars++;

        if (*vars == '\0')
        {
            vars++;
            continue;
        }

        s64 equals = (s64)(vars - varline);
        
        if (equals != name_len)
        {
            while (*vars != '\0')
                vars++;
            vars++;

            continue;
        }

        if (compare_strings(varline, name, equals - 1) != 0)
        {
            while (*vars != '\0')
                vars++;
            vars++;

            continue;
        }

        return varline + equals + 1;
    }

    return nullptr;
#else
    u32 idx = 0;

    while (::environ[idx] != nullptr)
    {
        const sys_char *varline = ::environ[idx];
        const sys_char *i = varline;

        while (*i != '=' && *i != '\0')
            i++;

        if (*i == '\0')
        {
            idx += 1;
            continue;
        }

        s64 equals = (s64)(i - varline);
        
        if (equals != name_len)
        {
            idx += 1;
            continue;
        }

        if (compare_strings(varline, name, equals - 1) != 0)
        {
            idx += 1;
            continue;
        }

        return varline + equals + 1;
    }

    return nullptr;
#endif
}

bool set_environment_variable(const sys_char *name, const sys_char *value, bool overwrite, error *err)
{
#if Windows
    if (!overwrite && get_environment_variable(name, err) != nullptr)
        return false;

    if (!SetEnvironmentVariable(name, value))
    {
        set_GetLastError_error(err);
        return false;
    }

    return true;
#else
    if (value == nullptr)
    {
        if (::unsetenv(name) != 0)
        {
            set_errno_error(err);
            return false;
        }

        return true;
    }

    if (::setenv(name, value, overwrite ? 1 : 0) != 0)
    {
        set_errno_error(err);
        return false;
    }

    return true;
#endif
}

