/*
  ==============================================================================

    SawtoothWave.h
    Created: 28 Jan 2024 12:55:15pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "Oscillator.h"

/**
 Represents a sawtooth wave for an oscillator.
 */
class SawtoothWave : public Oscillator
{
public:
    void reset() override
    {
        phase = 0.0f;
    }
    
    float nextSample() override
    {
        phase += increment;
        if (phase >= 1.0f) {
            phase -= 1.0f; // wrap back around to 0
        }
        
        return amplitude * (2.0f * phase - 1.0f);
    }
};
