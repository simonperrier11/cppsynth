/*
  ==============================================================================

    WavetableOscillator.cpp
    Created: 20 Mar 2024 10:56:34am
    Author:  Simon Perrier

  ==============================================================================
*/

#include "WavetableOscillator.h"
#include <cmath>

// LRN use initializer list for quick and easy constructor
// LRN std::move declares an object as movable to the compiler; its value becomes an xvalue,
//  which means its resources can be reused elsewhere. Basically we transfer the content of an
//  object elsewhere instead of making a copy (super efficient in constructors!)
WavetableOscillator::WavetableOscillator(std::vector<float> waveTable, float sampleRate) : waveTable{ std::move(waveTable) }, sampleRate{ sampleRate } {}

void WavetableOscillator::setFrequency(float frequency)
{
    // this.frequency = frequency;
    indexIncrement = frequency * static_cast<float>(waveTable.size()) / sampleRate;
}

float WavetableOscillator::getSample()
{
    const auto sample = interpolateLinearly();
    index += indexIncrement;
    
    // After increment, bring back the index to the waveTable size's range
    index = std::fmod(index, static_cast<float>(waveTable.size()));
    
    return sample;
}

float WavetableOscillator::interpolateLinearly()
{
    // Get current index and next sample index
    const auto truncatedIndex = static_cast<int>(index);
    const auto nextIndex = (truncatedIndex + 1) % static_cast<int>(waveTable.size());
    
    // TODO: Calculate weights of indexes
    return 0.0f;
}
