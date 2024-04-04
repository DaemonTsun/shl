
// Linux exit syscalls

#pragma once

extern "C"
{
[[noreturn]] void exit(int status);
[[noreturn]] void exit_group(int status);
}
