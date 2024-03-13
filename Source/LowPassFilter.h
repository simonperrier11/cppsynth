/*
  ==============================================================================

    LowPassFilter.h
    Created: 13 Mar 2024 12:59:44pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include "StateVariableFilter.h"

class LowPassFilter : public StateVariableFilter
{
public:
    void updateCoefficiants(float cutoff, float q)
    {
        g = std::tan(constants::PI * cutoff / sampleRate);
        k = 1.0f / q;
        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }
    
    void reset()
    {
        g = 0.0f;
        k = 0.0f;
        a1 = 0.0f;
        a2 = 0.0f;
        a3 = 0.0f;

        ic1eq = 0.0f;
        ic2eq = 0.0f;
        
        m0 = 0;
        m1 = 0;
        m2 = 1;
    }
};
