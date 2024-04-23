/*
  ==============================================================================

    Envelope.cpp
    Created: 23 Apr 2024 1:29:07pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "Envelope.h"

bool Envelope::isActive() const
{
    return level > constants::SILENCE_TRESHOLD;
}

bool Envelope::isInAttack() const
{
    return target >= constants::ENV_ATK_TARGET;
}

void Envelope::attack()
{
    // Attack stage : Set the target to 2.0 and set the current multiplier to the attack multiplier
    
    /*
     Give extra boost (silence tresh) so that the initial envelope is always greater than silence. 
     The += assignement enables legato-style playing continue envelope instead of restarting it)
     */
    level += constants::SILENCE_TRESHOLD + constants::SILENCE_TRESHOLD;
    target = constants::ENV_ATK_TARGET;
    multiplier = attackMultiplier;
}

float Envelope::nextValue()
{
    // Exponentially reach target by applying a one-pole filter with the formula :
    //  y[n] = (1 - a) * x[n] + a * y[n - 1], where y[n] is the level at the current sample step n,
    //  x[n] is the target level, a is the multiplier and y[n-1] is the level at the previous sample step n
    // level = (1 - multiplier) * target + multiplier * level;
    level = multiplier * (level - target) + target;
    
    // To know if we're in the attack stage, we'll consider a target of 2.0 instead of the
    //  sustain's 1.0; once we've reach the end of the attack stage, switch to decay stage
    if (level + target > constants::ENV_SUS_TARGET + constants::ENV_ATK_TARGET) {
        multiplier = decayMultiplier;
        target = sustainLevel;
    }
    
    return level;
}

void Envelope::reset()
{
    level = 0.0f;
    target = constants::ENV_REL_TARGET;
    multiplier = 0.0f;
}

void Envelope::release()
{
    // Release stage : Set the target to 0 and set the multiplier to the release multiplier
    target = constants::ENV_REL_TARGET;
    multiplier = releaseMultiplier;
}
