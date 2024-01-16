/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

// LRN Skip current header if already included once (non-standard)
#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 LRN The CppsynthAudioProcessor class extends the juce::AudioProcessor, which contains all audio processing classes
 and functions.
*/
class CppsynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    // LRN Constructor
    CppsynthAudioProcessor();
    ~CppsynthAudioProcessor() override;

    //==============================================================================
    // LRN Tells plug-in about sample rate, buffer size, allocates resources
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    
    // LRN Releases resources
    void releaseResources() override;

    // LRN #conditionals are preprocessor directives (before compilation)
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // LRN All audio processing is done here
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    // LRN Info that will be queried by the host
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    // LRN Disable copy constructor (constructor that takes reference to other object of
    // same class) so the class cannot be copied
    // LRN Also enables JUCE built-in memory leak detector
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CppsynthAudioProcessor)
};
