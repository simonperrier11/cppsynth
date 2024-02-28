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
        
        phase += increment;
        
        if (phase <= constants::PI_OVER_FOUR) {
            // Apply modulation
            float halfPeriod = (period / 2.0f) * modulation;
            
            phaseMax = std::floor(0.5f + halfPeriod) - 0.5f;
            dc = 0.5f * amplitude / phaseMax;
            phaseMax *= constants::PI;
            increment = phaseMax / halfPeriod;
            phase = -phase;
            
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
    float phaseMax;
    float sin0;
    float sin1;
    float dsin;
    float dc;
};
