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
//    float freq;
//    float sampleRate;
//    float phaseOffset;
//    int sampleIndex; // period is expressed in samples instead of time
    float amplitude;
    
    float increment; // determines the incrementation speed of phase
    float phase; // modulo counter from 0.0 to 1.0
    
    float sin0;
    float sin1;
    float dsin;
    
    void reset()
    {
        //sampleIndex = 0;
        
        phase = 0.0f;
        
        // Digital resonator method to init a sine wave very quickly
        // This way the sin() funcion is only called in reset()
        sin0 = amplitude * std::sin(phase * constants::twoPi);
        sin1 = amplitude * std::sin((phase - increment) * constants::twoPi);
        dsin = 2.0f * std::cos(increment * constants::twoPi);
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
        
//        phase += increment;
//        if (phase >= 1.0f) {
//            // Wrap back around phase to 0
//            // We don't set it directly to 0, because if the value went a little
//            // over 1.0, we want to keep its "extra progress" when resetting
//            phase -= 1.0f;
//        }
//        
//        // Multiply amplitude by the sin() function of 2pi * (0.0 to 1.0),
//        // which is the full cycle of the sine wave (0 to 360 degrees)
//        return amplitude * std::sin(constants::twoPi * phase);
        
        float sinx = dsin * sin0 - sin1;
        sin1 = sin0;
        sin0 = sinx;
        return sinx;
    }
    
    
};
