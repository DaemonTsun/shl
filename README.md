# shl
Simple c++ libraries designed to be copied into projects. Most of them only use a single header file, others come with a cpp file for implementation as well.

### [print.hpp](https://github.com/DaemonTsun/shl/blob/master/src/print.hpp)
Provides the `print(...)` function to print arbitrary arguments to `stdout`.

### [dprint.hpp](https://github.com/DaemonTsun/shl/blob/master/src/dprint.hpp)
Depends on `print.hpp`. Provides `dprint(...)` and other printing functions which do nothing (the macros expand to no code) if `NDEBUG` is set.

### [enum_flag.hpp](https://github.com/DaemonTsun/shl/blob/master/src/enum_flag.hpp)
Defines the `ENUM_CLASS_FLAG_OPS` function macro which, when given a `enum class` name, defines operators and functions to work with an enum class flag, e.g. `is_set`, `set`, `unset`, etc.

### [number_types.hpp](https://github.com/DaemonTsun/shl/blob/master/src/number_types.hpp)
Defines a few number types like `s64`, `s32`, `u64`, etc.

### [string.hpp](https://github.com/DaemonTsun/shl/blob/master/src/string.hpp)
Defines many common string functions like `trim`, `to_upper`, `to_lower`, `begins_with`, and more.
Also comes with the `str` function to convert arbitrary arguments to a single string, like `str(1, "hello", 'c') -> "1helloc"`.

### [parse.hpp](https://github.com/DaemonTsun/shl/blob/master/src/parse.hpp)
Depends on `string.hpp` and `number_types.hpp`. Defines very basic and common parsing functions to parse C-style comments, whitespace, integers, floating-point numbers, strings, identifiers and boolean values.

### [parse_object.hpp](https://github.com/DaemonTsun/shl/blob/master/src/parse_object.hpp)
Depends on `parse.hpp`. an extension to `parse.hpp`, this library defines additional parsing functions for parsing objects which can be either integers, floating-point numbers, strings, identifiers, booleans, lists of objects or tables of objects where identifiers are the keys.
The symbols used to parse the objects are defined at the top of the file and can easily be changed.
An object may look like this by default:

```
{
  a: 10,
  b: "hello",
  c: [1,2,3],
  d: {
    x: 5
  }
}
```

### [file_stream](https://github.com/DaemonTsun/shl/blob/master/src/file_stream.hpp)
Comes with a cpp file for definitions. Defines the `file_stream` struct which is a very light wrapper around `FILE*`. Defines a bunch of functions for reading and writing data, calculating size, reading the entire file or reading and writing blocks of data.
The point of this is to have a more consistent file reading and writing API.

### [filesystem](https://github.com/DaemonTsun/shl/blob/master/src/filesystem.hpp)
Right now it only defines the `get_executable_path` function to obtain the path to the current executable.

### [filesystem_watcher](https://github.com/DaemonTsun/shl/blob/master/src/filesystem_watcher.hpp)
Comes with a cpp file for definitions. Declares the `filesystem_watcher` struct, as well as multiple functions to watch files on the filesystem with.
The watcher, once started, runs in its own thread and calls the callback function when it detects changes in the watched files.
The callback function is called from the other thread.
For an example usage, see the [demo](https://github.com/DaemonTsun/shl/blob/master/demos/filesystem_watcher_demo/src/main.cpp).

### [error](https://github.com/DaemonTsun/shl/blob/master/src/error.hpp)
Comes with a cpp file for definitions. A simple error struct to replace STL exceptions with.
When using the formatting overload or the `format_error` function, the error message is written to a static buffer (by default of size 2048 chars).

## tests
If [t1](https://github.com/DaemonTsun/t1) is installed, tests can be run as well using `make tests && make runtests`.
