/**
 * Tests for CommandLine.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#include "CommandLine.hpp"

#include "Log/LogTarget.hpp"

#define CATCH_CONFIG_MAIN
#include "Catch/Catch.hpp"

#include <string>
#include <queue>

#define ARGS(...) \
    char const* args[] = { __VA_ARGS__ }; \
    int const num_args = sizeof(args) / sizeof(args[0])

class TestLogTarget : public LogTarget
{
public:
    ~TestLogTarget()
    {
        while (hasMessages())
            std::cout << pop() << std::endl;
    }

    std::string pop()
    {
        std::string m;
        if (!messages.empty())
        {
            m = messages.front();
            messages.pop();
        }
        return m;
    }

    bool hasMessages() const { return !messages.empty(); }

private:
    virtual void LogMessage(char const* m, LogType lt, char const* f, unsigned int l) override
    {
        messages.emplace(m);
    }

    std::queue<std::string> messages;
};

TEST_CASE( "CommandLine API, C Interface" )
{
    TestLogTarget tlt;
    CommandLineProcessor clp = CL_Create();
    REQUIRE(clp != NULL);

    SECTION( "Fails when no parameters are expected but parameters are received." )
    {
        ARGS("app", "-q", "85");
        int rv = CL_Parse(clp, num_args, args);
        REQUIRE(!rv);
        REQUIRE_THAT(tlt.pop(), Catch::StartsWith("Unknown option"));
        REQUIRE_THAT(tlt.pop(), Catch::Contains("'85' can't be handled."));
    }

    SECTION( "Counting options (aka booleans)" )
    {
        int value = 8;
        CL_AddCountingOption(clp, &value, "v");

        SECTION( "Counting option initialized to zero." )
        {
            REQUIRE(value == 0);
        }

        SECTION( "Sets truth if parameter is seen." )
        {
            ARGS("app", "-v");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(value == 1);
        }

        SECTION( "Actually counts appearances." )
        {
            ARGS("app", "-v", "-v", "-v", "-v", "-v");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(value == 5);
        }
    }

    SECTION( "Integer options" )
    {
        int number = 9;
        CL_AddIntegerOption(clp, &number, "number");

        SECTION( "Integer option is initialized to 0.")
        {
            REQUIRE(number == 0);
        }

        SECTION( "Parse the arg" )
        {
            ARGS("app", "-number", "42");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(number == 42);
        }

        SECTION( "Doesn't crash if there is no number; gives error message." )
        {
            ARGS("app", "-number");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(!rv);
            REQUIRE(tlt.hasMessages());
            REQUIRE_THAT(tlt.pop(), Catch::EndsWith("0 are available.\n"));
        }

        SECTION( "Non-integer gives error message." )
        {
            ARGS("app", "-number", "-jim");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(!rv);
            REQUIRE(tlt.hasMessages());
            REQUIRE_THAT(tlt.pop(), Catch::Contains("not a valid parameter"));
            REQUIRE_THAT(tlt.pop(), Catch::StartsWith("Unknown option") && Catch::Contains("-jim"));
        }

        SECTION( "Handle negative values." )
        {
            ARGS("app", "-number", "-867");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(number == -867);
        }
    }

    CL_Destroy(clp);
    CHECK(!tlt.hasMessages());
}

TEST_CASE( "C++ API" )
{
    TestLogTarget tlt;
    CommandLine cl;

    SECTION( "Application name gets set." )
    {
        ARGS( "testapp" );
        bool rv = cl.Parse(num_args, args);
        REQUIRE(rv);
        REQUIRE_THAT( cl.GetApplicationName(), Catch::Equals("testapp") );
    }

    SECTION( "Boolean options" )
    {
        CommandLine cl;
        int my_val = 0;

        SECTION( "Boolean reset to false." )
        {
            my_val = 1;
            cl.AddCountingOption(&my_val, "testFlag");
            REQUIRE( my_val == 0 );
        }

        SECTION( "Add the option." )
        {
            cl.AddCountingOption(&my_val, "flag");

            SECTION( "No option, stays false." )
            {
                ARGS( "appname" );
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 0);
            }

            SECTION( "Arg appears and sets value." )
            {
                ARGS( "appname", "-flag" );
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
            }
        }

        SECTION( "Two options" )
        {
            int my_other_val;
            cl.AddCountingOption(&my_val, "a");
            cl.AddCountingOption(&my_other_val, "b");

            SECTION("One arg")
            {
                ARGS("appName", "-a");
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
                REQUIRE(my_other_val == 0);
            }

            SECTION("Other arg")
            {
                ARGS("appName", "-b");
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 0);
                REQUIRE(my_other_val == 1);
            }

            SECTION("Both args")
            {
                ARGS("appName", "-a", "-b");
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
                REQUIRE(my_other_val == 1);
            }

            SECTION("Both args, other way.")
            {
                ARGS("appName", "-b", "-a");
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
                REQUIRE(my_other_val == 1);
            }
        }

        SECTION( "Integer options" )
        {
            int value;
            cl.AddIntegerOption(&value, "v");

            SECTION( "Parses correctly." )
            {
                ARGS("appName", "-v", "14");
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(value == 14);
            }

            SECTION( "Extra options fail." )
            {
                ARGS("appName", "-v", "323", "399");
                bool rv = cl.Parse(num_args, args);
                REQUIRE(!rv);
                REQUIRE(value == 323);
                REQUIRE_THAT(tlt.pop(), Catch::EndsWith("can't be handled.\n"));
            }
        }
    }

    CHECK(!tlt.hasMessages());
}
