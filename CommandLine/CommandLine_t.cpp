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
            std::cout << pop(); // they have newlines already
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

    SECTION( "Normal Arguments" )
    {
        char const* p = (char*)1;
        CL_AddArgument(clp, &p);

        SECTION( "Initialized to NULL" )
        {
            REQUIRE((void*)p == NULL);
        }

        SECTION( "Single" )
        {
            ARGS("app", "newb");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE_THAT(p, Catch::Equals("newb"));
        }

        SECTION( "Multiple" )
        {
            const char* q;
            CL_AddArgument(clp, &q);
            ARGS("app", "friend", "enemy");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE_THAT(p, Catch::Equals("friend"));
            REQUIRE_THAT(q, Catch::Equals("enemy"));
        }

        SECTION( "There aren't any overflow arguments" )
        {
            const char** overflow = CL_GetOverflowArguments(clp);
            REQUIRE(overflow == NULL);
        }
    }

    SECTION( "Overflow arguments" )
    {
        CL_EnableOverflowArguments(clp);

        SECTION( "No overflow args" )
        {
            SECTION( "Returns nothing if the command line hasn't been parsed yet" )
            {
                const char** overflow = CL_GetOverflowArguments(clp);
                REQUIRE(overflow != NULL);
                REQUIRE((void*)overflow[0] == NULL);
            }

            SECTION( "And nothing else" )
            {
                SECTION( "works ok" )
                {
                    ARGS("app");
                    int rv = CL_Parse(clp, num_args, args);
                    REQUIRE(rv);
                }

                SECTION( "so we parse the args..." )
                {
                    ARGS("app", "gee", "golly");
                    int rv = CL_Parse(clp, num_args, args);
                    REQUIRE(rv);
                    const char** overflow = CL_GetOverflowArguments(clp);
                    REQUIRE_THAT(overflow[0], Catch::Equals("gee"));
                    REQUIRE_THAT(overflow[1], Catch::Equals("golly"));
                    REQUIRE((void*)overflow[2] == NULL);
                }

                SECTION( "and unknown options still fail." )
                {
                    ARGS("qwertyuiop", "-aoeuidhtns");
                    int rv = CL_Parse(clp, num_args, args);
                    REQUIRE(!rv);
                    REQUIRE_THAT(tlt.pop(), Catch::StartsWith("Unknown option"));
                }
            }
        }

        SECTION( "With other arguments" )
        {
            char const* a, *b, *c;
            CL_AddArgument(clp, &a);
            CL_AddArgument(clp, &b);
            CL_AddArgument(clp, &c);

            ARGS("go.now", "a-arg", "b-arg", "c-arg", "o-arg-0", "o-arg-1");
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE_THAT(a, Catch::Equals("a-arg"));
            REQUIRE_THAT(a, Catch::Equals("a-arg"));
            REQUIRE_THAT(a, Catch::Equals("a-arg"));
            const char** overflow = CL_GetOverflowArguments(clp);
            REQUIRE_THAT(overflow[0], Catch::Equals("o-arg-0"));
            REQUIRE_THAT(overflow[1], Catch::Equals("o-arg-1"));
            REQUIRE((void*)overflow[2] == NULL);
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

    SECTION( "Normal arguments" )
    {
        char const* a, *b;
        cl.AddArgument(&a);
        cl.AddArgument(&b);

        ARGS("app", "alpha", "beta");
        bool rv = cl.Parse(num_args, args);
        REQUIRE(rv);
        REQUIRE_THAT(a, Catch::Equals("alpha"));
        REQUIRE_THAT(b, Catch::Equals("beta"));
    }

    SECTION( "Overflow arguments" )
    {
        cl.EnableOverflowArguments();

        SECTION( "Nothing parsed, empty vector" )
        {
            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.empty());
        }

        SECTION( "Parse a few arguments" )
        {
            ARGS("application", "one", "two", "three");
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.size() == 3);
            REQUIRE_THAT(overflow[0], Catch::Equals("one"));
            REQUIRE_THAT(overflow[1], Catch::Equals("two"));
            REQUIRE_THAT(overflow[2], Catch::Equals("three"));
        }

        SECTION( "along with normal" )
        {
            char const* a, *b;
            cl.AddArgument(&a);
            cl.AddArgument(&b);
            ARGS("xe", "alpha", "beta", "gamma", "delta");
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
            REQUIRE_THAT(a, Catch::Equals("alpha"));
            REQUIRE_THAT(b, Catch::Equals("beta"));
            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.size() == 2);
            REQUIRE_THAT(overflow[0], Catch::Equals("gamma"));
            REQUIRE_THAT(overflow[1], Catch::Equals("delta"));
        }
    }

    SECTION( "A little bit of everything." )
    {
        char const* arg0, *arg1;
        int int0, int1;
        int count0, count1;

        cl.EnableOverflowArguments();

        cl.AddArgument(&arg0);
        cl.AddArgument(&arg1);
        cl.AddIntegerOption(&int0, "0");
        cl.AddIntegerOption(&int1, "1");
        cl.AddCountingOption(&count0, "c0");
        cl.AddCountingOption(&count1, "c1");

        SECTION( "Toodaloo" )
        {
            ARGS("application", "fred", "-1", "18", "-c1", "joebob", "-c0", "jaqueline", "-c1",
                 "-c1", /*"-s1", "tags",*/ "gnargnar", "-c1");
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
            REQUIRE_THAT(arg0, Catch::Equals("fred"));
            REQUIRE_THAT(arg1, Catch::Equals("joebob"));
            REQUIRE(int0 == 0);
            REQUIRE(int1 == 18);
            REQUIRE(count0 == 1);
            REQUIRE(count1 == 4);

            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.size() == 2);
            REQUIRE_THAT(overflow[0], Catch::Equals("jaqueline"));
            REQUIRE_THAT(overflow[1], Catch::Equals("gnargnar"));
        }
    }

    CHECK(!tlt.hasMessages());
}
