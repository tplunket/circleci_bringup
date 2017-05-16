/**
 * A C++ handler that simplifies setting up log targets for Log.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#ifndef LogTarget_hpp
#define LogTarget_hpp

#include "Log.h"

class LogTarget
{
public:
    LogTarget() { LogTargetAdd(&Trampoline, this); }
    ~LogTarget() { LogTargetRemove(&Trampoline, this); }

private:
    virtual void LogMessage(char const* message, LogType lt,
                            char const* file, unsigned int line) = 0;

    static void Trampoline(char const* m, LogType lt, char const* file, unsigned int line, void* d)
    {
        ((LogTarget*)d)->LogMessage(m, lt, file, line);
    }
};

#endif // ndef LogTarget_hpp
