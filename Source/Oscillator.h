/*
  ==============================================================================

    Oscillator.h
    Created: 24 Jan 2024 10:14:21am
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

/**
 Represents an oscillator for the Synth. Every wave shape must inherit from this class.
 TODO: change this for wavetable synthesis
 */
class Oscillator
{
public:
    float amplitude;
    float increment; // determines the incrementation speed of phase
    float phase; // modulo counter from 0.0 to 1.0
    
    virtual void reset() = 0;
    virtual float nextSample() = 0;
};
