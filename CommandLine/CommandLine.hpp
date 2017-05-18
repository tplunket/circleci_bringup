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

    void AddCountingOption(int* value, CL_StringType name)
        { CL_AddCountingOption(processor, value, name); }
    void AddIntegerOption(int* value, CL_StringType name)
        { CL_AddIntegerOption(processor, value, name); }
    void AddFloatOption(float* value, CL_StringType name)
        { CL_AddFloatOption(processor, value, name); }
    void AddStringOption(CL_StringType* value, CL_StringType name)
        { CL_AddStringOption(processor, value, name); }
    void AddArgument(CL_StringType* value)
        { CL_AddArgument(processor, value); }
    void EnableOverflowArguments()
        { CL_EnableOverflowArguments(processor); }
    std::vector<CL_StringType> GetOverflowArguments()
        {
            CL_StringType* overflow = CL_GetOverflowArguments(processor);
            CL_StringType* end = overflow;
            while (*end != nullptr)
                ++end;
            return { overflow, end };
        }

    bool Parse(int argc, CL_StringType* argv) { return CL_Parse(processor, argc, argv) != 0; }

    CL_StringType GetApplicationName() const { return CL_GetAppName(processor); }

private:
    CommandLineProcessor processor;
};

#endif // ndef CommandLine_hpp
