/*
  ==============================================================================

    Blit.h
    Created: 31 Jan 2024 2:36:27pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "Oscillator.h"
#include "Constants.h"

/**
 Represents a bandlimited impulse train. Can be used to shape many waveforms.
 This is inspired by Creating Synthesizer Plug-Ins with C++ and JUCE by Matthijs Hollemans, published by The Audio Programmer
 TODO: change this for wavetable synthesis
 */

class Blit : public Oscillator
{
public:
    /*
     For this algorithm it makes more sense to think in terms of the period
     than the frequency, as the period gives you the number of samples between
     this impulse peak and the next
     */
    float period;
    /*
     Since this is a multiplier for the period — as opposed to the frequency — a value of
     modulation larger than 1 will make the note lower, while a value smaller than 1 will make
     the note higher
     */
    float modulation = 1.0f;
    
    void reset() override
    {
        increment = 0.0f;
        phase = 0.0f;
        
        sin0 = 0.0f;
        sin1 = 0.0f;
        dsin = 0.0f;
        
        dc = 0.0f;
    }
    
    float nextSample() override
    {
        float output = 0.0f;
        /*
         To generate the impulse train, sample between the peak of the last sinc function
         and the middle point (half a period) then mirror shape until time for peak of next impulse
         */
        
        // Increment phase
        phase += increment;
        
        // Start new impulse when phase reaches PI/4
        if (phase <= constants::PI_OVER_FOUR) {
            // Find midpoint (in samples) between previous sinc peak and the next one
            float halfPeriod = (period / 2.0f) * modulation; // Apply modulation
            phaseMax = std::floor(0.5f + halfPeriod) - 0.5f; // fudge halfway point to reduce aliasing
            
            // Calculate DC offset
            dc = 0.5f * amplitude / phaseMax;
            
            phaseMax *= constants::PI;
            increment = phaseMax / halfPeriod;
            phase = -phase;
            
            // TODO: not sure to fully understand this part, to review
            sin0 = amplitude * std::sin(phase);
            sin1 = amplitude * std::sin(phase - increment);
            dsin = 2.0f * std::cos(increment);

            if (phase*phase > 1e-9) {
                output = sin0 / phase;
            } else {
                output = amplitude;
            }
        }
        else {
            // If phase counter goes past halfway point, set phase to max and invert increment
            //  (output sinc function backwards)
            if (phase > phaseMax) {
                phase = phaseMax + phaseMax - phase;
                increment = -increment;
            }
            
            float sinp = dsin * sin0 - sin1;
            sin1 = sin0;
            sin0 = sinp;

            output = sinp / phase;
        }
        
        return output - dc;
    }
    
private:
    float phaseMax; // position of the midpoint between two impulse peaks
    float dc; // DC offset
    // Values for digital resonator approximation
    float sin0;
    float sin1;
    float dsin;
};
