/*
  ==============================================================================

    WavetableGenerator.h
    Created: 23 Apr 2024 12:45:34pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Constants.h"

/**
 This class contains the necessary functions to generate various wavetables.
 The fuctions here are taken and altered from here : https://thewolfsound.com/android-synthesizer-6-wavetable-synthesis-in-c-plus-plus/#wavetable-factory
 The wavetables generated are, except for the sine, limited in harmonics to prevent aliasing as much as possible.
 The harmonics limit is set depending on the frequency associated to the MIDI note. Higher frequencies will have less, 
 lower frequencies will have more.
 */
class WavetableGenerator
{
public:
    WavetableGenerator();

    /**
     Generates a sine wavetable.
     */
    std::vector<float> generateSineWavetable();
    
    /**
     Generates a triangle wavetable. This triangle waveform has a limit of 10 harmonics.
     */
    std::vector<float> generateTriangleWavetable();
    
    /**
     Generates a harmonics-limited square wavetable.
     */
    std::vector<float> generateSquareWavetable(int harmonicsCount);
    
    /**
     Generates a harmonics-limited sawtooth wavetable.
     */
    std::vector<float> generateSawtoothWavetable(int harmonicsCount);
};
