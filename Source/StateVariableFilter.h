/*
  ==============================================================================

    StateVariableFilter.h
    Created: 24 Feb 2024 1:29:38pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

class StateVariableFilter
{
    /**
     The state-variable filter implemented here is from : https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
     */
public:
    float sampleRate;
    
    /**
     Tick function of the filter.
     */
    virtual void updateCoefficiants(float cutoff, float q) = 0;
        
    /**
     Reinitializes the filter to its default state.
     */
    virtual void reset() = 0;
    
    /**
     Outputs the filter value for the input sample.
     */
    float render(float v0)
    {
        // voltages at nodes
        float v3 = v0 - ic2eq;
        float v1 = a1 * ic1eq + a2 * v3; // voltage for band-pass output
        float v2 = ic2eq + a2 * ic1eq + a3 * v3; // voltage for low-pass output
        
        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;
        
        return m0 * v0 + m1 * v1 + m2 * v2;
    }
    
protected:
    float g, k, a1, a2, a3; // filter coefficiants
    float ic1eq, ic2eq; // internal state for current sample
    float m0, m1, m2; // coefficiants that determine which type of filter is used (low, high, notch, etc.)
};
