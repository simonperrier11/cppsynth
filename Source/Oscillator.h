/*
  ==============================================================================

    Oscillator.h
    Created: 24 Jan 2024 10:14:21am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

// 2 * pi

/**
 Represents an oscillator for the Synth. For now, it's a simple sine oscillator.
 */
class Oscillator
{
public:
    float amplitude;
//    float freq;
//    float sampleRate;
//    float phaseOffset;
//    int sampleIndex; // period is expressed in samples instead of time
    float increment; // determines the incrementation speed of phasor
    float phasor; // modulo counter from 0.0 to 1.0
    
    void reset()
    {
        //sampleIndex = 0;
        phasor = 0.0f;
    }
    
    float nextSample()
    {
//        // The formula for a sine wave is :
//        // y = amplitude * sin(2pi * t * frequency/sampleRate + phase)
//        float output = amplitude * std::sin(TWO_PI * sampleIndex * freq/sampleRate + phaseOffset);
//        
//        sampleIndex += 1;
//        
//        return output;
        
        phasor += increment;
        if (phasor >= 1.0f) {
            // Wrap back around phase to 0
            // We don't set it directly to 0, because if the value went a little
            // over 1.0, we want to keep its "extra progress" when resetting
            phasor -= 1.0f;
        }
        
        // The std::sin function takes an argument in radians. To render one cycle of the sine wave, the angle should go from 0 to 2*pi, which is the same as 0 to 360 degrees. After 360 degrees, the cycle repeats.
        return amplitude * std::sin(constants::twoPi * phasor);
    }
};
