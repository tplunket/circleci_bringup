#include "CommandLine.hpp"

#define CATCH_CONFIG_MAIN
#include "Catch/Catch.hpp"

#define ARGS(...) \
    char const* args[] = { __VA_ARGS__ }; \
    int const num_args = sizeof(args) / sizeof(args[0])

TEST_CASE( "CommandLine API, C Interface" )
{
    CommandLineProcessor clp = CL_Create();
    REQUIRE(clp != NULL);

    CL_Destroy(clp);
}

TEST_CASE( "Default construction" )
{
    CommandLine cl;

    SECTION( "Application name gets set." )
    {
        ARGS( "testapp" );
        cl.Parse(num_args, args);
        REQUIRE_THAT( cl.GetApplicationName(), Catch::Equals("testapp") );
    }
}

TEST_CASE( "Boolean arguments" )
{
    CommandLine cl;
    int my_val = 0;

    SECTION( "Boolean reset to false." )
    {
        my_val = 1;
        cl.AddCountingOption(&my_val, "testFlag");
        REQUIRE( my_val == 0 );
    }

    SECTION( "Add the argument." )
    {
        cl.AddCountingOption(&my_val, "flag");

        SECTION( "No arg, stays false." )
        {
            ARGS( "appname" );
            cl.Parse(num_args, args);
            REQUIRE(my_val == 0);
        }

        SECTION( "Arg appears and sets value." )
        {
            ARGS( "appname", "-flag" );
            cl.Parse(num_args, args);
            REQUIRE(my_val == 1);
        }
    }

    SECTION( "Two arguments" )
    {
        int my_other_val;
        cl.AddCountingOption(&my_val, "a");
        cl.AddCountingOption(&my_other_val, "b");

        SECTION("One arg")
        {
            ARGS("appName", "-a");
            cl.Parse(num_args, args);
            REQUIRE(my_val == 1);
            REQUIRE(my_other_val == 0);
        }

        SECTION("Other arg")
        {
            ARGS("appName", "-b");
            cl.Parse(num_args, args);
            REQUIRE(my_val == 0);
            REQUIRE(my_other_val == 1);
        }

        SECTION("Both args")
        {
            ARGS("appName", "-a", "-b");
            cl.Parse(num_args, args);
            REQUIRE(my_val == 1);
            REQUIRE(my_other_val == 1);
        }

        SECTION("Both args, other way.")
        {
            ARGS("appName", "-b", "-a");
            cl.Parse(num_args, args);
            REQUIRE(my_val == 1);
            REQUIRE(my_other_val == 1);
        }
    }
}
