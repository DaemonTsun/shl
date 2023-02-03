# shl
at this point this is just a wrapper/replacement library for the standard library.

## features

- `file_stream` and `memory_stream`: consistent API for streams.
- `debug(fmt, ...)` and `trace(fmt, ...)`: debugging printf
- `hash_t`: hashing of arbitrary data
- `error`: alternative to std::exception
- `s32`, `s64`, u32`, ...: number types
- `#if Linux`: better platform preprocessor constants
- `parse_X`: parsing functions
- and more

See individual header files for documentation.

## building

make sure git submodules are properly initialized with `git submodule update --init --recursive`, then run the following:

```sh
$ mkdir bin
$ cd bin
$ cmake ..
$ make
```

## installation

`sudo make install` inside the build directory.

## tests
If [t1](https://github.com/DaemonTsun/t1) is installed, tests can be run as well using `make tests && make runtests`.
