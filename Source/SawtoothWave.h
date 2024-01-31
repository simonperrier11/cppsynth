/*
  ==============================================================================

    SawtoothWave.h
    Created: 28 Jan 2024 12:55:15pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "Oscillator.h"
#include "Constants.h"

/**
 Represents a bandlimited sawtooth wave for an oscillator.
 */

class SawtoothWave : public Oscillator
{
public:
    float freq;
    float sampleRate;
    float phaseBL; // phase counter for band-limited
    
    void reset() override
    {
        phase = 0.0f;
        phaseBL = -0.5f;
    }
    
    float nextBandLimitedSample()
    {
        phaseBL += increment;
        if (phaseBL >= 1.0f) {
            phaseBL -= 1.0f;
        }
        
        /*
         Band-limited sawtooth formula for a fundamental freq :
         2/pi * (sin(fund) - (sin(2*fund) / 2) + (sin(3*fund) / 3) - (sin(4*fund) / 4) + ...)
         */
        float output = 0.0f;
        float nyquist = sampleRate / 2.0f;
        float h = freq;
        float i = 1.0f;
        float m = constants::twoPi;
        
        // Add up sine waves in output until we reach the nyquist limit
        while (h < nyquist) {
            output += m * std::sin(constants::twoPi * phaseBL * i) / i;
            h += freq; // get frequency of the next harmonic
            i += 1.0f; // which harmonic we're currently at
            m = -m; // scaling factor 2/pi and sign flipping
        }
        return output;
    }
    
    float nextSample() override
    {
        return amplitude * nextBandLimitedSample();
    }
};
