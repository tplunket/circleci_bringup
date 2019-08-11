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

    SECTION( "permuting it all" )
    {
        if ((testValue != 0.0f) && (testValue < 1e10f))
        {
            float testValue2 = GENERATE(1.0f, 2.0f, 0.5f, 1000.0f);
            if (testValue < 0) testValue = -testValue;

            SprungValue<float> v(testValue, testValue2);
            v.SetGoal(0);

            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == 0);

            float lastVal = v.GetValue();
            v.Tick(0);
            CHECK(v.GetValue() == lastVal);
            CHECK(v.GetVelocity() == 0);

            lastVal = v.GetValue();
            v.Tick(1.0f);
            CHECK(v.GetValue() < lastVal);
            CHECK(v.GetValue() >= 0);
            CHECK(v.GetVelocity() <= 0);

            lastVal = v.GetValue();
            v.Tick(1.0f);
            CHECK(v.GetValue() <= lastVal);
            CHECK(v.GetValue() >= 0);
            CHECK(v.GetVelocity() <= 0);

            lastVal = v.GetValue();
            v.Tick(1.0f);
            CHECK(v.GetValue() <= lastVal);
            CHECK(v.GetValue() >= 0);
            CHECK(v.GetVelocity() <= 0);

            lastVal = v.GetValue();
            v.Tick(1.0f);
            CHECK(v.GetValue() <= lastVal);
            CHECK(v.GetValue() >= 0);
            CHECK(v.GetVelocity() <= 0);
        }
    }
}

TEST_CASE( "huge values" )
{
    const float k_largeValue = 90000000.0f;

    SECTION( "big moves" )
    {
        SprungValue<float> v(k_largeValue, 1.0f);
        v.SetGoal(0);

        CHECK(v.GetValue() == k_largeValue);
        CHECK(v.GetVelocity() == 0);

        float lastVal = v.GetValue();
        v.Tick(0);
        CHECK(v.GetValue() == lastVal);
        CHECK(v.GetVelocity() == 0);

        v.Tick(1.0f);
        CHECK(v.GetValue() < lastVal);
        CHECK(v.GetValue() > 0);
        CHECK(v.GetVelocity() < 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() < lastVal);
        CHECK(v.GetValue() > 0);
        CHECK(v.GetVelocity() < 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() < lastVal);
        CHECK(v.GetValue() > 0);
        CHECK(v.GetVelocity() < 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() < lastVal);
        CHECK(v.GetValue() > 0);
        CHECK(v.GetVelocity() < 0);
    }

    SECTION( "heavy tension" )
    {
        SprungValue<float> v(10.0f, k_largeValue);
        v.SetGoal(0);

        CHECK(v.GetValue() == 10.0f);
        CHECK(v.GetVelocity() == 0);

        float lastVal = v.GetValue();
        v.Tick(0);
        CHECK(v.GetValue() == lastVal);
        CHECK(v.GetVelocity() == 0);

        v.Tick(1.0f);
        CHECK(v.GetValue() < lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);
    }

    SECTION( "Both!" )
    {
        SprungValue<float> v(k_largeValue, k_largeValue);
        v.SetGoal(0);

        CHECK(v.GetValue() == k_largeValue);
        CHECK(v.GetVelocity() == 0);

        float lastVal = v.GetValue();
        v.Tick(0);
        CHECK(v.GetValue() == lastVal);
        CHECK(v.GetVelocity() == 0);

        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);

        lastVal = v.GetValue();
        v.Tick(1.0f);
        CHECK(v.GetValue() <= lastVal);
        CHECK(v.GetValue() >= 0);
        CHECK(v.GetVelocity() <= 0);
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

bool operator!=(const Simple2dVector& lhs, const Simple2dVector& rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
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

float dotProduct(const Simple2dVector& rhs, const Simple2dVector lhs)
{
    return (rhs.x * lhs.x) + (rhs.y * lhs.y);
}

float lengthSquared(const Simple2dVector& v)
{
    return dotProduct(v, v);
}

TEST_CASE( "A simple vector type." )
{
    auto testValue = GENERATE(Simple2dVector{0,0},
                              Simple2dVector{1.0,1.0},
                              Simple2dVector{99.0,-38.5});

    SECTION( "with no spring tension." )
    {
        SprungValue<Simple2dVector> v(testValue);
        v.SetGoal(Simple2dVector::zero);
        CHECK(v.GetValue() == testValue);
        CHECK(v.GetVelocity() == Simple2dVector::zero);

        v.Tick(0);
        CHECK(v.GetValue() == testValue);
        CHECK(v.GetVelocity() == Simple2dVector::zero);

        v.Tick(1);
        CHECK(v.GetValue() == testValue);
        CHECK(v.GetVelocity() == Simple2dVector::zero);
    }

    SECTION( "with some spring tension." )
    {
        if (testValue != Simple2dVector::zero)
        {
            SprungValue<Simple2dVector> v(testValue, 5.0f);
            v.SetGoal(Simple2dVector::zero);
            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == Simple2dVector::zero);

            float lastLen = lengthSquared(v.GetValue());
            v.Tick(0);
            CHECK(v.GetValue() == testValue);
            CHECK(v.GetVelocity() == Simple2dVector::zero);

            v.Tick(1);
            CHECK(lengthSquared(v.GetValue()) <= lastLen);
            // velocity and value are pointing in opposite directions.
            CHECK(dotProduct(v.GetValue(), v.GetVelocity()) < 0);

            lastLen = lengthSquared(v.GetValue());
            v.Tick(1);
            CHECK(lengthSquared(v.GetValue()) <= lastLen);
            CHECK(dotProduct(v.GetValue(), v.GetVelocity()) < 0);
        }
    }
}

