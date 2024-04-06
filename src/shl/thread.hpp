
#pragma once

#include "shl/error.hpp"
#include "shl/program_context.hpp"

typedef void *(*thread_function)(void *arg);

struct thread
{
    s32 thread_id;
    thread_function starting_function;
    void *argument;
    program_context *starting_context;
};

void init(thread *t, thread_function func, void *argument = nullptr, program_context *ctx = nullptr);

bool thread_start(thread *t, error *err = nullptr);
bool thread_is_done(thread *t);
bool thread_stop(thread *t, error *err = nullptr);
bool thread_destroy(thread *t, error *err = nullptr);
