/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 16 Mar 2024 3:09:57pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "LookAndFeel.h"

LookAndFeel::LookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(30, 60, 90));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0, 0, 0));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(90, 180, 240));
    setColour(juce::Slider::thumbColourId, juce::Colour(255, 255, 255));
}
