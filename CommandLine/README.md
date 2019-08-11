# CommandLine

A library for easily loading the command line into variables. Variables, either global or local in
scope context, are directly connected to command line options and automatically loaded.

## Glossary

An **option** is an optional item appearing on the command line, preceded by '-'. May require (a fixed number of) parameters.

A **parameter** is an item, perhaps one of several, connected to an option.

An **argument** is a non-option item on the command line. Can be fixed in number or the system can be configured to collect the "overflow" arguments.

## Usage

CommandLine is written in C and offers a C++ wrapper which is convenient and very light weight.

### C

The C interface is the one upon which everything else sits. Typical usage is to create the "parser object," add a bunch of options to it, parse the command line, then destroy the parser object.

```c
int g_value = 0;

int HandleCommandLine(int argc, const char** argv)
{
    CommandLineProcessor* clp = CL_Create();

    CL_AddIntegerOption(clp, &value, "value");

    CL_Parse(clp, argc, argv);
    CL_Destroy(clp);
}

int main(int argc, const char** argv)
{
    HandleCommandLine(argc, argv);
    printf("Given value is %d.\n", g_value);
}
```

There's no requirement for values to receive command line data to be global, they can just as easily be local variables.

### C++

### `UNICODE` (under Windows)

CommandLine can be built to support `wchar_t` as its character type, which is the default character type for the Windows command line. This is turned on by default but can be overridden by setting `CL_USE_wchar_t` to zero (or if the CMake build is used, by passing `-DUSE_wchar_t=off` on the CMake command line).
