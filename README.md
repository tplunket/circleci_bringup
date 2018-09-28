# Little Utilities

1. [`Log`](#markdown-header-log), a simple logging library, written in C with a super easy to use C++ wrapper.
2. [`CommandLine`](#markdown-header-commandline), a simple command line processor which loads command line arguments directly into variables.
3. [`ConvertToC`](#markdown-header-converttoc), a small program to convert data files into C character arrays.

These are the bits of utility code that I find myself implementing and reimplementing so I just did it one more time and am releasing all of this code under the MIT/X license. The primary goal is that it's all easy to build and use; hopefully the header files give the user all of the info the using programmer will need and in the case of applications the help output should make use obvious.

## Log

*Log* is a super simple library for logging. It does the necessary string formatting and then passes that string to any number of *log targets* which are specified at application startup.

As with many other logging systems, log messages are wrapped in macros indicating the *severity* of the message. Severities are, in decreasing importance, `Error`, `Warning`, `Info`, and `Spew`. The macros accept printf-style formatting specifiers and the formatting is done with system functionality so use whatever your library supports.

Use should be pretty obvious:

```c
if (val != 5)
    Warning("val should be 5 but it is %d.", val);
else
    Info("val is 5 as expected.");
```

So then your *log targets* are functions that receive the formatted log message along with the important details such as the severity and source file and line. When a log target is configured, the user can provide a pointer to whatever the user might want to have subsequently provided to the *log target*. Trailing newlines are added implicitly, although if the string ends with a newline another won't be added.

The simplest *log target* in C would look something like this:

```c
#include "Log/Log.h"

void my_log_target(char const* m, LogType lt, char const* file, unsigned int line, void* d)
{
    char (*severities)[] = { "error", "warning", "debug", "spew" };
    printf("%s- %s(%u): %s", severities[lt], file, line, m);
}

void init_logging()
{
    LogTargetAdd(my_log_target, NULL);
}

void deinit_logging()
{
    LogTargetRemove(my_log_target, NULL);
}
```

Of course, many log target implementations will do a little more, setting console colors or filtering based on the log type. The optional callback object can be used to store additional context as necessary, and it is through this pointer that the C++ wrapper passes its object state.

For C++ there is a `LogTarget` object which is subclassed and instantiated to provide similar functionality. There are two simple options provided, one uses `printf` and the other writes to `std::cout` or `std::cerr` depending on the severity. Using one of these is dead simple:

```c++
#include "Log/StdStreamLogTarget.hpp"

int main()
{
    StdStreamLogTarget logTarget;
    Info("Logging initialized.");
}
```

Use of the C and C++ APIs can be mixed and matched as appropriate to the application as the differences are restricted to the *log targets*; the logging messages themselves are just macros that call the C API under the hood.

## CommandLine

The *CommandLine* library is also available in C or C++ flavors.

To support Unicode on Windows platforms, CommandLine can be configured to use `wchar_t`.

Beyond that, the documentation for this library is sufficiently large to justify not including it here. Check out the [CommandLine](CommandLine) subproject for more info.

## ConvertToC

A single-file application that is usually hooked into build systems to convert data files into C source. Default behavior is to spit out a `const char[]`, although this can be changed via command line options.

`test_file.txt`:

```
This is a test file.
It has quite int'resting lines.
Just an example.
```

processed with `./ConvertToC test_file.txt` results in `test_file.txt.c`:

```c
const unsigned int k_test_file_txt_length = 76;
const char k_test_file_txt[] =
    "This is a test file.\n"
    "It has quite int'resting lines.\n"
    "Just an example.\n"
    "";
```

Alternatively, "pure binary" can be spit out with the `-b` command line switch (or `-bin` or `-binary`), `./ConvertToC test_file.txt -b`:

```c
const unsigned int k_test_file_txt_length = 76;
const unsigned char k_test_file_txt[] =
{
    /* byte 0 */
     84, 104, 105, 115,  32, 105, 115,  32,  97,  32, 116, 101, 115, 116,  32, 102,
    105, 108, 101,  46,  10,  73, 116,  32, 104,  97, 115,  32, 113, 117, 105, 116,
    101,  32, 105, 110, 116,  39, 114, 101, 115, 116, 105, 110, 103,  32, 108, 105,
    110, 101, 115,  46,  10,  84, 104, 111, 115, 101,  32, 108, 105, 110, 101, 115,
     32,  97, 114, 101,  32, 104,  97, 105, 107, 117,  46,  10,   0,
};
```

Hex values can also be printed with the use of `-h`/`-hex`. Binary data always has a zero appended (which mirrors the behavior when a string is exported) although the reported size is the actual file size.

One cool feature, if I may be so bold, is that multiline text embedded in otherwise binary data is formatted "nicely," so that it can be easily read.

# Building

CMake is used to build the applications, including test applications for the libraries. The libraries though are simple enough that it's probably easiest just to drop the code into your project. If you have CMake installed though, you can just run `test.bat` or `./test.sh` with the name of the project you want to build, e.g. `test.bat CommandLine`.
