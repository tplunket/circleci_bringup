/**
 * A C++ handler that simplifies setting up log targets for Log.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#ifndef PrintfLogTarget_hpp
#define PrintfLogTarget_hpp

#include "LogTarget.hpp"
#include <cstdio>

struct PrintfLogTarget : public LogTarget
{
	explicit PrintfLogTarget(bool annotate=false) : annotate(annotate) {}

private:
	void LogMessage(char const* message, LogType lt, char const* file, unsigned int line)
	{
		if (annotate)
			printf("%s(%d): %s", file, line, message);
		else
			printf("%s", message);
	}

	bool annotate;
};

#endif // ndef PrintfLogTarget_hpp
