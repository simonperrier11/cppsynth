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
    
    // OSC1
    osc1Label.setText("# OSC1 #", {});
    osc1Label.setJustificationType(juce::Justification::centred);
    osc1Label.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(osc1Label);
    osc1LevelKnob.label = "Level";
    addAndMakeVisible(osc1LevelKnob);
    osc1MorphKnob.label = "Shape";
    addAndMakeVisible(osc1MorphKnob);

    // Noise
    noiseLabel.setText("# NOISE #", {});
    noiseLabel.setJustificationType(juce::Justification::centred);
    noiseLabel.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(noiseLabel);
    noiseLevelKnob.label = "Level";
    addAndMakeVisible(noiseLevelKnob);
    
    // OSC2
    osc2Label.setText("# OSC2 #", {});
    osc2Label.setJustificationType(juce::Justification::centred);
    osc2Label.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(osc2Label);
    osc2LevelKnob.label = "Level";
    addAndMakeVisible(osc2LevelKnob);
    osc2MorphKnob.label = "Shape";
    addAndMakeVisible(osc2MorphKnob);
    oscTuneKnob.label = "Semitones";
    addAndMakeVisible(oscTuneKnob);
    oscFineKnob.label = "Cents";
    addAndMakeVisible(oscFineKnob);
    
    // ENV
    envLabel.setText("# ENVELOPE #", {});
    envLabel.setJustificationType(juce::Justification::centred);
    envLabel.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(envLabel);
    envAttackKnob.label = "Attack";
    addAndMakeVisible(envAttackKnob);
    envDecayKnob.label = "Decay";
    addAndMakeVisible(envDecayKnob);
    envSustainKnob.label = "Sustain";
    addAndMakeVisible(envSustainKnob);
    envReleaseKnob.label = "Release";
    addAndMakeVisible(envReleaseKnob);
    
    // LFO
    lfoLabel.setText("# LFO #", {});
    lfoLabel.setJustificationType(juce::Justification::centred);
    lfoLabel.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(lfoLabel);
    lfoRateKnob.label = "Rate";
    addAndMakeVisible(lfoRateKnob);
    vibratoKnob.label = "Pitch %";
    addAndMakeVisible(vibratoKnob);
    lpfLFOKnob.label = "LPF %";
    addAndMakeVisible(lpfLFOKnob);
    hpfLFOKnob.label = "HPF %";
    addAndMakeVisible(hpfLFOKnob);
    
    // LPF
    lpfLabel.setText("# LOWPASS FILTER #", {});
    lpfLabel.setJustificationType(juce::Justification::centred);
    lpfLabel.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(lpfLabel);
    lpfFreqKnob.label = "Frequency";
    addAndMakeVisible(lpfFreqKnob);
    lpfResoKnob.label = "Q";
    addAndMakeVisible(lpfResoKnob);
    lpfEnvKnob.label = "Envelope %";
    addAndMakeVisible(lpfEnvKnob);
    
    // LPF ENV
    lpfAttackKnob.label = "Attack";
    addAndMakeVisible(lpfAttackKnob);
    lpfDecayKnob.label = "Decay";
    addAndMakeVisible(lpfDecayKnob);
    lpfSustainKnob.label = "Sustain";
    addAndMakeVisible(lpfSustainKnob);
    lpfReleaseKnob.label = "Release";
    addAndMakeVisible(lpfReleaseKnob);
    
    // HPF
    hpfLabel.setText("# HIGHPASS FILTER #", {});
    hpfLabel.setJustificationType(juce::Justification::centred);
    hpfLabel.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(hpfLabel);
    hpfFreqKnob.label = "Frequency";
    addAndMakeVisible(hpfFreqKnob);
    hpfResoKnob.label = "Q";
    addAndMakeVisible(hpfResoKnob);
    hpfEnvKnob.label = "Envelope %";
    addAndMakeVisible(hpfEnvKnob);

    // HPF ENV
    hpfAttackKnob.label = "Attack";
    addAndMakeVisible(hpfAttackKnob);
    hpfDecayKnob.label = "Decay";
    addAndMakeVisible(hpfDecayKnob);
    hpfSustainKnob.label = "Sustain";
    addAndMakeVisible(hpfSustainKnob);
    hpfReleaseKnob.label = "Release";
    addAndMakeVisible(hpfReleaseKnob);

    // Toggles
    polyModeButton.setButtonText(juce::CharPointer_UTF8("Poly"));
    polyModeButton.setClickingTogglesState(true);
    addAndMakeVisible(polyModeButton);
    velocitySensitivityButton.setButtonText(juce::CharPointer_UTF8("Velocity"));
    velocitySensitivityButton.setClickingTogglesState(true);
    addAndMakeVisible(velocitySensitivityButton);
    noiseTypeButton.setButtonText(juce::CharPointer_UTF8("White/Pink"));
    noiseTypeButton.setClickingTogglesState(true);
    addAndMakeVisible(noiseTypeButton);
    ringModButton.setButtonText(juce::CharPointer_UTF8("Ring Mod"));
    ringModButton.setClickingTogglesState(true);
    addAndMakeVisible(ringModButton);
    phaseRandButton.setButtonText(juce::CharPointer_UTF8("Phase Rand"));
    phaseRandButton.setClickingTogglesState(true);
    addAndMakeVisible(phaseRandButton);

    // MASTER
    masterLabel.setText("# MASTER SETTINGS #", {});
    masterLabel.setJustificationType(juce::Justification::centred);
    masterLabel.setColour(juce::Label::textColourId, juce::Colours::greenyellow);
    addAndMakeVisible(masterLabel);
    outputLevelKnob.label = "Volume";
    addAndMakeVisible(outputLevelKnob);
    tuningKnob.label = "Cents";
    addAndMakeVisible(tuningKnob);
    
    // Plugin size
    setSize(1100, 550);
}

CppsynthAudioProcessorEditor::~CppsynthAudioProcessorEditor()
{
}

void CppsynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background color
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void CppsynthAudioProcessorEditor::resized()
{
    // Rectangle bound for UI elements
    // LRN: params are posX, posY, sizeX, sizeY
    juce::Rectangle osc1LabelPos(20, 10, 80, 40);
    juce::Rectangle osc1Elem(20, 60, 80, 100);
    juce::Rectangle noiseLabelPos(20, 300, 80, 40);
    juce::Rectangle osc2LabelPos(140, 10, 80, 40);
    juce::Rectangle osc2Elem(140, 60, 80, 100);
    juce::Rectangle envLabelPos(260, 10, 80, 40);
    juce::Rectangle envElem(260, 60, 80, 100);
    juce::Rectangle lfoLabelPos(380, 10, 80, 40);
    juce::Rectangle lfoElem(380, 60, 80, 100);
    juce::Rectangle lpfLabelPos(515, 10, 120, 40);
    juce::Rectangle lpfElem(500, 60, 80, 100);
    juce::Rectangle lpfEnvElem(585, 60, 80, 100);
    juce::Rectangle hpfLabelPos(710, 10, 120, 40);
    juce::Rectangle hpfElem(695, 60, 80, 100);
    juce::Rectangle hpfEnvElem(780, 60, 80, 100);
    juce::Rectangle masterLabelPos(930, 10, 120, 40);
    juce::Rectangle masterElem(900, 60, 80, 100);
    juce::Rectangle lastColElem(995, 60, 80, 100);
    
    // OSC1
    osc1Label.setBounds(osc1LabelPos);
    osc1LevelKnob.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);
    osc1MorphKnob.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 10);
    
    // Noise
    noiseLabel.setBounds(noiseLabelPos);
    osc1Elem = osc1Elem.withY(noiseLabelPos.getBottom() + 10);
    osc1Elem = osc1Elem.withHeight(50);
    noiseTypeButton.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);
    osc1Elem = osc1Elem.withHeight(100);
    noiseLevelKnob.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);

    // OSC2
    osc2Label.setBounds(osc2LabelPos);
    osc2LevelKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    osc2MorphKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    oscTuneKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    oscFineKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    
    // ENV
    envLabel.setBounds(envLabelPos);
    envAttackKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    envDecayKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    envSustainKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    envReleaseKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    
    // LFO
    lfoLabel.setBounds(lfoLabelPos);
    lfoRateKnob.setBounds(lfoElem);
    lfoElem = lfoElem.withY(lfoElem.getBottom() + 20);
    vibratoKnob.setBounds(lfoElem);
    lfoElem = lfoElem.withY(lfoElem.getBottom() + 20);
    lpfLFOKnob.setBounds(lfoElem);
    lfoElem = lfoElem.withY(lfoElem.getBottom() + 20);
    hpfLFOKnob.setBounds(lfoElem);
    lfoElem = lfoElem.withY(lfoElem.getBottom() + 20);

    // LPF
    lpfLabel.setBounds(lpfLabelPos);
    lpfFreqKnob.setBounds(lpfElem);
    lpfElem = lpfElem.withY(lpfElem.getBottom() + 20);
    lpfResoKnob.setBounds(lpfElem);
    lpfElem = lpfElem.withY(lpfElem.getBottom() + 20);
    lpfEnvKnob.setBounds(lpfElem);
    lpfElem = lpfElem.withY(lpfElem.getBottom() + 20);
    
    // LPF ENV
    lpfAttackKnob.setBounds(lpfEnvElem);
    lpfEnvElem = lpfEnvElem.withY(lpfEnvElem.getBottom() + 20);
    lpfDecayKnob.setBounds(lpfEnvElem);
    lpfEnvElem = lpfEnvElem.withY(lpfEnvElem.getBottom() + 20);
    lpfSustainKnob.setBounds(lpfEnvElem);
    lpfEnvElem = lpfEnvElem.withY(lpfEnvElem.getBottom() + 20);
    lpfReleaseKnob.setBounds(lpfEnvElem);
    lpfEnvElem = lpfEnvElem.withY(lpfEnvElem.getBottom() + 20);

    // HPF
    hpfLabel.setBounds(hpfLabelPos);
    hpfFreqKnob.setBounds(hpfElem);
    hpfElem = hpfElem.withY(hpfElem.getBottom() + 20);
    hpfResoKnob.setBounds(hpfElem);
    hpfElem = hpfElem.withY(hpfElem.getBottom() + 20);
    hpfEnvKnob.setBounds(hpfElem);
    hpfElem = hpfElem.withY(hpfElem.getBottom() + 20);
    
    // HPF ENV
    hpfAttackKnob.setBounds(hpfEnvElem);
    hpfEnvElem = hpfEnvElem.withY(hpfEnvElem.getBottom() + 20);
    hpfDecayKnob.setBounds(hpfEnvElem);
    hpfEnvElem = hpfEnvElem.withY(hpfEnvElem.getBottom() + 20);
    hpfSustainKnob.setBounds(hpfEnvElem);
    hpfEnvElem = hpfEnvElem.withY(hpfEnvElem.getBottom() + 20);
    hpfReleaseKnob.setBounds(hpfEnvElem);
    hpfEnvElem = hpfEnvElem.withY(hpfEnvElem.getBottom() + 20);
    
    // Master
    masterLabel.setBounds(masterLabelPos);
    outputLevelKnob.setBounds(masterElem);
    masterElem = masterElem.withY(masterElem.getBottom() + 20);
    tuningKnob.setBounds(masterElem);
    masterElem = masterElem.withY(masterElem.getBottom() + 20);
    phaseRandButton.setBounds(masterElem);
    masterElem = masterElem.withY(masterElem.getBottom() + 20);

    // Other settings
    polyModeButton.setBounds(lastColElem);
    lastColElem = lastColElem.withY(lastColElem.getBottom() + 20);
    velocitySensitivityButton.setBounds(lastColElem);
    lastColElem = lastColElem.withY(lastColElem.getBottom() + 20);
    ringModButton.setBounds(lastColElem);
    lastColElem = lastColElem.withY(lastColElem.getBottom() + 20);
}
