#ifndef LogTarget_hpp
#define LogTarget_hpp

#include "Log.h"

class LogTarget
{
public:
    LogTarget() { LogTargetAdd(&Trampoline, this); }
    ~LogTarget() { LogTargetRemove(&Trampoline, this); }

    virtual void LogMessage(char const* message, LogType lt,
                            char const* file, unsigned int line) = 0;

private:
    static void Trampoline(char const* m, LogType lt, char const* file, unsigned int line, void* d)
    {
        ((LogTarget*)d)->LogMessage(m, lt, file, line);
    }
};

#endif // ndef LogTarget_hpp
