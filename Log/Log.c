/*- Log.c -------------------------------------------------------------------*\
|
|   Contents:   A simple logging system.
|
|   Author:     Tom Plunket <tom@mightysprite.com>
|
|   Copyright:  (c) 2010 Tom Plunket, all rights reserved
|
\*----------------------------------------------------------------------------*/

#include "Log.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define k_bufferSize 256
#define k_maxNumTargets 4

static LogTarget l_logTargets[k_maxNumTargets] = { 0 };

//------------------------------------------------
// LogTargetAdd
//
// Add a function to the list of functions called with logging output
void LogTargetAdd(LogTarget function)
{
    unsigned int i;

    for (i = 0; i < k_maxNumTargets; ++i)
    {
        if (l_logTargets[i] == NULL)
        {
            l_logTargets[i] = function;
            break;
        }
    }
}

//------------------------------------------------
// LogTargetRemove
//
// Remove a function from the list of functions called when logging
void LogTargetRemove(LogTarget function)
{
    unsigned int i;

    for (i = 0; i < k_maxNumTargets; ++i)
    {
        if (l_logTargets[i] == function)
        {
            l_logTargets[i] = NULL;
        }
    }
}

//------------------------------------------------
// LogMessage
//
// The primary worker for this whole deal; gets the information, formats
// the message, and sends it out to the receivers.
void LogMessage(LogType type, const char* file, const unsigned int line, const char* message, ...)
{
    static char staticBuffer[k_bufferSize];
    char* tempBuffer = NULL;
    char* buffer;

    unsigned int i;

    va_list args;
    va_start(args, message);
    int numChars = vsnprintf(staticBuffer, k_bufferSize, message, args);
    va_end(args);

    if ((numChars + 1) < k_bufferSize) // make sure there's room for the newline added below
    {
        buffer = staticBuffer;
    }
    else
    {
        tempBuffer = malloc(numChars + 2); // leave space for a possible newline
        va_start(args, message);
        vsnprintf(tempBuffer, numChars + 1, message, args);
        va_end(args);
        buffer = tempBuffer;
    }

    //printf("numChars: %d\nmessage: %s\n", numChars, buffer);

    if (buffer[numChars-1] != '\n')
    {
        buffer[numChars] = '\n';
        buffer[numChars+1] = 0;
    }

    for (i = 0; i < k_maxNumTargets; ++i)
    {
        LogTarget lf = l_logTargets[i];

        if (lf != NULL)
        {
            lf(buffer, type, file, line);
        }
    }

    if (tempBuffer != NULL)
    {
        free(tempBuffer);
        tempBuffer = NULL;
    }
}

