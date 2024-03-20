/*
  ==============================================================================

    WavetableOscillator.h
    Created: 20 Mar 2024 10:53:14am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once
#include <vector>

class WavetableOscillator
{
public:
    float amplitude;
    float modulation;
    // float frequency;

    WavetableOscillator(std::vector<float> waveTable, float sampleRate);
    
    /**
     Calculates the indexIncrement according to the desired frequency in Hz.
     */
    void setFrequency(float frequency);
    
    /**
     Returns a sample of the oscillator and increments the index.
     */
    float getSample();
    
private:
    std::vector<float> waveTable;
    float sampleRate;
    float index = 0.0f;
    float indexIncrement = 0.0f;
    
    /**
     Interpolate between sample points in wavetable.
     */
    float interpolateLinearly();
};
