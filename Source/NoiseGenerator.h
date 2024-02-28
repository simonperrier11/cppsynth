/*
  ==============================================================================

    NoiseGenerator.h
    Created: 23 Jan 2024 3:26:59pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

/**
 Represents a simple white noise generator.
 TODO: transform this into abstract class, write child classes for white/pink noise
 */
class NoiseGenerator
{
public:
    void reset()
    {
        noiseSeed = 22222;
    }

    float nextValue()
    {
        // Generate the next integer pseudorandom number
        noiseSeed = noiseSeed * 196314165 + 907633515;

        // Convert to a signed value
        int temp = int(noiseSeed >> 7) - 16777216;

        // Convert to a floating-point number between -1.0 and 1.0
        return float(temp) / 16777216.0f;
    }

private:
    unsigned int noiseSeed;
};
