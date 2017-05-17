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

#ifdef CL_USE_wchar_t
#define S(x) L ## x
#else
#define S(x) x
#endif

#define ARGS(...) \
    CL_StringType args[] = { __VA_ARGS__ }; \
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
        ARGS(S("app"), S("-q"), S("85"));
        int rv = CL_Parse(clp, num_args, args);
        REQUIRE(!rv);
        REQUIRE_THAT(tlt.pop(), Catch::StartsWith("Unknown option"));
        REQUIRE_THAT(tlt.pop(), Catch::Contains("'85' can't be handled."));
    }

    SECTION( "Counting options (aka booleans)" )
    {
        int value = 8;
        CL_AddCountingOption(clp, &value, S("v"));

        SECTION( "Counting option initialized to zero." )
        {
            REQUIRE(value == 0);
        }

        SECTION( "Sets truth if parameter is seen." )
        {
            ARGS(S("app"), S("-v"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(value == 1);
        }

        SECTION( "Actually counts appearances." )
        {
            ARGS(S("app"), S("-v"), S("-v"), S("-v"), S("-v"), S("-v"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(value == 5);
        }
    }

    SECTION( "Integer options" )
    {
        int number = 9;
        CL_AddIntegerOption(clp, &number, S("number"));

        SECTION( "Integer option is initialized to 0.")
        {
            REQUIRE(number == 0);
        }

        SECTION( "Parse the arg" )
        {
            ARGS(S("app"), S("-number"), S("42"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(number == 42);
        }

        SECTION( "Doesn't crash if there is no number; gives error message." )
        {
            ARGS(S("app"), S("-number"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(!rv);
            REQUIRE(tlt.hasMessages());
            REQUIRE_THAT(tlt.pop(), Catch::EndsWith("0 are available.\n"));
        }

        SECTION( "Non-integer gives error message." )
        {
            ARGS(S("app"), S("-number"), S("-jim"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(!rv);
            REQUIRE(tlt.hasMessages());
            REQUIRE_THAT(tlt.pop(), Catch::Equals("'-jim' is not a valid parameter to '-number'.\n"));
            REQUIRE_THAT(tlt.pop(), Catch::StartsWith("Unknown option") && Catch::Contains("-jim"));
        }

        SECTION( "Handle negative values." )
        {
            ARGS(S("app"), S("-number"), S("-867"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
            REQUIRE(number == -867);
        }
    }

    SECTION( "String options" )
    {
        CL_StringType q = (CL_CharType*)1;
        CL_AddStringOption(clp, &q, S("q"));

        SECTION( "Initialized to NULL" )
        {
            REQUIRE(q == NULL);
        }

        SECTION( "Grab the value" )
        {
            ARGS(S("application"), S("-q"), S("quickly"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(q, Catch::Equals("quickly"));
#endif
        }

        SECTION( "Missing string parameter" )
        {
            ARGS(S("app"), S("-q"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(!rv);
            REQUIRE_THAT(tlt.pop(), Catch::Contains("'-q'") && Catch::Contains("requires 1"));
        }
    }

    SECTION( "Normal Arguments" )
    {
        CL_StringType p = (CL_CharType*)1;
        CL_AddArgument(clp, &p);

        SECTION( "Initialized to NULL" )
        {
            REQUIRE((void*)p == NULL);
        }

        SECTION( "Single" )
        {
            ARGS(S("app"), S("newb"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(p, Catch::Equals("newb"));
#endif
        }

        SECTION( "Multiple" )
        {
            CL_StringType q;
            CL_AddArgument(clp, &q);
            ARGS(S("app"), S("friend"), S("enemy"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(p, Catch::Equals("friend"));
            REQUIRE_THAT(q, Catch::Equals("enemy"));
#endif
        }

        SECTION( "There aren't any overflow arguments" )
        {
            CL_StringType* overflow = CL_GetOverflowArguments(clp);
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
                CL_StringType* overflow = CL_GetOverflowArguments(clp);
                REQUIRE(overflow != NULL);
                REQUIRE((void*)overflow[0] == NULL);
            }

            SECTION( "And nothing else" )
            {
                SECTION( "works ok" )
                {
                    ARGS(S("app"));
                    int rv = CL_Parse(clp, num_args, args);
                    REQUIRE(rv);
                }

                SECTION( "so we parse the args..." )
                {
                    ARGS(S("app"), S("gee"), S("golly"));
                    int rv = CL_Parse(clp, num_args, args);
                    REQUIRE(rv);
                    CL_StringType* overflow = CL_GetOverflowArguments(clp);
#ifndef CL_USE_wchar_t
                    REQUIRE_THAT(overflow[0], Catch::Equals("gee"));
                    REQUIRE_THAT(overflow[1], Catch::Equals("golly"));
#endif
                    REQUIRE((void*)overflow[2] == NULL);
                }

                SECTION( "and unknown options still fail." )
                {
                    ARGS(S("qwertyuiop"), S("-aoeuidhtns"));
                    int rv = CL_Parse(clp, num_args, args);
                    REQUIRE(!rv);
                    REQUIRE_THAT(tlt.pop(), Catch::StartsWith("Unknown option"));
                }
            }
        }

        SECTION( "With other arguments" )
        {
            CL_StringType a, b, c;
            CL_AddArgument(clp, &a);
            CL_AddArgument(clp, &b);
            CL_AddArgument(clp, &c);

            ARGS(S("go.now"), S("a-arg"), S("b-arg"), S("c-arg"), S("o-arg-0"), S("o-arg-1"));
            int rv = CL_Parse(clp, num_args, args);
            REQUIRE(rv);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(a, Catch::Equals("a-arg"));
            REQUIRE_THAT(b, Catch::Equals("b-arg"));
            REQUIRE_THAT(c, Catch::Equals("c-arg"));
#endif
            CL_StringType* overflow = CL_GetOverflowArguments(clp);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(overflow[0], Catch::Equals("o-arg-0"));
            REQUIRE_THAT(overflow[1], Catch::Equals("o-arg-1"));
#endif
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
        ARGS( S("testapp") );
        bool rv = cl.Parse(num_args, args);
        REQUIRE(rv);
#ifndef CL_USE_wchar_t
        REQUIRE_THAT( cl.GetApplicationName(), Catch::Equals("testapp") );
#endif
    }

    SECTION( "Boolean options" )
    {
        CommandLine cl;
        int my_val = 0;

        SECTION( "Boolean reset to false." )
        {
            my_val = 1;
            cl.AddCountingOption(&my_val, S("testFlag"));
            REQUIRE( my_val == 0 );
        }

        SECTION( "Add the option." )
        {
            cl.AddCountingOption(&my_val, S("flag"));

            SECTION( "No option, stays false." )
            {
                ARGS( S("appname") );
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 0);
            }

            SECTION( "Arg appears and sets value." )
            {
                ARGS( S("appname"), S("-flag") );
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
            }
        }

        SECTION( "Two options" )
        {
            int my_other_val;
            cl.AddCountingOption(&my_val, S("a"));
            cl.AddCountingOption(&my_other_val, S("b"));

            SECTION("One arg")
            {
                ARGS(S("appName"), S("-a"));
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
                REQUIRE(my_other_val == 0);
            }

            SECTION("Other arg")
            {
                ARGS(S("appName"), S("-b"));
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 0);
                REQUIRE(my_other_val == 1);
            }

            SECTION("Both args")
            {
                ARGS(S("appName"), S("-a"), S("-b"));
                bool rv = cl.Parse(num_args, args);
                REQUIRE(rv);
                REQUIRE(my_val == 1);
                REQUIRE(my_other_val == 1);
            }

            SECTION("Both args, other way.")
            {
                ARGS(S("appName"), S("-b"), S("-a"));
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
        cl.AddIntegerOption(&value, S("v"));

        SECTION( "Parses correctly." )
        {
            ARGS(S("appName"), S("-v"), S("14"));
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
            REQUIRE(value == 14);
        }

        SECTION( "Extra options fail." )
        {
            ARGS(S("appName"), S("-v"), S("323"), S("399"));
            bool rv = cl.Parse(num_args, args);
            REQUIRE(!rv);
            REQUIRE(value == 323);
            REQUIRE_THAT(tlt.pop(), Catch::EndsWith("can't be handled.\n"));
        }
    }

    SECTION( "Normal arguments" )
    {
        CL_StringType a, b;
        cl.AddArgument(&a);
        cl.AddArgument(&b);

        ARGS(S("app"), S("alpha"), S("beta"));
        bool rv = cl.Parse(num_args, args);
        REQUIRE(rv);
#ifndef CL_USE_wchar_t
        REQUIRE_THAT(a, Catch::Equals("alpha"));
        REQUIRE_THAT(b, Catch::Equals("beta"));
#endif
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
            ARGS(S("application"), S("one"), S("two"), S("three"));
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.size() == 3);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(overflow[0], Catch::Equals("one"));
            REQUIRE_THAT(overflow[1], Catch::Equals("two"));
            REQUIRE_THAT(overflow[2], Catch::Equals("three"));
#endif
        }

        SECTION( "along with normal" )
        {
            CL_StringType a, b;
            cl.AddArgument(&a);
            cl.AddArgument(&b);
            ARGS(S("xe"), S("alpha"), S("beta"), S("gamma"), S("delta"));
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(a, Catch::Equals("alpha"));
            REQUIRE_THAT(b, Catch::Equals("beta"));
#endif
            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.size() == 2);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(overflow[0], Catch::Equals("gamma"));
            REQUIRE_THAT(overflow[1], Catch::Equals("delta"));
#endif
        }
    }

    SECTION( "A little bit of everything." )
    {
        CL_StringType arg0, arg1;
        int int0, int1;
        int count0, count1;
        CL_StringType s0, s1;

        cl.EnableOverflowArguments();

        cl.AddArgument(&arg0);
        cl.AddArgument(&arg1);
        cl.AddIntegerOption(&int0, S("0"));
        cl.AddIntegerOption(&int1, S("1"));
        cl.AddCountingOption(&count0, S("c0"));
        cl.AddCountingOption(&count1, S("c1"));
        cl.AddStringOption(&s0, S("s0"));
        cl.AddStringOption(&s1, S("s1"));

        SECTION( "Toodaloo" )
        {
            ARGS(S("application"), S("fred"), S("-1"), S("18"), S("-c1"), S("joebob"), S("-c0"), S("jaqueline"), S("-c1"),
                 S("-c1"), S("-s1"), S("tags"), S("gnargnar"), S("-c1"));
            bool rv = cl.Parse(num_args, args);
            REQUIRE(rv);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(arg0, Catch::Equals("fred"));
            REQUIRE_THAT(arg1, Catch::Equals("joebob"));
#endif
            REQUIRE(int0 == 0);
            REQUIRE(int1 == 18);
            REQUIRE(count0 == 1);
            REQUIRE(count1 == 4);
            REQUIRE((void*)s0 == nullptr);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(s1, Catch::Equals("tags"));
#endif

            auto overflow = cl.GetOverflowArguments();
            REQUIRE(overflow.size() == 2);
#ifndef CL_USE_wchar_t
            REQUIRE_THAT(overflow[0], Catch::Equals("jaqueline"));
            REQUIRE_THAT(overflow[1], Catch::Equals("gnargnar"));
#endif
        }
    }

    CHECK(!tlt.hasMessages());
}
