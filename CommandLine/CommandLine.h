#ifndef CommandLine_h
#define CommandLine_h
/**
 * A simple command line processing system
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 *
 * Definitions:
 *
 *    Option: an optional item appearing on the command line, preceded by '-'. May require (a fixed
 *            number of) parameters.
 *    Parameter: an item, perhaps one of several, connected to an option.
 *    Argument: a non-option item on the command line. Can be fixed in number and the system can be
 *              configured to collect the overflow.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CL_USE_wchar_t
#include <wchar.h>
typedef wchar_t CL_CharType;
#else
typedef char CL_CharType;
#endif

typedef CL_CharType const* CL_StringType;

struct CommandLineProcessor_;
typedef struct CommandLineProcessor_* CommandLineProcessor;

/// Lifetime
CommandLineProcessor CL_Create();
void CL_Destroy(CommandLineProcessor);

/// Options
void CL_AddCountingOption(CommandLineProcessor, int* value, CL_StringType name);
void CL_AddIntegerOption(CommandLineProcessor, int* value, CL_StringType name);
void CL_AddFloatOption(CommandLineProcessor, float* value, CL_StringType name);
void CL_AddStringOption(CommandLineProcessor, CL_StringType* value, CL_StringType name);

/// Arguments
void CL_AddArgument(CommandLineProcessor, CL_StringType* value);
void CL_EnableOverflowArguments(CommandLineProcessor);
CL_StringType* CL_GetOverflowArguments(CommandLineProcessor);

/// Post
CL_StringType CL_GetAppName(CommandLineProcessor);
int CL_Parse(CommandLineProcessor, int argc, CL_StringType* argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ndef CommandLine_h
