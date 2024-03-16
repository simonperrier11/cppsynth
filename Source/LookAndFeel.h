/*
  ==============================================================================

    LookAndFeel.h
    Created: 16 Mar 2024 3:09:57pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4 {
public:
    LookAndFeel();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};
