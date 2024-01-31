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
 Represents a sawtooth wave for an oscillator, built using BLIT (bandlimited impulse trains).
 */

class SawtoothWave : public Oscillator
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


//class SawtoothWave : public Oscillator
//{
//public:
//    float freq;
//    float sampleRate;
//    float phaseBL; // phase counter for band-limited
//    
//    void reset() override
//    {
//        phase = 0.0f;
//        phaseBL = -0.5f;
//    }
//    
//    float nextBandLimitedSample()
//    {
//        phaseBL += increment;
//        if (phaseBL >= 1.0f) {
//            phaseBL -= 1.0f;
//        }
//        
//        /*
//         Band-limited sawtooth formula for a fundamental freq :
//         2/pi * (sin(fund) - (sin(2*fund) / 2) + (sin(3*fund) / 3) - (sin(4*fund) / 4) + ...)
//         */
//        float output = 0.0f;
//        float nyquist = sampleRate / 2.0f;
//        float h = freq;
//        float i = 1.0f;
//        float m = constants::twoPi;
//        
//        // Add up sine waves in output until we reach the nyquist limit
//        while (h < nyquist) {
//            output += m * std::sin(constants::twoPi * phaseBL * i) / i;
//            h += freq; // get frequency of the next harmonic
//            i += 1.0f; // which harmonic we're currently at
//            m = -m; // scaling factor 2/pi and sign flipping
//        }
//        return output;
//    }
//    
//    float nextSample() override
//    {
//        return amplitude * nextBandLimitedSample();
//    }
//};
