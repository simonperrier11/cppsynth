/*
  ==============================================================================

    PinkNoise.h
    Created: 17 Mar 2024 3:34:27pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "NoiseGenerator.h"

/**
 Represents a pink noise (-3dB/octave).
 The algorithm is adapted from a snippet taken here : https://forum.juce.com/t/pink-noise-generator/40013/2
 */
class PinkNoise : public NoiseGenerator
{
public:
    void reset()
    {
        b0 = 0.0f;
        b1 = 0.0f;
        b2 = 0.0f;
        
        random.setSeedRandomly();
    }
    
    float getSample()
    {
        float whiteNoise = random.nextFloat() * 2.0f - 1.0f;
        b0 = 0.99765f * b0 + whiteNoise * 0.0990460f;
        b1 = 0.96300f * b1 + whiteNoise * 0.2965164f;
        b2 = 0.57000f * b2 + whiteNoise * 1.0526913f;
        float pinkNoise = b0 + b1 + b2 + whiteNoise * 0.1848f;
        
        return pinkNoise / 4.0f; // Attenuate pink noise
    }
    
private:
    float b0, b1, b2; // pink noise coefficiants
};
