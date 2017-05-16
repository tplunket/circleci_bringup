/**
 * A simple logging system.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#include "Log.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define k_bufferSize 256

struct LogTargetData
{
    LogTargetFn function;
    void* data;
    struct LogTargetData* next;
};
static struct LogTargetData* s_logTargets = NULL;

/**
 * Add a function to the list of functions called with logging output.
 */
void LogTargetAdd(LogTargetFn function, void* data)
{
    struct LogTargetData** p = &s_logTargets;
    while (*p != NULL)
    {
        struct LogTargetData* ltd = *p;
        if ((ltd->function == function) && (ltd->data == data))
        {
            ltd->function("This log target has already been added.",
                          k_logWarning, __FILE__, __LINE__, data);
        }

        p = &ltd->next;
    }

    {
        struct LogTargetData* ltd = (struct LogTargetData*)malloc(sizeof(struct LogTargetData));
        ltd->function = function;
        ltd->data = data;
        ltd->next = NULL;
        *p = ltd;
    }
}

/**
 * Remove a function from the list of functions called when logging
 */
void LogTargetRemove(LogTargetFn function, void* data)
{
    struct LogTargetData** p = &s_logTargets;
    while (*p != NULL)
    {
        struct LogTargetData* ltd = *p;
        if ((ltd->function == function) && (ltd->data == data))
        {
            *p = ltd->next;
            free(ltd);
        }

        if (*p != NULL)
            p = &(*p)->next;
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

    struct LogTargetData* ltd = s_logTargets;
    while (ltd != NULL)
    {
        if (ltd->function != NULL)
        {
            ltd->function(buffer, type, file, line, ltd->data);
        }

        ltd = ltd->next;
    }

    if (tempBuffer != NULL)
    {
        free(tempBuffer);
        tempBuffer = NULL;
    }
}

