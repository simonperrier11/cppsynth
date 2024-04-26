/*
  ==============================================================================

    Envelope.h
    Created: 4 Feb 2024 10:36:59am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "Constants.h"

/**
 This class represents an ADSR envelope. The inner working is that of a one-pole filter.
 */
class Envelope
{
public:
    float level; // output level of the envelope

    // different multiplier values are used at different stages of the envelope
    float attackMultiplier;
    float decayMultiplier;
    float sustainLevel; // sustain is not a stage in itself
    float releaseMultiplier;
    
    /**
     Returns active status of the envelope.
     */
    bool isActive() const;
    
    /**
     Returns in attack status; the target when in attack stage is 2.0 instead of 1.0.
     */
    bool isInAttack() const;
    
    /**
     Triggers the envelope's attack stage.
     */
    void attack();
    
    /**
     Returns the next value (level) of the envelope.
     */
    float nextValue();
    
    /**
     Resets the envelope to its initial state.
     */
    void reset();
    
    /**
     Sets the target to 0 and sets the multiplier to the release multiplier.
     */
    void release();
private:
    float multiplier; // envelope multiplier to calculate slope from 1.0 to silence
    float target; // Target value of the envelope
};
