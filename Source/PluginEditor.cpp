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
    // Set global look and feel
    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);
    
    // Add elements to UI
    // Poly mode button 
    polyModeButton.setButtonText(juce::CharPointer_UTF8("Poly"));
    polyModeButton.setClickingTogglesState(true);
    addAndMakeVisible(polyModeButton);
    
    // Output level knob
    outputLevelKnob.label = "Output Level";
    addAndMakeVisible(outputLevelKnob);
    
    // LPF Q knob
    lpfResoKnob.label = "LPF Q";
    addAndMakeVisible(lpfResoKnob);
    
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
    // Rectangle bound for UI elements
    juce::Rectangle r(20, 20, 100, 120);
    
    outputLevelKnob.setBounds(r);
    r = r.withX(r.getRight() + 20);
    lpfResoKnob.setBounds(r);
    polyModeButton.setSize(80, 30);
    polyModeButton.setCentrePosition(r.withX(r.getRight()).getCentre());
}
