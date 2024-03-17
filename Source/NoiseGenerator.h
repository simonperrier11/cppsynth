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
     Returns the next value for the noise generator.
     */
    virtual float nextValue() = 0;

protected:
    // The source of noise for all child classes
    juce::Random random;
};
