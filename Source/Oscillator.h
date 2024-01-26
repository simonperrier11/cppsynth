/*
  ==============================================================================

    Oscillator.h
    Created: 24 Jan 2024 10:14:21am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

// TODO Oscillator should be a parent class (interface?) and have
// other classes inheriting from it : sine, saw, tri, etc.

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
    float increment; // determines the incrementation speed of phase
    float phase; // modulo counter from 0.0 to 1.0
    
    void reset()
    {
        //sampleIndex = 0;
        phase = 0.0f;
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
        
        phase += increment;
        if (phase >= 1.0f) {
            // Wrap back around phase to 0
            // We don't set it directly to 0, because if the value went a little
            // over 1.0, we want to keep its "extra progress" when resetting
            phase -= 1.0f;
        }
        
        // Multiply amplitude by the sin() function of 2pi * (0.0 to 1.0),
        // which is the full cycle of the sine wave (0 to 360 degrees)
        return amplitude * std::sin(constants::twoPi * phase);
    }
};
