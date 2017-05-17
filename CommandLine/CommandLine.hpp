#ifndef CommandLine_hpp
#define CommandLine_hpp
/**
 * A simple command line processor.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#include "CommandLine.h"

#include <vector>

class CommandLine
{
public:
    CommandLine() : processor(CL_Create()) {}
    ~CommandLine() { CL_Destroy(processor); }

    void AddCountingOption(int* value, char const* name)
        { CL_AddCountingOption(processor, value, name); }
    void AddIntegerOption(int* value, char const* name)
        { CL_AddIntegerOption(processor, value, name); }
    void AddArgument(char const** value)
        { CL_AddArgument(processor, value); }
    void EnableOverflowArguments()
        { CL_EnableOverflowArguments(processor); }
    std::vector<char const*> GetOverflowArguments()
        {
            char const** overflow = CL_GetOverflowArguments(processor);
            char const** end = overflow;
            while (*end != nullptr)
                ++end;
            return { overflow, end };
        }

    bool Parse(int argc, char const** argv) { return CL_Parse(processor, argc, argv) != 0; }

    char const* GetApplicationName() const { return CL_GetAppName(processor); }

private:
    CommandLineProcessor processor;
};

#endif // ndef CommandLine_hpp
