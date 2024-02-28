/*
  ==============================================================================

    Envelope.h
    Created: 4 Feb 2024 10:36:59am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "Constants.h"

class Envelope
{
public:
    // Output level of the envelope
    float level;

    // Different multiplier values are used at different stages of the envelope
    float attackMultiplier;
    float decayMultiplier;
    float sustainLevel; // not a stage in itself
    float releaseMultiplier;
    
    /**
     Returns active status of the envelope.
     */
    inline bool isActive() const
    {
        return level > constants::SILENCE_TRESHOLD;
    }
    
    /**
     Returns in attack status; the target when in attack stage is 2.0 instead of 1.0.
     */
    inline bool isInAttack() const
    {
        return target >= constants::ENV_ATK_TARGET;
    }
    
    /**
     Sets the target to 2.0 and sets the current multiplier to the attack multiplier.
     */
    void attack()
    {
        // Give extra boost (silence tresh) so that the initial envelope is always greater than silence
        // The += assignement enables legato-style playing continue envelope instead of restarting it)
        level += constants::SILENCE_TRESHOLD + constants::SILENCE_TRESHOLD;
        target = constants::ENV_ATK_TARGET;
        multiplier = attackMultiplier;
    }
    
    /**
     Returns the next value of level.
     */
    float nextValue()
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
    
    /**
     Resets the envelope to its initial configuration.
     */
    void reset()
    {
        level = 0.0f;
        target = constants::ENV_REL_TARGET;
        multiplier = 0.0f;
    }
    
    /**
     Sets the target to 0 and sets the multiplier to the release multiplier.
     */
    void release()
    {
        target = constants::ENV_REL_TARGET;
        multiplier = releaseMultiplier;
    }
private:
    // Multiplier that is needed to make level go from 1.0 to 0.0001 (silence)
    //  in a certain amout of time
    float multiplier;
    // Target value of the envelope
    float target;
};
