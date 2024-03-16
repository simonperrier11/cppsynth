/*
  ==============================================================================

    RotaryKnob.h
    Created: 16 Mar 2024 2:34:17pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 This class represents a rotary knob with a label.
 */
class RotaryKnob  : public juce::Component
{
public:
    juce::Slider slider;
    juce::String label;
    
    RotaryKnob();
    ~RotaryKnob() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};
