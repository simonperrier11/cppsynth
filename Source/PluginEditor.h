/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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
    
    // LRN using here is used to set shortcut for class names (aliasing)
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;

    // Declare synth controls, and attach them to APVTS
    // LRN By making a slider object a regular member variable of the editor,
    //  it is instantiated when the editor window is opened.
    //  It is automatically destroyed when the editor window is closed.
    juce::Slider outputLevelKnob;
    SliderAttachment outputLevelAttachment { audioProcessor.apvts, ParameterID::outputLevel.getParamID(), outputLevelKnob };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CppsynthAudioProcessorEditor)
};
