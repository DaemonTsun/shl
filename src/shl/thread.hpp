
#pragma once

#include "shl/error.hpp"
#include "shl/time.hpp"
#include "shl/program_context.hpp"

typedef void *(*thread_function)(void *arg);

struct thread
{
    s32 thread_id;
    thread_function starting_function;
    void *argument;
    program_context *starting_context;
    void *os_thread_data;
};

bool thread_create(thread *t, thread_function func, void *argument = nullptr, error *err = nullptr);
bool thread_create(thread *t, thread_function func, void *argument, program_context *ctx_base, s64 stack_size, s64 storage_size, error *err = nullptr);
bool thread_destroy(thread *t, error *err = nullptr);

bool thread_start(thread *t, error *err = nullptr);
bool thread_is_ready(thread *t);
bool thread_is_running(thread *t);
bool thread_is_stopped(thread *t);

bool thread_stop(thread *t, timespan *wait_timeout = nullptr, error *err = nullptr);

// make sure thread is stopped and not destroyed 
void *thread_result(thread *t);
