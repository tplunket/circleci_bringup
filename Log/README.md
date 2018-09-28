#  Log

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

# Building

CMake is can be used to build the test application for this "library", using the test script in the root directory with a parameter of "Log", but to include it in your own code it's probably just easiest to drop the source files that you want directly into your project.
