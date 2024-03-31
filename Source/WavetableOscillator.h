/*
  ==============================================================================

    WavetableOscillator.h
    Created: 20 Mar 2024 10:53:14am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>

/**
 This class represents a wavetable oscillator. A wavetable oscillator is an oscillator that uses a lookup table
 for its value at each sample point.
 Inspired from : https://thewolfsound.com/sound-synthesis/wavetable-synth-plugin-in-juce/
 */
class WavetableOscillator
{
public:
    float amplitude;
    float modulation;
    float initFrequency = 0; // Original frequency before modulation

    WavetableOscillator(std::vector<float> waveTable, float sampleRate);
    
    /**
     Calculates the indexIncrement according to the desired frequency in Hz.
     */
    void setFrequency(float frequency);
    
    /**
     Returns a sample of the oscillator and increments the index.
     */
    float getSample();
    
    /**
     Stops playback and resets index/index increment.
     */
    void stop();
    
    /**
     Returns true if the oscillator is playing.
     */
    bool isPlaying();
    
private:
    std::vector<float> waveTable;
    float sampleRate;
    float index = 0.0f;
    float indexIncrement = 0.0f;
    
    /**
     Interpolate between sample points in wavetable. Get weighted sum of the 2 nearest sample points of the index.
     */
    float interpolateLinearly();
};
