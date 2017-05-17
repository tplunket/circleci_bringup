/**
 * Unit tests for Log.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#include "Log.h"

#define CATCH_CONFIG_MAIN
#include "Catch/Catch.hpp"

TEST_CASE( "Macros in if clauses" )
{
    /// Do the macros play nice with un-blocked if/else blocks?

    SECTION( "Basic if/else" )
    {
        if (true)
            Info("It works!");
        else
            Error("Did we get a compile error?");
    }

    SECTION( "Nested elses" )
    {
        if (true)
            if (true)
                Info("This is getting tricky.");
            else
                Error("yuck!");
        else
            Error("I really hope everything works.");
    }
}

TEST_CASE( "Log targets get called with data pointer." )
{
    bool aGotCalled = false;
    bool bGotCalled = false;

    auto targetFunction = [](char const*, LogType, char const*, unsigned int, void* data) -> void
    {
        *(bool*)data = true;
    };

    SECTION( "No targets added, nothing called. duh." )
    {
        Info("This won't actually output anywhere.");
        REQUIRE(!aGotCalled);
        REQUIRE(!bGotCalled);
    }

    SECTION( "Add a single target." )
    {
        LogTargetAdd(targetFunction, &aGotCalled);
        int number = 8;
        Info("The number is %d", number);
        Info("This is some test.");
        REQUIRE(aGotCalled);
        LogTargetRemove(targetFunction, &aGotCalled);
    }

    SECTION( "Add multiple targets." )
    {
        bool cGotCalled, dGotCalled, eGotCalled;
        LogTargetAdd(targetFunction, &aGotCalled);
        LogTargetAdd(targetFunction, &bGotCalled);
        LogTargetAdd(targetFunction, &cGotCalled);
        LogTargetAdd(targetFunction, &dGotCalled);
        LogTargetAdd(targetFunction, &eGotCalled);
        Warning("Call all targets.");
        REQUIRE(aGotCalled);
        REQUIRE(bGotCalled);
        REQUIRE(cGotCalled);
        REQUIRE(dGotCalled);
        REQUIRE(eGotCalled);
        // now remove them in awkward order
        LogTargetRemove(targetFunction, &cGotCalled);
        LogTargetRemove(targetFunction, &dGotCalled);
        LogTargetRemove(targetFunction, &bGotCalled);
        LogTargetRemove(targetFunction, &eGotCalled);
        LogTargetRemove(targetFunction, &aGotCalled);
    }
}

TEST_CASE( "Test the actual log messages returned" )
{
    static const int k_bufferSize = 5120;
    struct LogCapture
    {
        char buffer[k_bufferSize];
        const char* issuingFile;
        int issuingLine;
    };

    LogCapture data { { 0 }, nullptr, 0 };

    auto targetFunction = [](char const* message, LogType type, char const* file, unsigned int line,
                             void* data) -> void
    {
        LogCapture* d = (LogCapture*)data;
        strncpy(d->buffer, message, k_bufferSize);
        d->buffer[k_bufferSize-1] = 0;
        d->issuingFile = file;
        d->issuingLine = line;
    };

    LogTargetAdd(targetFunction, &data);

    SECTION( "Message gets trailing newline" )
    {
        Info("Is the newline stuck on the end for me?");
        REQUIRE_THAT("Is the newline stuck on the end for me?\n", Catch::Equals(data.buffer));
    }

    //----------------------------
    // But if there is already a newline at the end, don't double it.
    SECTION( "Theres no additional newline though" )
    {
        Info("I already have a newline.\n");
        REQUIRE_THAT("I already have a newline.\n", Catch::Equals(data.buffer));
    }

    //----------------------------
    // Do we get the file that issued the message through properly?
    SECTION( "Is the filename passed properly" )
    {
        Spew("spam I am.");
        REQUIRE_THAT(__FILE__, Catch::Equals(data.issuingFile));
    }

    //----------------------------
    // ...and do we get the proper line number, as well?
    SECTION( "Is the line number passed properly" )
    {
        Error("This is fun.");
        REQUIRE(__LINE__ - 1 == data.issuingLine); // -1 because the message is the line above this.
    }

    //----------------------------
    // If we pass a mega-sized message through, does it get passed properly?
    SECTION( "Mega message" )
    {
        char* bigBuffer = new char[5002];
        memset(bigBuffer, 'a', 5000);
        bigBuffer[5000] = 0;
        Info(bigBuffer);
        bigBuffer[5000] = '\n';
        bigBuffer[5001] = 0;
        REQUIRE_THAT(bigBuffer, Catch::Equals(data.buffer));
        delete[] bigBuffer;
    }

    LogTargetRemove(targetFunction, &data);
}
