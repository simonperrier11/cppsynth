/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// LRN the syntax "constructor() : member1(value1), member2(value2), ..." is
//  called member initialization; allows to init members of class with values
//  Here the synth has a parent class in which we may want to specify the constructor
//  used (defined by the preproc conditions)
CppsynthAudioProcessor::CppsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

// LRN ~ before constructor name is destructor
CppsynthAudioProcessor::~CppsynthAudioProcessor()
{
}

//==============================================================================
const juce::String CppsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CppsynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CppsynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CppsynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CppsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CppsynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CppsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CppsynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CppsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void CppsynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CppsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Allocate memory needed for Synth, then reset
    synth.allocateResources(sampleRate, samplesPerBlock);
    reset();
}

void CppsynthAudioProcessor::releaseResources()
{
    // Free memory used by Synth
    synth.deallocateResources();
}

void CppsynthAudioProcessor::reset()
{
    synth.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CppsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    
    // LRN this currently means that it will return false (not supported) unless
    //  we are in mono or stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// LRN buffer is where the synth will place audio samples it generates (output)
// LRN midiMessages are incoming MIDI msg
void CppsynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // LRN sets a special CPU flag for the duration of function that will
    //  automatically truncate floating point numbers to zero instead
    //  of turning them into denormal numbers (since denormal numbers are
    //  very slow; they are very close to 0 so we consider them to be 0)
    juce::ScopedNoDenormals noDenormals;
    
    // LRN auto keyword automatically detects and assigns a data type to the
    //  variable (compiler looks at its initialization; here it's int, the return
    //  type of getTotalNumInputChannels())
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Split buffer in segments by MIDI events, and process them
    splitBufferByEvents(buffer, midiMessages);
    
//    // This is the place where you'd normally do the guts of your plugin's
//    // audio processing...
//    // Make sure to reset the state if your inner loop is processing
//    // the samples and the outer loop is handling the channels.
//    // Alternatively, you can process the samples with the channels
//    // interleaved by keeping the same state.
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//    }
}

void CppsynthAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Offset in samples
    int bufferOffset = 0;
    
    // Iterate through midiMessages
    for (const auto metadata : midiMessages) {
        // LRN the status byte is in metadata.data[0], the data bytes are in
        //  metadata.data[1] and metadata.data[2], the timestampe is in
        //  metadata.samplePosition (nb of samples relative to the start of the AudioBuffer)
        
        // Render audio up to the event's timestamp (calculated in samples), if any
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            
            // Increment offset by nb of samples for this segment
            bufferOffset += samplesThisSegment;
        }
        
        // Handle event (ignore MIDI sysex messages, which have more than 3 bytes)
        if (metadata.numBytes <= 3) {
            // Get data bytes if they exist
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            
            // Status byte is in metadata.data[0]
            handleMidi(metadata.data[0], data1, data2);
        }
    }
    
    // Render audio after last MIDI event, if any; if there were no events, the whole
    // buffer is rendered
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment, bufferOffset);
    }
    
    // Let JUCE know all MIDI events have been processed
    midiMessages.clear();
}

void CppsynthAudioProcessor::handleMidi(uint8_t data0, uint8_t data1, uint8_t data2)
{
    // Pass the MIDI msg to Synth
    synth.midiMessage(data0, data1, data2);

//    // Debug code
//    char s[16];
//    // Write formatted output to sized buffer, instead of directly printing it
//    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
//    DBG(s);
}

void CppsynthAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    // Create array of 2 float* pointers, one for the left channel, the other
    // for the right channel
    float* outputBuffers[2] = { nullptr, nullptr };
    
    // Get a WRITE pointer to the audio data inside the AudioBuffer object (channel 0)
    // Because the AudioBuffer is "split" and rendered based on the
    // timestamps of the MIDI events, add bufferOffset
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    
    synth.render(outputBuffers, sampleCount);
}

//==============================================================================
bool CppsynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CppsynthAudioProcessor::createEditor()
{
    return new CppsynthAudioProcessorEditor (*this);
}

//==============================================================================
void CppsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CppsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CppsynthAudioProcessor();
}

