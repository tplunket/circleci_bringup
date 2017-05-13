#include "CommandLine.h"

#define CATCH_CONFIG_MAIN
#include "Catch/Catch.hpp"

#define ARGS(...) \
    char const* args[] = { __VA_ARGS__ }; \
    int const num_args = sizeof(args) / sizeof(args[0])

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
    bool my_val = false;

    SECTION( "Boolean reset to false." )
    {
        my_val = true;
        cl.AddOption(&my_val, "testFlag");
        REQUIRE( my_val == false );
    }

    SECTION( "Add the argument." )
    {
        cl.AddOption(&my_val, "flag");

        SECTION( "No arg, stays false." )
        {
            ARGS( "appname" );
            cl.Parse(num_args, args);
            REQUIRE(my_val == false);
        }

        SECTION( "Arg appears and sets value." )
        {
            ARGS( "appname", "-flag" );
            cl.Parse(num_args, args);
            REQUIRE(my_val == true);
        }
    }

    SECTION( "Two arguments" )
    {
        bool my_other_val;
        cl.AddOption(&my_val, "a");
        cl.AddOption(&my_other_val, "b");

        SECTION("One arg")
        {
            ARGS("appName", "-a");
            cl.Parse(num_args, args);
            REQUIRE(my_val == true);
            REQUIRE(my_other_val == false);
        }

        SECTION("Other arg")
        {
            ARGS("appName", "-b");
            cl.Parse(num_args, args);
            REQUIRE(my_val == false);
            REQUIRE(my_other_val == true);
        }

        SECTION("Both args")
        {
            ARGS("appName", "-a", "-b");
            cl.Parse(num_args, args);
            REQUIRE(my_val == true);
            REQUIRE(my_other_val == true);
        }

        SECTION("Both args, other way.")
        {
            ARGS("appName", "-b", "-a");
            cl.Parse(num_args, args);
            REQUIRE(my_val == true);
            REQUIRE(my_other_val == true);
        }
    }
}
