/**
 * Unit tests for LogTarget helper class for maintaining log targets.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 */

#include "LogTarget.hpp"

#include "Catch/Catch.hpp"

#include <string>

TEST_CASE( "LogTarget class" )
{
    struct MyLogTarget : public LogTarget
    {
        std::string message;
        const char* issuingFile;
        int issuingLine;

        virtual void LogMessage(char const* m, LogType lt, char const* f, unsigned int l) override
        {
            message = m;
            issuingFile = f;
            issuingLine = l;
        }
    };

    SECTION( "Local LogTarget gets data." )
    {
        MyLogTarget mlt;

        Info("Just a message.");

        REQUIRE(mlt.message == "Just a message.\n");
        REQUIRE_THAT(mlt.issuingFile, Catch::Equals(__FILE__));
        REQUIRE(mlt.issuingLine == __LINE__ - 4);
    }
}
