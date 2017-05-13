/*- Log_t.cpp ----------------------------------------------------------------*\
|
|   Contents:   Unit tests for Log.
|
|   Author:     Tom Plunket <tom@mightysprite.com>
|
|   Copyright:  (c) 2010 Tom Plunket, all rights reserved
|
\*----------------------------------------------------------------------------*/

#include "Log.h"

#include <UnitTest++.h>

//------------------------------------------------
// Do the macros play nice with un-blocked if/else blocks?
TEST(MacrosInIfClauses)
{
    if (true)
        Info("It works!");
    else
        Error("Did we get a compile error?");
}

TEST(MacrosInNestedIfs)
{
    if (true)
        if (true)
            Info("This is getting tricky.");
        else
            Error("yuck!");
    else
        Error("I really hope everything works.");
}

//----------------------------------------------------------
// ManualLogTarget
//
// This fixture has log targets available, but doesn't automatically
// insert them into the logging system.  The targets themselves
// simply track whether or not they've been called.
class ManualLogTarget
{
public:
    ManualLogTarget() { gotCalled = false; }

    static void TargetFunction(const char* message,
                               LogType type, const char* file, unsigned int line)
    {
        gotCalled = true;
    }

    static void AnotherTargetFunction(const char* message,
                                      LogType type, const char* file, unsigned int line)
    {
        otherGotCalled = true;
    }

    static bool gotCalled, otherGotCalled;
};

bool ManualLogTarget::gotCalled = false;
bool ManualLogTarget::otherGotCalled = false;

//----------------------------
// Can a single target be added, and is it called?
TEST_FIXTURE(ManualLogTarget, AddRemoveTarget)
{
    LogTargetAdd(&TargetFunction);
    int number = 8;
    Info("The number is %d", number);

    Info("This is some test.");
    CHECK(gotCalled);
    LogTargetRemove(&TargetFunction);
}

//----------------------------
// If there's no target inserted, the call isn't made, right?
TEST_FIXTURE(ManualLogTarget, NoTarget)
{
    Info("This won't actually output anywhere.");
    CHECK(!gotCalled);
}

//----------------------------
// If we add two targets, do they both get called?
TEST_FIXTURE(ManualLogTarget, TwoTargets)
{
    LogTargetAdd(&TargetFunction);
    LogTargetAdd(&AnotherTargetFunction);
    Warning("Call both functions.");
    CHECK(gotCalled);
    CHECK(otherGotCalled);
    LogTargetRemove(&TargetFunction);
    LogTargetRemove(&AnotherTargetFunction);
}

//----------------------------------------------------------
// AutoLogTarget
//
// Fixture adds one log target to the logging system, and that target
// copies the message.
class AutoLogTarget
{
public:
    AutoLogTarget() { LogTargetAdd(&TargetFunction); }
    ~AutoLogTarget() { LogTargetRemove(&TargetFunction); }

    static void TargetFunction(const char* message,
                               LogType type, const char* file, unsigned int line)
    {
        strncpy(buffer, message, k_bufferSize);
        buffer[k_bufferSize-1] = 0;
        issuingFile = file;
        issuingLine = line;
    }
    static const int k_bufferSize = 5120;
    static char buffer[k_bufferSize];
    static const char* issuingFile;
    static int issuingLine;
};

char AutoLogTarget::buffer[k_bufferSize];
const char* AutoLogTarget::issuingFile;
int AutoLogTarget::issuingLine;

//----------------------------
// Usually we want the system to automatically stuff newlines onto the end of the message.
TEST_FIXTURE(AutoLogTarget, MessageGetsTrailingNewline)
{
    Info("Is the newline stuck on the end for me?");
    CHECK_EQUAL("Is the newline stuck on the end for me?\n", buffer);
}

//----------------------------
// But if there is already a newline at the end, don't double it.
TEST_FIXTURE(AutoLogTarget, TheresNoAdditionalNewlineThough)
{
    Info("I already have a newline.\n");
    CHECK_EQUAL("I already have a newline.\n", buffer);
}

//----------------------------
// Do we get the file that issued the message through properly?
TEST_FIXTURE(AutoLogTarget, IsTheFilenamePassedProperly)
{
    Spew("spam I am.");
    CHECK_EQUAL(__FILE__, issuingFile);
}

//----------------------------
// ...and do we get the proper line number, as well?
TEST_FIXTURE(AutoLogTarget, IsTheLineNumberPassedProperly)
{
    Error("This is fun.");
    CHECK_EQUAL(__LINE__ - 1, issuingLine);
}

//----------------------------
// If we pass a mega-sized message through, does it get passed properly?
TEST_FIXTURE(AutoLogTarget, MegaMessage)
{
    char* bigBuffer = new char[5002];
    memset(bigBuffer, 'a', 5000);
    bigBuffer[5000] = 0;
    Info(bigBuffer);
    bigBuffer[5000] = '\n';
    bigBuffer[5001] = 0;
    CHECK_EQUAL(bigBuffer, buffer);
    delete[] bigBuffer;
}

//----------------------------------------------------------
// Run the tests...
int main()
{
    return UnitTest::RunAllTests();
}

