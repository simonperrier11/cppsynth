/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

CppsynthAudioProcessorEditor::CppsynthAudioProcessorEditor (CppsynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Add elements to UI
    // Output level knob
    outputLevelKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    outputLevelKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(outputLevelKnob);
    
    setSize (600, 400);
}

CppsynthAudioProcessorEditor::~CppsynthAudioProcessorEditor()
{
}

void CppsynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background color
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CppsynthAudioProcessorEditor::resized()
{
    outputLevelKnob.setBounds(20, 20, 100, 120);
}
