
#include "shl/program_context.hpp"

static program_context *_get_set_context(program_context *next = nullptr)
{
    thread_local program_context _tl_default_context = default_context;
    thread_local program_context *_ctx = &_tl_default_context;

    if (next == nullptr)
        return _ctx;

    program_context *prev = _ctx;
    _ctx = next;
    return prev;
}

program_context *get_context_pointer()
{
    return _get_set_context(nullptr); 
}

program_context *set_context_pointer(program_context *next)
{
    return _get_set_context(next); 
}
