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
 Represents a bandlimited impulse train. Can be used to shape many waveforms. */

class Blit : public Oscillator
{
public:
    // For this algorithm it makes more sense to think in terms of the period
    // than the frequency, as the period gives you the number of samples between
    // this impulse peak and the next.
    float period;
    
    void reset() override
    {
        increment = 0.0f;
        phase = 0.0f;
        
        sin0 = 0.0f;
        sin1 = 0.0f;
        dsin = 0.0f;
        
        dc = 0.0f;
    }
    
    // TODO change approach
    float nextSample() override
    {
        float output = 0.0f;
        
        phase += increment;
        
        if (phase <= constants::piOverFour) {
            float halfPeriod = period / 2.0f;
            
            phaseMax = std::floor(0.5f + halfPeriod) - 0.5f;
            dc = 0.5f * amplitude / phaseMax;
            phaseMax *= constants::pi;
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