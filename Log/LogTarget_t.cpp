/**
 * Unit tests for LogTarget helper class for maintaining log targets.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#include "LogTarget.hpp"

#include "Catch/Catch.hpp"

#include <string>

TEST_CASE( "LogTarget class" )
{
    struct MyLogTarget : public LogTarget
    {
        LogType type;
        std::string message;
        const char* issuingFile;
        int issuingLine;

        virtual void LogMessage(char const* m, LogType lt, char const* f, unsigned int l) override
        {
            type = lt;
            message = m;
            issuingFile = f;
            issuingLine = l;
        }
    };

    SECTION( "Local LogTarget gets data." )
    {
        MyLogTarget mlt;

        Info("Just a message.");

        REQUIRE(mlt.issuingLine == __LINE__ - 2);
        REQUIRE_THAT(mlt.issuingFile, Catch::Equals(__FILE__));
        REQUIRE(mlt.type == k_logInfo);
        REQUIRE(mlt.message == "Just a message.\n");
    }

    SECTION( "Local LogTarget gets data again." )
    {
        MyLogTarget mlt;

        Error("This time it's serious.");

        REQUIRE(mlt.issuingLine == __LINE__ - 2);
        REQUIRE_THAT(mlt.issuingFile, Catch::Equals(__FILE__));
        REQUIRE(mlt.type == k_logError);
        REQUIRE(mlt.message == "This time it's serious.\n");
    }
}
