/*
  ==============================================================================

    WhiteNoise.h
    Created: 17 Mar 2024 3:34:22pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "NoiseGenerator.h"

/**
 Represents a white noise, the most basic of noise.
 */
class WhiteNoise : public NoiseGenerator
{
public:
    void reset()
    {
        random.setSeedRandomly();
    }

    float nextValue()
    {
        return random.nextFloat() * 2.0f - 1.0f;
    }
};
