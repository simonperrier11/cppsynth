/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

// LRN Skip current header if already included once (non-standard)
#pragma once

#include <JuceHeader.h>
#include "Synth.h"

// IDs for various parameters accessible to host
namespace ParameterID
{
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

    PARAMETER_ID(oscMix)
    PARAMETER_ID(oscTune)
    PARAMETER_ID(oscFine)
    PARAMETER_ID(glideMode)
    PARAMETER_ID(glideRate)
    PARAMETER_ID(glideBend)
    PARAMETER_ID(filterFreq)
    PARAMETER_ID(filterReso)
    PARAMETER_ID(filterEnv)
    PARAMETER_ID(filterLFO)
    PARAMETER_ID(filterVelocity)
    PARAMETER_ID(filterAttack)
    PARAMETER_ID(filterDecay)
    PARAMETER_ID(filterSustain)
    PARAMETER_ID(filterRelease)
    PARAMETER_ID(hpfFreq)
    PARAMETER_ID(hpfReso)
    PARAMETER_ID(hpfEnv)
    PARAMETER_ID(hpfLFO)
    PARAMETER_ID(hpfVelocity)
    PARAMETER_ID(hpfAttack)
    PARAMETER_ID(hpfDecay)
    PARAMETER_ID(hpfSustain)
    PARAMETER_ID(hpfRelease)
    PARAMETER_ID(envAttack)
    PARAMETER_ID(envDecay)
    PARAMETER_ID(envSustain)
    PARAMETER_ID(envRelease)
    PARAMETER_ID(lfoRate)
    PARAMETER_ID(vibrato)
    PARAMETER_ID(noise)
    PARAMETER_ID(octave)
    PARAMETER_ID(tuning)
    PARAMETER_ID(outputLevel)
    PARAMETER_ID(polyMode)

    #undef PARAMETER_ID
}

/**
 Represents the core processor of the plugin. The CppsynthAudioProcessor class extends the juce::AudioProcessor,
 which contains all audio processing classes and functions.
*/
class CppsynthAudioProcessor : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    // LRN {} after declaration does value initialization (call to constructor here)
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };
    
    // LRN Constructor
    CppsynthAudioProcessor();
    ~CppsynthAudioProcessor() override;

    // LRN Tells plug-in about sample rate, buffer size, allocates resources
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    
    // LRN Releases resources
    void releaseResources() override;
    
    void reset() override;

    // LRN #conditionals are preprocessor directives (before compilation)
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // LRN All audio processing is done here; process one block of sampleSize
    // LRN & after type is to signify pass-by-ref; not a pointer, not a copy,
    //  it's the object itself
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // LRN Info that will be queried by the host
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    Synth synth;
    
    // Parameters accessible to host
    juce::AudioParameterFloat* oscMixParam;
    juce::AudioParameterFloat* oscTuneParam;
    juce::AudioParameterFloat* oscFineParam;
    juce::AudioParameterChoice* glideModeParam;
    juce::AudioParameterFloat* glideRateParam;
    juce::AudioParameterFloat* glideBendParam;
    juce::AudioParameterFloat* filterFreqParam;
    juce::AudioParameterFloat* filterResoParam;
    juce::AudioParameterFloat* filterEnvParam;
    juce::AudioParameterFloat* filterLFOParam;
    juce::AudioParameterFloat* filterVelocityParam;
    juce::AudioParameterFloat* filterAttackParam;
    juce::AudioParameterFloat* filterDecayParam;
    juce::AudioParameterFloat* filterSustainParam;
    juce::AudioParameterFloat* filterReleaseParam;
    juce::AudioParameterFloat* hpfFreqParam;
    juce::AudioParameterFloat* hpfResoParam;
    juce::AudioParameterFloat* hpfEnvParam;
    juce::AudioParameterFloat* hpfLFOParam;
    juce::AudioParameterFloat* hpfVelocityParam;
    juce::AudioParameterFloat* hpfAttackParam;
    juce::AudioParameterFloat* hpfDecayParam;
    juce::AudioParameterFloat* hpfSustainParam;
    juce::AudioParameterFloat* hpfReleaseParam;
    juce::AudioParameterFloat* envAttackParam;
    juce::AudioParameterFloat* envDecayParam;
    juce::AudioParameterFloat* envSustainParam;
    juce::AudioParameterFloat* envReleaseParam;
    juce::AudioParameterFloat* lfoRateParam;
    juce::AudioParameterFloat* vibratoParam;
    juce::AudioParameterFloat* noiseParam;
    juce::AudioParameterFloat* octaveParam;
    juce::AudioParameterFloat* tuningParam;
    juce::AudioParameterFloat* outputLevelParam;
    juce::AudioParameterChoice* polyModeParam;
    
    // Atomic (thread-safe) flag to signal a parameter change
    std::atomic<bool> parametersChanged { false };

    /**
     Instanciate all audio parameters objects into the layout.
     */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    /**
     Splits a buffer into segments by the corresponding MIDI events (aligned with timestamps) in order to
     properly handle noteOn and noteOff events of a same MIDI source in a block
     */
    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    
    /**
     Handles the MIDI data : data0 contains event code, data1 and data2 contains event data
     */
    // LRN uint8_t is a cross-platform compatible unsigned int of 8 bytes
    void handleMidi(uint8_t data0, uint8_t data1, uint8_t data2);
    
    /**
     Renders the audio in buffer
     */
    void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);
    
    /**
     Listener function, called when a parameter is changed from a ValueTree.
     */
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;
    
    /**
     Function where the calculations are done after a parameter change.
     */
    void update();
    
    //==============================================================================
    // LRN Disable copy constructor (constructor that takes reference to other object of
    // same class) so the class cannot be copied
    // LRN Also enables JUCE built-in memory leak detector
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CppsynthAudioProcessor)
};
