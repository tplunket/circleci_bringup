/*- Log.h --------------------------------------------------------------------*\
|
|   Contents:   A simple logging system.
|
|   Author:     Tom Plunket <tom@mightysprite.com>
|
|   Copyright:  (c) 2010 Tom Plunket, all rights reserved
|
\*----------------------------------------------------------------------------*/

#ifndef Log_h
#define Log_h

#if __cplusplus
extern "C" {
#endif

//----------------------------------------------------------
// The following macros encompass the bulk of the API.
//
// They're used like printf, and they hit all of the targets that have
// been configured.  Those targets can then format the message however
// they like, although the "complicated bit," where the message parameters
// are inserted, are done internally.  I.e. the message is formatted.
//
//    Error("Oh no, we had %d errors!", numErrors);
//
// will result in the log target getting the string fully built, and with
// a newline stuck on the end.
#define Error(...)    LogMessage(k_logError,   __FILE__, __LINE__, __VA_ARGS__)
#define Warning(...)  LogMessage(k_logWarning, __FILE__, __LINE__, __VA_ARGS__)
#define Info(...)     LogMessage(k_logInfo,    __FILE__, __LINE__, __VA_ARGS__)
#define Spew(...)     LogMessage(k_logSpew,    __FILE__, __LINE__, __VA_ARGS__)

//----------------------------------------------------------
// LogType
//
// This enumeration is used by the above macros to feed to the
// LogMessage function to indicate the severity of the message.
// Different targets may choose to respond differently to
// the different severity levels.
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

//----------------------------------------------------------
// Log Targets
//
// The API to add and remove Log Targets; each Log Target
// will be called for each log message generated.
typedef void(*LogTarget)(const char* m, LogType lt, const char* file, unsigned int line);
void LogTargetAdd(LogTarget function);
void LogTargetRemove(LogTarget function);

//----------------------------------------------------------
// Log one message; normally this function won't be called directly
void LogMessage(LogType type, const char* file, const unsigned int line, const char* message, ...);

#if __cplusplus
} // extern "C"
#endif

#endif // Log_h

