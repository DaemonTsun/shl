
#pragma once

/* error.hpp

Defines the error struct, a lightweight alternative to std::exception.
error has more information in debug builds, including the trace, file, function and line number.
An error must always be null initialized before (re)using it.

Functions:

set_error(*err, errcode, message)
    Writes the given error code and message to *err.
    In debug, file, function and line information is also set in *err.
    Does nothing if *err is nullptr.
    Debug builds: if err does not have a trace, creates a new trace and sets the error
        information (errcode, message, file, function, line) in err.
        If err does have a trace, add a new error with errcode, message, file,
        function and line number to the trace (but does not modify err).
        This means set_error does not modify an error if it already has a trace,
        and instead adds a new error to its trace.

forward_error(*err)
    Adds the current file, function and line trace information to the error trace of err.
    Does nothing in release builds.

format_error(*err, errcode, fmt, ...)
    Same as set_error, but formats the error message using format_error_message.

format_error_message(fmt, ...)
    Similar to snprintf, but returns a pointer to the formatted string.
    The pointer points to static ring buffer memory which may be overwritten
    with newer error messages, thus the error messages are shortlived.

set_error_by_code(*err, errcode)
    Calls set_error on err with errcode and a platform-specific error message.


Iterating traces:
for_error_trace(Frame, Trace, Err):
    Iterates Trace (of type error) from Err, with Frame being the depth of the trace.
    Example:

        error err{};
        set_error(&err, 10, "a");
        set_error(&err, 20, "b");

        for_error_trace(frame, e, &err)
            printf("%lld, %d, %s\n", frame, e->error_code, e->what);

    prints:
        0 10 a
        1 20 b


A typical pattern for error handling is to have an optional *error parameter in a function
and set the error if it's not nullptr (set_error, format_error, ... do nothing on nullptr),
e.g.:

    bool do_thing(int x, int y, error *err = nullptr)
    {
        if (x <= y)
        {
            format_error(err, "x (%d) must be greater than y (%d)", x, y);
            return false;
        }

        ...
    }

and to propagate the error, call forward_error() to add this point to the error trace:

    bool more_generic_function(error *err = nullptr)
    {
        int x = 0;
        int y = 2;

        if (!do_thing(x, y, err))
        {
            forward_error(err);
            return false;
        }
        ...
    }

The trace can then be handled with for_error_trace (only has the deepest error in release builds):

    int main()
    {
        error err{};
        
        if (!more_generic_function(&err))
        {
            for_error_trace(frame, e, &err)
                printf("[%lld][%s:%lu %s] %d: %s\n", frame,
                        e->file, e->line, e->function,
                        e->error_code, e->what);

            return -1;
        }
        ...
    }
*/

struct error
{
    int error_code;
    const char *what;

#ifndef NDEBUG
    const char    *file;
    unsigned long  line;
    const char    *function;
    void          *trace;
#endif
};

const char *format_error_message(const char *format, ...);

const char *errno_error_message(int errcode);
const char *windows_error_message(int errcode);

#ifndef NDEBUG
void _set_error(error *err, int error_code, const char *message,
                const char *file, const char *function, unsigned long line);

#  define forward_error(Err)\
    do { if ((Err) != nullptr) _set_error((Err), (Err)->error_code, "from here", __FILE__, __func__, __LINE__); } while (0)

#  define set_error(Err, Code, Msg) \
    do { if ((Err != nullptr)) _set_error(Err, Code, Msg, __FILE__, __func__, __LINE__); } while(0)


#else
// release
static inline void _set_error(error *err, int error_code, const char *message)
{
    if (err)
    {
        err->error_code = error_code;
        err->what = message;
    }
}

#  define forward_error(Err)\
    do {} while (0)

#  define set_error(Err, Code, Msg) \
    do { if ((Err != nullptr)) _set_error(Err, Code, Msg); } while(0)
#endif

#define set_errno_error(Err) \
    do { int _errcode = errno; set_error((Err), _errcode, ::errno_error_message(_errcode)); } while (0)

#define set_GetLastError_error(Err) \
    do { int _errcode = (int)GetLastError(); set_error((Err), _errcode, ::windows_error_message(_errcode)); } while (0)

#if Windows
#  define set_error_by_code(Err, Code) \
    do { int _errcode = (int)(Code); set_error((Err), _errcode, ::windows_error_message(_errcode)); } while (0)
#else // Linux
#  define set_error_by_code(Err, Code) \
    do { int _errcode = (int)(Code); set_error((Err), _errcode, ::errno_error_message(_errcode)); } while (0)
#endif

#define format_error(Err, Code, FMT, ...) \
    set_error(Err, Code, format_error_message(FMT __VA_OPT__(,) __VA_ARGS__))


// iterate traces
#ifndef NDEBUG
#  define for_error_trace_IV(I_Var, V_Var, ERR)\
    if constexpr (error *V_Var = (error*)((ERR)->trace); true)\
    for (s64 I_Var = 0; V_Var != nullptr; ++I_Var, V_Var = (error*)V_Var->trace)

#  define for_error_trace_V(V_Var, ARRAY)\
    for_error_trace_IV(V_Var##_index, V_Var, ARRAY)

#  define for_error_trace(...) GET_MACRO2(__VA_ARGS__, for_error_trace_IV, for_error_trace_V)(__VA_ARGS__)
#else // release
#  define for_error_trace_IV(I_Var, V_Var, ERR)\
    if constexpr (error *V_Var = (error*)(ERR); true)\
    if constexpr (s64 I_Var = 0; true)\

#  define for_error_trace_V(V_Var, ARRAY)\
    for_error_trace_IV(V_Var##_index, V_Var, ARRAY)

#  define for_error_trace(...) GET_MACRO2(__VA_ARGS__, for_error_trace_IV, for_error_trace_V)(__VA_ARGS__)
#endif
