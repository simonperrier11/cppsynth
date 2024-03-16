/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 16 Mar 2024 2:34:17pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RotaryKnob.h"

// Constants for size
static constexpr int labelHeight = 15;
static constexpr int textBoxHeight = 20;

RotaryKnob::RotaryKnob()
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, textBoxHeight);
    addAndMakeVisible(slider);
    
    setBounds(0, 0, 20, 120);
}

RotaryKnob::~RotaryKnob()
{
}

void RotaryKnob::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    
    auto bounds = getLocalBounds();
    g.drawText (label, juce::Rectangle<int>{ 0, 0, bounds.getWidth(), labelHeight }, juce::Justification::centred);
    
//    // Debug : show rectangles
//    g.setColour(juce::Colours::yellow);
//    g.drawRect(0, labelHeight, bounds.getWidth(),
//               bounds.getHeight() - labelHeight - textBoxHeight, 1);
//
//    g.setColour(juce::Colours::green);
//    g.drawRect(0, 0, bounds.getWidth(), labelHeight, 1);
}

void RotaryKnob::resized()
{
    // Get juce::Rectangle object with current width and height of component
    auto bounds = getLocalBounds();
    slider.setBounds(0, labelHeight, bounds.getWidth(), bounds.getHeight() - labelHeight);
}
