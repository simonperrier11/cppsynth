/*
  ==============================================================================

    LookAndFeel.h
    Created: 16 Mar 2024 3:09:57pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 This class represents the general look and feel for the UI.
 */
class LookAndFeel : public juce::LookAndFeel_V4 {
public:
    LookAndFeel();

    /**
     Draws a rotary slider with a needle-style dial. Taken from the "Creating Synthesizer Plug-Ins with C++ and Juce"
     book by Matthis Hollemans and The Audio Programmer.
     */
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};
