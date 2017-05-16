/**
 * A simple logging system.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#ifndef Log_h
#define Log_h

#if __cplusplus
extern "C" {
#endif

/**
 * The following macros encompass the bulk of the API.
 *
 * They're used like printf, and they hit all of the targets that have been configured.  Those
 * targets can then format the message however they like, although the "complicated bit," where the
 * message parameters are inserted, is done internally.  I.e. the message is formatted.
 *
 * <code>Error("Oh no, we had %d errors!", numErrors);</code>
 *
 * ...will result in the log target getting the string fully built, and with a newline stuck on the
 * end.
 */
#define Error(...)    LogMessage(k_logError,   __FILE__, __LINE__, __VA_ARGS__)
#define Warning(...)  LogMessage(k_logWarning, __FILE__, __LINE__, __VA_ARGS__)
#define Info(...)     LogMessage(k_logInfo,    __FILE__, __LINE__, __VA_ARGS__)
#define Spew(...)     LogMessage(k_logSpew,    __FILE__, __LINE__, __VA_ARGS__)

/**
 * This enumeration is used by the above macros to feed to the LogMessage function to indicate the
 * severity of the message. Different targets may choose to respond differently to the different
 * severity levels.
 */
enum LogType_
{
    k_logError,
    k_logWarning,
    k_logInfo,
    k_logSpew,
    k_numLogTypes,
    k_logForce32Bit = 0xffffffff
};
typedef enum LogType_ LogType;

/**
 * The API to add and remove Log Targets; each Log Target will be called for each log message
 * generated.
 */
typedef void(*LogTargetFn)(char const* m, LogType lt, char const* file, unsigned int line, void* d);
void LogTargetAdd(LogTargetFn function, void* data);
void LogTargetRemove(LogTargetFn function, void* data);

/// Log one message; normally this function won't be called directly
void LogMessage(LogType type, char const* file, const unsigned int line, char const* message, ...);

#if __cplusplus
} // extern "C"
#endif

#endif // Log_h

