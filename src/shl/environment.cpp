#include <string.h>

#include "shl/number_types.hpp"
#include "shl/environment.hpp"

#if Linux
#include <stdlib.h>
#include <errno.h>
extern char **environ;
#endif

const sys_char *get_environment_variable(const sys_char *name, error *err)
{
    if (name == nullptr)
        return nullptr;

#if Windows
    // TODO: implement
    return nullptr;
#else
    s64 len = strlen(name);
    u32 idx = 0;

    while (::environ[idx] != nullptr)
    {
        const sys_char *varline = ::environ[idx];
        const sys_char *i = varline;

        while (*i != '=' && *i != '\0')
            i++;

        if (*i == '\0')
            return nullptr;

        s64 equals = (s64)(i - varline);
        
        if (equals != len)
        {
            idx += 1;
            continue;
        }

        if (::strncmp(varline, name, equals - 1) != 0)
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
    // TODO: implement
    return false;
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

