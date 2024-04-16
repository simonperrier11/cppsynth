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
    void paint(juce::Graphics&) override;
    
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
    
    // Generic labels to identify sections
    juce::Label osc1Label;
    juce::Label osc2Label;
    juce::Label noiseLabel;
    juce::Label envLabel;
    juce::Label lfoLabel;
    juce::Label lpfLabel;
    juce::Label hpfLabel;
    juce::Label masterLabel;
    
    // LRN using here is used to set shortcut for class names (aliasing)
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;

    // Declare synth controls, and attach them to APVTS
    // LRN By making a RotaryKnob object a regular member variable of the editor,
    //  it is instantiated when the editor window is opened.
    //  It is automatically destroyed when the editor window is closed.
    // MASTER
    RotaryKnob outputLevelKnob;
    SliderAttachment outputLevelAttachment { audioProcessor.apvts, ParameterID::outputLevel.getParamID(), outputLevelKnob.slider };
    RotaryKnob octaveKnob;
    SliderAttachment octaveAttachment { audioProcessor.apvts, ParameterID::octave.getParamID(), octaveKnob.slider };
    RotaryKnob tuningKnob;
    SliderAttachment tuningAttachment { audioProcessor.apvts, ParameterID::tuning.getParamID(), tuningKnob.slider };

    // OSC1
    RotaryKnob osc1LevelKnob;
    SliderAttachment osc1LevelAttachment { audioProcessor.apvts, ParameterID::osc1Level.getParamID(), osc1LevelKnob.slider };
    RotaryKnob osc1MorphKnob;
    SliderAttachment osc1MorphAttachment { audioProcessor.apvts, ParameterID::osc1Morph.getParamID(), osc1MorphKnob.slider };
    
    // Noise
    RotaryKnob noiseLevelKnob;
    SliderAttachment noiseLevelAttachment { audioProcessor.apvts, ParameterID::noiseLevel.getParamID(), noiseLevelKnob.slider };

    // OSC2
    RotaryKnob osc2LevelKnob;
    SliderAttachment osc2LevelAttachment { audioProcessor.apvts, ParameterID::osc2Level.getParamID(), osc2LevelKnob.slider };
    RotaryKnob osc2MorphKnob;
    SliderAttachment osc2MorphAttachment { audioProcessor.apvts, ParameterID::osc2Morph.getParamID(), osc2MorphKnob.slider };
    RotaryKnob oscTuneKnob;
    SliderAttachment oscTuneAttachment { audioProcessor.apvts, ParameterID::oscTune.getParamID(), oscTuneKnob.slider };
    RotaryKnob oscFineKnob;
    SliderAttachment oscFineAttachment { audioProcessor.apvts, ParameterID::oscFine.getParamID(), oscFineKnob.slider };
    
    // ENV
    RotaryKnob envAttackKnob;
    SliderAttachment envAttackAttachment { audioProcessor.apvts, ParameterID::envAttack.getParamID(), envAttackKnob.slider };
    RotaryKnob envDecayKnob;
    SliderAttachment envDecayAttachment { audioProcessor.apvts, ParameterID::envDecay.getParamID(), envDecayKnob.slider };
    RotaryKnob envSustainKnob;
    SliderAttachment envSustainAttachment { audioProcessor.apvts, ParameterID::envSustain.getParamID(), envSustainKnob.slider };
    RotaryKnob envReleaseKnob;
    SliderAttachment envReleaseAttachment { audioProcessor.apvts, ParameterID::envRelease.getParamID(), envReleaseKnob.slider };

    // LPF
    RotaryKnob lpfFreqKnob;
    SliderAttachment lpfFreqAttachment { audioProcessor.apvts, ParameterID::lpfFreq.getParamID(), lpfFreqKnob.slider };
    RotaryKnob lpfResoKnob;
    SliderAttachment lpfResoAttachment { audioProcessor.apvts, ParameterID::lpfReso.getParamID(), lpfResoKnob.slider };
    RotaryKnob lpfEnvKnob;
    SliderAttachment lpfEnvAttachment { audioProcessor.apvts, ParameterID::lpfEnv.getParamID(), lpfEnvKnob.slider };
    RotaryKnob lpfLFOKnob;
    SliderAttachment lpfLFOAttachment { audioProcessor.apvts, ParameterID::lpfLFO.getParamID(), lpfLFOKnob.slider };
    
    // LPF ENV
    RotaryKnob lpfAttackKnob;
    SliderAttachment lpfAttackAttachment { audioProcessor.apvts, ParameterID::lpfAttack.getParamID(), lpfAttackKnob.slider };
    RotaryKnob lpfDecayKnob;
    SliderAttachment lpfDecayAttachment { audioProcessor.apvts, ParameterID::lpfDecay.getParamID(), lpfDecayKnob.slider };
    RotaryKnob lpfSustainKnob;
    SliderAttachment lpfSustainAttachment { audioProcessor.apvts, ParameterID::lpfSustain.getParamID(), lpfSustainKnob.slider };
    RotaryKnob lpfReleaseKnob;
    SliderAttachment lpfReleaseAttachment { audioProcessor.apvts, ParameterID::lpfRelease.getParamID(), lpfReleaseKnob.slider };
    
    // HPF
    RotaryKnob hpfFreqKnob;
    SliderAttachment hpfFreqAttachment { audioProcessor.apvts, ParameterID::hpfFreq.getParamID(), hpfFreqKnob.slider };
    RotaryKnob hpfResoKnob;
    SliderAttachment hpfResoAttachment { audioProcessor.apvts, ParameterID::hpfReso.getParamID(), hpfResoKnob.slider };
    RotaryKnob hpfEnvKnob;
    SliderAttachment hpfEnvAttachment { audioProcessor.apvts, ParameterID::hpfEnv.getParamID(), hpfEnvKnob.slider };
    RotaryKnob hpfLFOKnob;
    SliderAttachment hpfLFOAttachment { audioProcessor.apvts, ParameterID::hpfLFO.getParamID(), hpfLFOKnob.slider };

    // HPF ENV
    RotaryKnob hpfAttackKnob;
    SliderAttachment hpfAttackAttachment { audioProcessor.apvts, ParameterID::hpfAttack.getParamID(), hpfAttackKnob.slider };
    RotaryKnob hpfDecayKnob;
    SliderAttachment hpfDecayAttachment { audioProcessor.apvts, ParameterID::hpfDecay.getParamID(), hpfDecayKnob.slider };
    RotaryKnob hpfSustainKnob;
    SliderAttachment hpfSustainAttachment { audioProcessor.apvts, ParameterID::hpfSustain.getParamID(), hpfSustainKnob.slider };
    RotaryKnob hpfReleaseKnob;
    SliderAttachment hpfReleaseAttachment { audioProcessor.apvts, ParameterID::hpfRelease.getParamID(), hpfReleaseKnob.slider };
    
    // LFO
    RotaryKnob lfoRateKnob;
    SliderAttachment lfoRateAttachment { audioProcessor.apvts, ParameterID::lfoRate.getParamID(), lfoRateKnob.slider };
    RotaryKnob vibratoKnob;
    SliderAttachment vibratoAttachment { audioProcessor.apvts, ParameterID::vibrato.getParamID(), vibratoKnob.slider };

    // Toggles
    juce::TextButton polyModeButton;
    ButtonAttachment polyModeAttachment { audioProcessor.apvts, ParameterID::polyMode.getParamID(), polyModeButton };
    juce::TextButton noiseTypeButton;
    ButtonAttachment noiseTypeAttachment { audioProcessor.apvts, ParameterID::noiseType.getParamID(), noiseTypeButton };
    juce::TextButton velocitySensitivityButton;
    ButtonAttachment velocitySensitivityAttachment { audioProcessor.apvts, ParameterID::velocitySensitivity.getParamID(), velocitySensitivityButton };
    juce::TextButton ringModButton;
    ButtonAttachment ringModAttachment { audioProcessor.apvts, ParameterID::ringMod.getParamID(), ringModButton };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CppsynthAudioProcessorEditor)
};
