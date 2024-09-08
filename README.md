# shl
A replacement library for the standard library, because STDs are no good.

## Features

- [`array`](src/shl/array.hpp), [`fixed_array`](src/shl/fixed_array.hpp), [`chunk_array`](src/shl/chunk_array.hpp), [`set`](src/shl/set.hpp), [`linked_list`](src/shl/linked_list.hpp), [`hash_table`](src/shl/hash_table.hpp): lightweight containers that don't take 10 billion years to compile and are easier to use
- [`string`, `const_string`](src/shl/string.hpp): lightweight string library with UTF support
- [`format` and `to_string`](src/shl/format.hpp): formatting library with a better and type-safe interface
- [`s32`, `s64`, `u32`, ...](src/shl/number_types.hpp): shorter number types
- [type functions](src/shl/type_functions.hpp): compile-time functions for type information, like `is_same(T1, T2)`
- [`defer { ... };`](src/shl/defer.hpp): defer statements in C++
- [`file_stream`](src/shl/file_stream.hpp) and [`memory_stream`](src/shl/memory_stream.hpp): consistent API for streams
- [`put`, `tprint`](src/shl/print.hpp): I/O writing and formatting, specifically a better "printf" that doesn't use `stdio.h`
- [`breakpoint()`, `breakpoint(1)`, `enable_breakpoint(1)`, ...](src/shl/debug.hpp): debugging utilities for setting (conditional) breakpoints directly in code
- [`hash_t`](src/shl/hash.hpp): fast hashing of arbitrary data
- [`mt19937`, `pcg64`, `next_random_int`, ...](src/shl/random.hpp): random number generation and distrubution 
- [`__FILE_HASH__` and `__LINE_HASH__`](src/shl/murmur_hash.hpp): constexpr implementation of Murmur Hash 3, with a macro to provide a unique hash for the current line in the current file
- [`error`](src/shl/error.hpp): alternative to `std::exception`
- [`#if Linux`](src/shl/platform.hpp): better platform preprocessor constants
- [`#if MSVC`](src/shl/compiler.hpp): better compiler preprocessor constants
- [`rotl`, `rotr`, `bitmask`, ...](src/shl/bits.hpp): bit manipulation functions and macros
- [`alloc`, `dealloc`, `move_memory`, `copy_memory`](src/shl/memory.hpp): type-safe memory management functions
- [`allocator`](src/shl/allocator.hpp): allocator type for using different types of allocators
- [`arena_allocator`](src/shl/allocator_arena.hpp): arena allocator
- [`program_context`](src/shl/program_context.hpp): (per thread) context for setting "global" information, e.g. allocator
- [`sleep(float seconds)`](src/shl/time.hpp): multiplatform time functions
- [`process_create`, `process_start`, ...](src/shl/process.hpp): process management
- [`thread_create`, `thread_start`, ...](src/shl/thread.hpp): thread management
- [`pipe`](src/shl/pipe.hpp): OS pipes
- [`scratch_buffer`](src/shl/scratch_buffer.hpp): a buffer with a fixed part on the stack, which may grow onto the heap
- [`ring_buffer`](src/shl/ring_buffer.hpp): a circular buffer using virtual addresses
- [`linux_syscall`](src/shl/impl/linux/syscalls.hpp): syscalls without libc
- and more

See individual header files for documentation.

## Usage

Either clone the repostory and simply include it in CMake like so:

```cmake
add_subdirectory(path/to/shl)
target_link_libraries(your-target PRIVATE shl-0.10.0)
target_include_directories(your-target PRIVATE ${shl-0.10.0_SOURCES_DIR})
```

OR follow the next steps to build and install it and manually link and include it.

### Building (optional)

If shl is not included in CMake and should be installed instead, run the following from the root of the repository:

```sh
$ cmake <dir with CMakeLists.txt> -B <output dir>
$ cmake --build <output dir>
```

### Installation (optional)

`cmake --install <output dir>` inside the build directory.

## Tests (optional)

Tests can be built by specifying the `-DTests=1` command line flag in the `cmake` command and the tests can be run using `cmake --build <output dir> --target runtests` or `ctest --test-dir <output dir>`.
