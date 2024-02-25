/*
  ==============================================================================

    Filter.h
    Created: 24 Feb 2024 1:29:38pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

class Filter
{
    /**
     This state-variable filter is described in details here : https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
     See p. 31, case "low"
     */
public:
    float sampleRate;
    
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
    }
    
    float render(float sample)
    {
        // voltages at nodes
        float v3 = sample - ic2eq;
        float v1 = a1 * ic1eq + a2 * v3; // voltage for band-pass output
        float v2 = ic2eq + a2 * ic1eq + a3 * v3; // voltage for low-pass output
        
        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;
        
        return v2;
    }
    
private:
    float g, k, a1, a2, a3; // filter coefficiants
    float ic1eq, ic2eq; // internal state (current charge on capacitors)
};
