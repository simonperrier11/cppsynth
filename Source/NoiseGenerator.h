/*
  ==============================================================================

    NoiseGenerator.h
    Created: 23 Jan 2024 3:26:59pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

/**
 Represents a noise generator.
 */
class NoiseGenerator
{
public:
    /**
     Resets the noise generator
     */
    virtual void reset() = 0;

    /**
     Returns the next sample for the noise generator.
     */
    virtual float getSample() = 0;

protected:
    juce::Random random; // seed for noise
};
