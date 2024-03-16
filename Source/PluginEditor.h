/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"

class CppsynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CppsynthAudioProcessorEditor (CppsynthAudioProcessor&);
    ~CppsynthAudioProcessorEditor() override;

    /**
     Draws contents of the UI.
     */
    void paint (juce::Graphics&) override;
    
    /**
     Lays out position of contents of the UI.
     */
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CppsynthAudioProcessor& audioProcessor;
    
    // Global style
    LookAndFeel globalLNF;
    
    // LRN using here is used to set shortcut for class names (aliasing)
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;

    // Declare synth controls, and attach them to APVTS
    // LRN By making a slider object a regular member variable of the editor,
    //  it is instantiated when the editor window is opened.
    //  It is automatically destroyed when the editor window is closed.
    RotaryKnob outputLevelKnob;
    SliderAttachment outputLevelAttachment { audioProcessor.apvts, ParameterID::outputLevel.getParamID(), outputLevelKnob.slider };

    RotaryKnob lpfResoKnob;
    SliderAttachment lpfResoAttachment { audioProcessor.apvts, ParameterID::lpfReso.getParamID(), lpfResoKnob.slider };
    
    juce::TextButton polyModeButton;
    ButtonAttachment polyModeAttachment { audioProcessor.apvts, ParameterID::polyMode.getParamID(), polyModeButton };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CppsynthAudioProcessorEditor)
};
