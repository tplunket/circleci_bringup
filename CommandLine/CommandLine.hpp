#ifndef CommandLine_hpp
#define CommandLine_hpp
/**
 * A simple command line processor.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 */

#include "CommandLine.h"

class CommandLine
{
public:
    CommandLine() : processor(CL_Create()) {}
    ~CommandLine() { CL_Destroy(processor); }

    void AddCountingOption(int* value, char const* name)
        { CL_AddCountingOption(processor, value, name); }

    bool Parse(int argc, char const** argv) { return CL_Parse(processor, argc, argv) == argc; }

    char const* GetApplicationName() const { return CL_GetAppName(processor); }

private:
    CommandLineProcessor processor;
};

#endif // ndef CommandLine_hpp
