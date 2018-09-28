#  ConvertToC

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

CMake is used to build the application. There are dependencies on both [Log](../Log) and [CommandLine](../CommandLine) although the source for those libraries is just dragged into the CMake environment for this project. Regardless, this project can be built by running the appropriate `test` script in the root directory with `CommandLine` as an argument.

