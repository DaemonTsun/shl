
// Linux exit syscalls

#pragma once

[[noreturn]] void exit(int status);
[[noreturn]] void exit_group(int status);
