/**
 * A drop-in LogTarget that can be used for basic message logging in
 * applications that use the C++ standard library.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#ifndef StdStreamLogTarget_hpp
#define StdStreamLogTarget_hpp

#include "LogTarget.hpp"
#include <iostream>

struct StdStreamLogTarget : public LogTarget
{
    StdStreamLogTarget(bool annotate=false) : annotate(annotate) {}

private:
    void LogMessage(char const* message, LogType lt, char const* file, unsigned int line)
    {
        std::ostream& s = (lt < k_logError) ? std::cout : std::cerr;
        if (annotate)
            s << file << '(' << line << "): ";
        s << message;
    }

    bool annotate;
};

#endif // ndef StdStreamLogTarget_hpp
