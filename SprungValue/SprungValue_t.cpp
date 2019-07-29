/**
 * Unit tests for SprungValue.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2019 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#include "SprungValue.h"

#define CATCH_CONFIG_MAIN
#include "Catch/Catch.hpp"

TEST_CASE( "A single floating point value." )
{
    float testValue = GENERATE(0.0f, 1.0f, 2.0f, 0.5f, 1000.0f, 1e20, -1.0f, -47.25f);

    SECTION( "...with no (default) spring tension." )
    {
        SprungValue<float> v(testValue);
        CHECK(v.GetValue() == testValue);
        CHECK(v.GetVelocity() == 0);

        v.Tick(0);
        CHECK(v.GetValue() == testValue);
        CHECK(v.GetVelocity() == 0);

        v.Tick(1);
        CHECK(v.GetValue() == testValue);
        CHECK(v.GetVelocity() == 0);

        SECTION( "Doesn't move when goal changes." )
        {
            v.SetGoal(-80.0f);
            v.Tick(1);
            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == 0);
        }
    }

    SECTION( "with spring tension" )
    {
        SECTION( "Doesn't move when goal and starting value are the same." )
        {
            SprungValue<float> v(testValue, 10.0f);
            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == 0);

            v.Tick(0);
            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == 0);

            v.Tick(1);
            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == 0);
        }
    }
}

TEST_CASE( "A single floating point value being pulled." )
{

}

struct Simple2dVector
{
    float x, y;

    static const Simple2dVector zero;
};

const Simple2dVector Simple2dVector::zero { 0, 0 };

bool operator==(const Simple2dVector& lhs, const Simple2dVector& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

Simple2dVector operator+(const Simple2dVector& lhs, const Simple2dVector & rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

Simple2dVector operator-(const Simple2dVector& lhs, const Simple2dVector & rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}

Simple2dVector operator*(const Simple2dVector& lhs, float scale)
{
    return { lhs.x * scale, lhs.y * scale };
}

TEST_CASE( "A simple vector type with no spring tension." )
{
    auto testValue = GENERATE(Simple2dVector{0,0},
                              Simple2dVector{1.0,1.0},
                              Simple2dVector{99.0,-38.5});

    SprungValue<Simple2dVector> v(testValue);
    CHECK(v.GetValue() == testValue);
    CHECK(v.GetVelocity() == Simple2dVector::zero);

    v.Tick(0);
    CHECK(v.GetValue() == testValue);
    CHECK(v.GetVelocity() == Simple2dVector::zero);

    v.Tick(1);
    CHECK(v.GetValue() == testValue);
    CHECK(v.GetVelocity() == Simple2dVector::zero);
}

