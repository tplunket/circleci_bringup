/**
 * A mechanism to control a value with a virtual critically damped spring.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2019 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#ifndef SprungValue_h
#define SprungValue_h

#include <utility>

template <typename T>
class SprungValue
{
public:
    SprungValue() {}
    explicit SprungValue(T startingValue) : value(startingValue) {}
    SprungValue(T startingValue, float strength) : value(startingValue), strength(strength) {}
    SprungValue(T startingValue, T startingVelocity, float strength) :
        value(startingValue),
        velocity(startingVelocity),
        strength(strength),
            {}

    T Tick(float deltaTime); /// delta time is in seconds.

    T GetValue() const { return value; }
    T GetVelocity() const { return velocity; }

    void SetGoal(T newGoal, T newGoalVelocity=T())
            {
                goal = newGoal;
                goalVelocity = newGoalVelocity;
            }

private:
    T value = T();
    T velocity = T();

    T goal = value;
    T goalVelocity = T();

    float strength = 0.0f;
};

template <typename T>
inline
T SprungValue<T>::Tick(float deltaTime)
{
    T x0 = value - goal;
    T v0 = velocity - goalVelocity;
    float w = strength;
    float t = deltaTime;

    // Solution from http://www.ryanjuckett.com/programming/damped-springs/
    float wt = w * t;
    T v0_x0_w = v0 + (x0 * w);
    float e_wt = std::exp(-wt);

    T x = ((v0_x0_w * t) + x0) * e_wt;
    T v = (v0 - (v0_x0_w * wt)) * e_wt;

    value = x + goal;
    velocity = v + goalVelocity;

    return value;
}

#endif // SprungValue_h

