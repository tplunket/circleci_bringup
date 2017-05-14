/**
 * A simple logging system.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010 Tom Plunket, all rights reserved
 */

#include "Log.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define k_bufferSize 256
#define k_maxNumTargets 4

struct LogTargetData
{
    LogTarget function;
    void* data;
};
static struct LogTargetData l_logTargets[k_maxNumTargets] = { 0 };

/**
 * Add a function to the list of functions called with logging output.
 */
void LogTargetAdd(LogTarget function, void* data)
{
    unsigned int i;

    for (i = 0; i < k_maxNumTargets; ++i)
    {
        struct LogTargetData* ltd = &l_logTargets[i];
        if (ltd->function == NULL)
        {
            ltd->function = function;
            ltd->data = data;
            break;
        }
    }
}

/**
 * Remove a function from the list of functions called when logging
 */
void LogTargetRemove(LogTarget function, void* data)
{
    unsigned int i;

    for (i = 0; i < k_maxNumTargets; ++i)
    {
        struct LogTargetData* ltd = &l_logTargets[i];
        if ((ltd->function == function) && (ltd->data == data))
        {
            ltd->function = NULL;
            ltd->data = NULL;
        }
    }
}

/**
 * The primary worker for this whole deal; gets the information, formats the message, and sends it
 * out to the receivers.
 */
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
        struct LogTargetData const* ltd = &l_logTargets[i];

        if (ltd->function != NULL)
        {
            ltd->function(buffer, type, file, line, ltd->data);
        }
    }

    if (tempBuffer != NULL)
    {
        free(tempBuffer);
        tempBuffer = NULL;
    }
}

