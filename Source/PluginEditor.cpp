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
    osc1LevelKnob.label = "Level";
    addAndMakeVisible(osc1LevelKnob);
    osc1MorphKnob.label = "Shape";
    addAndMakeVisible(osc1MorphKnob);

    // Noise
    noiseTypeButton.setButtonText(juce::CharPointer_UTF8("Noise Type"));
    noiseTypeButton.setClickingTogglesState(true);
    addAndMakeVisible(noiseTypeButton);
    noiseLevelKnob.label = "Level";
    addAndMakeVisible(noiseLevelKnob);
    
    // OSC2
    osc2LevelKnob.label = "Level";
    addAndMakeVisible(osc2LevelKnob);
    osc2MorphKnob.label = "Shape";
    addAndMakeVisible(osc2MorphKnob);
    oscTuneKnob.label = "Semitones";
    addAndMakeVisible(oscTuneKnob);
    oscFineKnob.label = "Cents";
    addAndMakeVisible(oscFineKnob);
    
    // ENV
    envAttackKnob.label = "Attack";
    addAndMakeVisible(envAttackKnob);
    envDecayKnob.label = "Decay";
    addAndMakeVisible(envDecayKnob);
    envSustainKnob.label = "Sustain";
    addAndMakeVisible(envSustainKnob);
    envReleaseKnob.label = "Release";
    addAndMakeVisible(envReleaseKnob);
    
    // LPF
    lpfFreqKnob.label = "Frequency";
    addAndMakeVisible(lpfFreqKnob);
    lpfResoKnob.label = "Q";
    addAndMakeVisible(lpfResoKnob);
    lpfEnvKnob.label = "Envelope %";
    addAndMakeVisible(lpfEnvKnob);
    lpfLFOKnob.label = "LFO %";
    addAndMakeVisible(lpfLFOKnob);
    
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
    hpfFreqKnob.label = "Frequency";
    addAndMakeVisible(hpfFreqKnob);
    hpfResoKnob.label = "Q";
    addAndMakeVisible(hpfResoKnob);
    hpfEnvKnob.label = "Envelope %";
    addAndMakeVisible(hpfEnvKnob);
    hpfLFOKnob.label = "LFO %";
    addAndMakeVisible(hpfLFOKnob);

    // HPF ENV
    hpfAttackKnob.label = "Attack";
    addAndMakeVisible(hpfAttackKnob);
    hpfDecayKnob.label = "Decay";
    addAndMakeVisible(hpfDecayKnob);
    hpfSustainKnob.label = "Sustain";
    addAndMakeVisible(hpfSustainKnob);
    hpfReleaseKnob.label = "Release";
    addAndMakeVisible(hpfReleaseKnob);
    
    // LFO
    lfoRateKnob.label = "LFO Rate";
    addAndMakeVisible(lfoRateKnob);
    vibratoKnob.label = "Depth";
    addAndMakeVisible(vibratoKnob);



//    polyModeButton.setButtonText(juce::CharPointer_UTF8("Poly"));
//    polyModeButton.setClickingTogglesState(true);
//    addAndMakeVisible(polyModeButton);
//    outputLevelKnob.label = "Output Level";
//    addAndMakeVisible(outputLevelKnob);
    
//    // Plugin size on open
//    setResizable(true, false);
//    // LRN: minX, minY, maxX, maxY
//    setResizeLimits(600, 400, 1400, 1000);
    setSize(1100, 500);
}

CppsynthAudioProcessorEditor::~CppsynthAudioProcessorEditor()
{
}

void CppsynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background color
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CppsynthAudioProcessorEditor::resized()
{
    // Rectangle bound for UI elements
    // LRN: params are posX, posY, sizeX, sizeY
    juce::Rectangle osc1Elem(20, 20, 80, 100);
    juce::Rectangle osc2Elem(140, 20, 80, 100);
    juce::Rectangle envElem(260, 20, 80, 100);
    juce::Rectangle lfoElem(380, 20, 80, 100);
    juce::Rectangle lpfElem(380 + 120, 20, 80, 100);
    juce::Rectangle lpfEnvElem(465 + 120, 20, 80, 100);
    juce::Rectangle hpfElem(575 + 120, 20, 80, 100);
    juce::Rectangle hpfEnvElem(660 + 120, 20, 80, 100);
    
    // OSC1
    osc1LevelKnob.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);
    osc1MorphKnob.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);
    
    // Noise
    noiseTypeButton.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);
    noiseLevelKnob.setBounds(osc1Elem);
    osc1Elem = osc1Elem.withY(osc1Elem.getBottom() + 20);

    // OSC2
    osc2LevelKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    osc2MorphKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    oscTuneKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    oscFineKnob.setBounds(osc2Elem);
    osc2Elem = osc2Elem.withY(osc2Elem.getBottom() + 20);
    
    // ENV
    envAttackKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    envDecayKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    envSustainKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    envReleaseKnob.setBounds(envElem);
    envElem = envElem.withY(envElem.getBottom() + 20);
    
    // OSC1
    lfoRateKnob.setBounds(lfoElem);
    lfoElem = lfoElem.withY(lfoElem.getBottom() + 20);
    vibratoKnob.setBounds(lfoElem);
    lfoElem = lfoElem.withY(lfoElem.getBottom() + 20);

    // LPF
    lpfFreqKnob.setBounds(lpfElem);
    lpfElem = lpfElem.withY(lpfElem.getBottom() + 20);
    lpfResoKnob.setBounds(lpfElem);
    lpfElem = lpfElem.withY(lpfElem.getBottom() + 20);
    lpfEnvKnob.setBounds(lpfElem);
    lpfElem = lpfElem.withY(lpfElem.getBottom() + 20);
    lpfLFOKnob.setBounds(lpfElem);
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
    hpfFreqKnob.setBounds(hpfElem);
    hpfElem = hpfElem.withY(hpfElem.getBottom() + 20);
    hpfResoKnob.setBounds(hpfElem);
    hpfElem = hpfElem.withY(hpfElem.getBottom() + 20);
    hpfEnvKnob.setBounds(hpfElem);
    hpfElem = hpfElem.withY(hpfElem.getBottom() + 20);
    hpfLFOKnob.setBounds(hpfElem);
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

//    polyModeButton.setSize(80, 30);
//    polyModeButton.setCentrePosition(r.withX(r.getRight()).getCentre());
//    outputLevelKnob.setBounds(r);
//    r = r.withX(r.getRight() + 20);
//
}
