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
    // By referencing this (pluginprocessor), the GUI is automatically made using the layout
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 800);
    return editor;
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

juce::AudioProcessorValueTreeState::ParameterLayout CppsynthAudioProcessor::createParameterLayout()
{
    // LRN lambda definition : auto foo = [](T bar, ...)
    // LRN int without name is unnamed parameter (not actually used in function but might be required 
    //  to be present by interface, stuff like that...)
    // Lambda to return an OSC mix % from a value
    auto oscMixStringFromValue = [](float value, int)
    {
        char s[16] = {0};
        // Print mix ratio of OSC1:OSC2
        snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value, 0.5f * value);
        return juce::String(s);
    };
    
    // Lambda to filter values under a treshold and return result as a String
    auto filterVelocityStringFromValue = [](float value, int)
    {
        if (value < -90.0f)
            return juce::String("OFF");
        else
            return juce::String(value);
    };
    
    // Lambda to transform LFO rate to formatted String
    auto lfoRateStringFromValue = [](float value, int)
    {
        // Mapping for an exponential curve (finer control on values < 0.5)
        float lfoHz = std::exp(7.0f * value - 4.0f);
        return juce::String(lfoHz, 3);
    };
    
    // Lambda to format vibrato/PWM value to String
    auto vibratoStringFromValue = [](float value, int)
    {
        // Vibrato and PWN are mutually exclusive
        // value under 0 is PWM amount, over 0 is vibrato amount
        if (value < 0.0f)
            return "PWM " + juce::String(-value, 1);
        else
            return juce::String(value, 1);
    };

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // LRN make_unique<T> constructs object of type T and wraps it in a unique_ptr
    // LRN a unique_ptr is a smart pointer that retains sole ownership of an object through a pointer and
    //  destroys that object when the unique_ptr goes out of scope (the AudioParameterChoice is then owned by
    //  the ParameterLayout instance)
    // LRN AudioParameterChoice has an ID, a label, choices and idx for default
    // LRN AudioParameterFloat/Choice store values in atomic variables, so they are thread-safe
    // Poly/Mono mode selection
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID::polyMode, 
                                                            "Polyphony",
                                                            juce::StringArray { "Mono", "Poly" },
                                                            1));
    
    // LRN AudioParameterFloat has an ID, a label, a range, a default value, and an attribute object that
    //  describes the label for the units
    // LRN juce::NormalisableRange<float> maps a range from 0.0 to 1.0
    // OSC tune in semitones
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscTune, 
                                                           "OSC1 Tune",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           -12.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("semi")));
    
    // LRN NormalisableRange can have a skew (4th param) (see doc) and we can say if it happens 
    //  at center (5th param)
    // OSC fine tuning in cents
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscFine, 
                                                           "OSC1 Finetune",
                                                           juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3f, true),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("cent")));
    
    // OSC Mix
    // Transform value to String for display at 5th parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscMix, 
                                                           "OSC Mix",
                                                           juce::NormalisableRange<float>(0.0f, 100.f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()
                                                            .withLabel("%")
                                                            .withStringFromValueFunction(oscMixStringFromValue)));
    
    // Glide Mode
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID::glideMode, 
                                                            "Glide Mode",
                                                            juce::StringArray { "Off", "Legato", "Always" },
                                                            0));

    // Glide Rate
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::glideRate, 
                                                           "Glide Rate",
                                                           juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
                                                           35.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Glide Bend (add additionnal glide to notes played)
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::glideBend, 
                                                           "Glide Bend",
                                                           juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("semi")));

    // Filter cutoff frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterFreq,
                                                           "Filter Freq",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
                                                           100.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Filter resonance
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterReso,
                                                           "Filter Reso",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           15.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Filter enveloppe amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterEnv,
                                                           "Filter Env",
                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                           50.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Filter LFO amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterLFO,
                                                           "Filter LFO",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));
    
    // Filter modulation velocity sensitivity amount, also OFF disables all velocity for amplitude
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterVelocity,
                                                           "Velocity",
                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()
                                                            .withLabel("%")
                                                            .withStringFromValueFunction(filterVelocityStringFromValue)));

    // Filter attack
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterAttack,
                                                           "Filter Attack",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Filter decay
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterDecay,
                                                           "Filter Decay",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           30.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Filter sustain
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterSustain,
                                                           "Filter Sustain",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Filter release
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::filterRelease,
                                                           "Filter Release",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           25.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Envelope attack
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envAttack,
                                                           "Env Attack",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Envelope decay
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envDecay,
                                                           "Env Decay",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           50.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Envelope sustain
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envSustain,
                                                           "Env Sustain",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           100.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Envelope release
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envRelease,
                                                           "Env Release",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           30.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LFO rate
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lfoRate,
                                                           "LFO Rate",
                                                           juce::NormalisableRange<float>(),
                                                           0.81f,
                                                           juce::AudioParameterFloatAttributes()
                                                            .withLabel("Hz")
                                                            .withStringFromValueFunction(lfoRateStringFromValue)));

    // Vibrato or PWM amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::vibrato,
                                                           "Vibrato/PWM",
                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()
                                                            .withLabel("%")
                                                            .withStringFromValueFunction(vibratoStringFromValue)));

    // Noise mix
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::noise,
                                                           "Noise",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Master tune
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::octave,
                                                           "Octave",
                                                           juce::NormalisableRange<float>(-2.0f, 2.0f, 1.0f),
                                                           0.0f));

    // Master finetune
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::tuning,
                                                           "Tuning",
                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("cent")));

    // Master volume
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::outputLevel,
                                                           "Output Level",
                                                           juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("dB")));
    
    return layout;
}
