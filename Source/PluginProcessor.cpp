/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

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
    // Assign each identified parameter in the APVTS to a variable
    castJuceParameter(apvts, ParameterID::osc1Level, osc1LevelParam);
    castJuceParameter(apvts, ParameterID::osc2Level, osc2LevelParam);
    castJuceParameter(apvts, ParameterID::noiseLevel, noiseLevelParam);
    castJuceParameter(apvts, ParameterID::oscTune, oscTuneParam);
    castJuceParameter(apvts, ParameterID::oscFine, oscFineParam);
    castJuceParameter(apvts, ParameterID::osc1Morph, osc1MorphParam);
    castJuceParameter(apvts, ParameterID::osc2Morph, osc2MorphParam);
//    castJuceParameter(apvts, ParameterID::glideMode, glideModeParam);
//    castJuceParameter(apvts, ParameterID::glideRate, glideRateParam);
//    castJuceParameter(apvts, ParameterID::glideBend, glideBendParam);
    castJuceParameter(apvts, ParameterID::lpfFreq, lpfFreqParam);
    castJuceParameter(apvts, ParameterID::lpfReso, lpfResoParam);
    castJuceParameter(apvts, ParameterID::lpfEnv, lpfEnvParam);
    castJuceParameter(apvts, ParameterID::lpfLFO, lpfLFOParam);
    // castJuceParameter(apvts, ParameterID::lpfVelocity, lpfVelocityParam);
    castJuceParameter(apvts, ParameterID::lpfAttack, lpfAttackParam);
    castJuceParameter(apvts, ParameterID::lpfDecay, lpfDecayParam);
    castJuceParameter(apvts, ParameterID::lpfSustain, lpfSustainParam);
    castJuceParameter(apvts, ParameterID::lpfRelease, lpfReleaseParam);
    castJuceParameter(apvts, ParameterID::hpfFreq, hpfFreqParam);
    castJuceParameter(apvts, ParameterID::hpfReso, hpfResoParam);
    castJuceParameter(apvts, ParameterID::hpfEnv, hpfEnvParam);
    castJuceParameter(apvts, ParameterID::hpfLFO, hpfLFOParam);
    // castJuceParameter(apvts, ParameterID::hpfVelocity, hpfVelocityParam);
    castJuceParameter(apvts, ParameterID::hpfAttack, hpfAttackParam);
    castJuceParameter(apvts, ParameterID::hpfDecay, hpfDecayParam);
    castJuceParameter(apvts, ParameterID::hpfSustain, hpfSustainParam);
    castJuceParameter(apvts, ParameterID::hpfRelease, hpfReleaseParam);
    castJuceParameter(apvts, ParameterID::envAttack, envAttackParam);
    castJuceParameter(apvts, ParameterID::envDecay, envDecayParam);
    castJuceParameter(apvts, ParameterID::envSustain, envSustainParam);
    castJuceParameter(apvts, ParameterID::envRelease, envReleaseParam);
    castJuceParameter(apvts, ParameterID::lfoRate, lfoRateParam);
    castJuceParameter(apvts, ParameterID::vibrato, vibratoParam);
    castJuceParameter(apvts, ParameterID::octave, octaveParam);
    castJuceParameter(apvts, ParameterID::tuning, tuningParam);
    castJuceParameter(apvts, ParameterID::outputLevel, outputLevelParam);
    castJuceParameter(apvts, ParameterID::polyMode, polyModeParam);
    castJuceParameter(apvts, ParameterID::velocitySensitivity, velocitySensitivityParam);
    castJuceParameter(apvts, ParameterID::noiseType, noiseTypeParam);
    castJuceParameter(apvts, ParameterID::ringMod, ringModParam);
    
    // Add listener for parameter changes
    apvts.state.addListener(this);
}

// LRN ~ before constructor name is destructor
CppsynthAudioProcessor::~CppsynthAudioProcessor()
{
    apvts.state.removeListener(this);
}

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

void CppsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Pass sample rate to synth
    synth.allocateResources(sampleRate, samplesPerBlock);
    parametersChanged.store(true); // force update() to be executed
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
    
    // Assign initial (without ramp) and next target values to smoother
    synth.outputLevelSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(outputLevelParam->get()));
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CppsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Support mono and stereo buses layout only
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

    // In case we have more outputs than inputs, clear any empty output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Atomically check if parametersChanged is equal to expected, then set back to false
    // If DAW is in "offline mode" we always update
    bool expected = true;
    if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        update(); // Update synth if any changes
    }
    
    // Split buffer in segments by MIDI events, and process them
    splitBufferByEvents(buffer, midiMessages);
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
}

void CppsynthAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    // Create array of 2 float* pointers, one for the left channel, the other
    // for the right channel; the synth will write in those
    float* outputBuffers[2] = { nullptr, nullptr };
    
    // Get a WRITE pointer to the audio data inside the AudioBuffer object (channel 0)
    // Because the AudioBuffer is "split" and rendered based on the timestamps of the MIDI events,
    //  add bufferOffset
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    
    synth.render(outputBuffers, sampleCount);
}

bool CppsynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* CppsynthAudioProcessor::createEditor()
{
    // Generic JUCE UI
    // // By referencing *this (pluginprocessor), the GUI is automatically made using the layout
    // // created with createParameterLayout()
    // auto editor = new juce::GenericAudioProcessorEditor(*this);
    // editor->setSize(500, 800);
    // return editor;
    
    // PluginEditor UI
    return new CppsynthAudioProcessorEditor(*this);
}

void CppsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save parameters as XML to memory block
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void CppsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore parameters
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
        parametersChanged.store(true);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    // This creates new instances of the plugin
    return new CppsynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout CppsynthAudioProcessor::createParameterLayout()
{
    // LRN lambda definition : auto foo = [](T bar, ...)
    // LRN int without name is unnamed parameter (not actually used in function but might be required 
    //  to be present by interface, stuff like that...)
//    // Lambda to filter values under a treshold and return result as a String
//    auto filterVelocityStringFromValue = [](float value, int)
//    {
//        if (value < -90.0f)
//            return juce::String("OFF");
//        else
//            return juce::String(value);
//    };
    
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
        
    // Velocity sensitivity toggle
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID::velocitySensitivity,
                                                            "Velocity Sensitivity",
                                                            juce::StringArray { "Off", "On" },
                                                            1));
    
    // Velocity sensitivity toggle
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID::ringMod,
                                                            "Ring Mod",
                                                            juce::StringArray { "Off", "On" },
                                                            0));
    
    // OSC tune in semitones
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscTune, 
                                                           "OSC2 Semitones",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           -12.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("semi")));
    
    // LRN NormalisableRange can have a skew (4th param) (see doc) and we can say if it happens 
    //  at center (5th param)
    // OSC fine tuning in cents
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscFine, 
                                                           "OSC2 Tune",
                                                           juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3f, true),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("cent")));
        
    // OSC1 Level
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::osc1Level,
                                                           "OSC1 Level",
                                                           juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
                                                           100.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // OSC2 Level
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::osc2Level,
                                                           "OSC2 Level",
                                                           juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));
    
    // OSC1 Morph
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::osc1Morph,
                                                           "OSC1 Shape",
                                                           juce::NormalisableRange<float>(0.f, 0.999f, 0.001f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()));
    
    // OSC2 Morph
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::osc2Morph,
                                                           "OSC2 Shape",
                                                           juce::NormalisableRange<float>(0.f, 0.999f, 0.001f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()));

    // Noise type
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID::noiseType,
                                                            "Noise Type",
                                                            juce::StringArray { "White", "Pink" },
                                                            0));

    // Noise Level
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::noiseLevel,
                                                           "Noise Level",
                                                           juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

//    // Glide Mode
//    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID::glideMode, 
//                                                            "Glide Mode",
//                                                            juce::StringArray { "Off", "Legato", "Always" },
//                                                            0));
//
//    // Glide Rate
//    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::glideRate, 
//                                                           "Glide Rate",
//                                                           juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
//                                                           0.0f,
//                                                           juce::AudioParameterFloatAttributes().withLabel("%")));
//
//    // Glide Bend (add additionnal glide to notes played)
//    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::glideBend, 
//                                                           "Glide Bend",
//                                                           juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
//                                                           0.0f,
//                                                           juce::AudioParameterFloatAttributes().withLabel("semi")));


//    // Filter modulation velocity sensitivity amount, also OFF disables all velocity for amplitude
//    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfVelocity,
//                                                           "Velocity",
//                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
//                                                           0.0f,
//                                                           juce::AudioParameterFloatAttributes()
//                                                            .withLabel("%")
//                                                            .withStringFromValueFunction(lpfVelocityStringFromValue)));
    
    // Envelope attack
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envAttack,
                                                           "Env Attack",
                                                           juce::NormalisableRange<float>(0.0f, 10000.0f, 0.01f, 0.3f),
                                                           0.0f,
                                                           "ms"));

    // Envelope decay
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envDecay,
                                                           "Env Decay",
                                                           juce::NormalisableRange<float>(0.0f, 10000.0f, 0.01f, 0.3f),
                                                           0.0f,
                                                           "ms"));

    // Envelope sustain
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envSustain,
                                                           "Env Sustain",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           100.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // Envelope release
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::envRelease,
                                                           "Env Release",
                                                           juce::NormalisableRange<float>(0.0f, 10000.0f, 0.01f, 0.3f),
                                                           0.0f,
                                                           "ms"));

    // Low-pass filter cutoff frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfFreq,
                                                           "LPF Cutoff",
                                                           juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.5f, false),
                                                           20000.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // LPF resonance
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfReso,
                                                           "LPF Q",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LPF envelope attack
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfAttack,
                                                           "LPF Attack",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LPF ENV decay
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfDecay,
                                                           "LPF Decay",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LPF ENV sustain
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfSustain,
                                                           "LPF Sustain",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           100.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LPF ENV release
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfRelease,
                                                           "LPF Release",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LPF ENV amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfEnv,
                                                           "LPF Env Depth",
                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));
    
    // High-pass filter cutoff frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfFreq,
                                                           "HPF Cutoff",
                                                           juce::NormalisableRange<float>(0.0f, 20000.0f, 0.1f, 0.5f, false),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // HPF resonance
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfReso,
                                                           "HPF Q",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // HPF envelope attack
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfAttack,
                                                           "HPF Attack",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // HPF ENV decay
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfDecay,
                                                           "HPF Decay",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // HPF ENV sustain
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfSustain,
                                                           "HPF Sustain",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           100.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // HPF ENV release
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfRelease,
                                                           "HPF Release",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // HPF ENV amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfEnv,
                                                           "HPF Env Depth",
                                                           juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));

    // LFO rate
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lfoRate,
                                                           "LFO Rate",
                                                           juce::NormalisableRange<float>(),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()
                                                            .withLabel("Hz")
                                                            .withStringFromValueFunction(lfoRateStringFromValue)));

    // LFO depth for pitch
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::vibrato,
                                                           "LFO Depth (Pitch)",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 0.5f, false),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()
                                                            .withLabel("%")
                                                            .withStringFromValueFunction(vibratoStringFromValue)));

    // Filter LFO amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::lpfLFO,
                                                           "LFO Depth (LPF Cutoff)",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("%")));
    
    // Filter LFO amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::hpfLFO,
                                                           "LFO Depth (HPF Cutoff)",
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
                                                           -3.0f,
                                                           juce::AudioParameterFloatAttributes().withLabel("dB")));
    
    return layout;
}

void CppsynthAudioProcessor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    // DBG("parameter changed");
    parametersChanged.store(true);
}

void CppsynthAudioProcessor::update()
{
    float sampleRate = float(getSampleRate());
    float inverseSampleRate = 1.0f / sampleRate;
    
    // Lower update rate for some stuff that does not need to be calculated as frequently
    const float inverseUpdateRate = inverseSampleRate * constants::LOWER_UPDATE_RATE_MAX_VALUE;
    
    // LRN -> is like ., except it dereferences a pointer first; foo.bar() calls method bar() on object foo,
    //  foo->bar calls method bar on the object pointed to by pointer foo
    // Envelope attack
    float envAttackTimeMs = envAttackParam->get();
    // Add some samples of attack to prevent pop
    float envAttackSamples = (sampleRate * envAttackTimeMs / 100) + constants::POP_PREVENT_SAMPLES;

    // The multiplier for the one-pole filter is passed as the envAttack param for the synth
    synth.envAttack = std::exp(std::log(constants::SILENCE_TRESHOLD) / envAttackSamples);
    
    // Sutain is simply percentage between 0.0 and 1.0
    synth.envSustain = envSustainParam->get() / 100.0f;
    
    // Envelope decay
    float envDecayTimeMs = envDecayParam->get();
    float envDecaySamples = sampleRate * envDecayTimeMs / 1000;
    synth.envDecay = std::exp(std::log(constants::SILENCE_TRESHOLD) / envDecaySamples);
    
    // Envelope release
    float envReleaseTimeMs = envReleaseParam->get();
    float envReleaseSamples = (sampleRate * envReleaseTimeMs / 1000) + constants::POP_PREVENT_SAMPLES;
    synth.envRelease = std::exp(std::log(constants::SILENCE_TRESHOLD) / envReleaseSamples);

    // OSCs & noise levels
    synth.osc1Level = osc1LevelParam->get() / 100.0f;
    synth.osc2Level = osc2LevelParam->get() / 100.0f;
    synth.noiseLevel = noiseLevelParam->get() / 100.0f * 0.06f; // Heavily reduce noise cause it's so loud
    
    // OSC2 tuning
    float semi = oscTuneParam->get();
    float cent = oscFineParam->get();
    
    // To calculate pitch of any note with starting pitch, we use pitch * 2^(N/12)
    //  where N is the number of fractionnal semitones
    // add tiny little offset to prevent both osc from cancelling each other
    synth.osc2detune = std::pow(2.0f, (semi + 0.01f * cent) / 12.0f) + 0.00001;
    
    // Tuning
    float octave = octaveParam->get();
    float tuning = tuningParam->get();
    synth.tune = octave * 12.0f + tuning / 100.0f;
    
    // OSC morph
    synth.osc1Morph = osc1MorphParam->get();
    synth.osc2Morph = osc2MorphParam->get();
        
    // Mono/unisson/poly mode
    synth.polyMode = polyModeParam->getIndex();
    
    // Noise type
    synth.noiseType = noiseTypeParam->getIndex();
    
    // Filter cutoff frequency
    synth.lpfCutoff = lpfFreqParam->get();
    synth.hpfCutoff = hpfFreqParam->get();
    
    // Filter Q
    // create an exponential curve that starts at filterQ = 1 and goes up to filterQ = 20
    float lpfReso = lpfResoParam->get() / 100.0f;
    synth.lpfQ = std::exp(3.0f * lpfReso);
    
    float hpfReso = hpfResoParam->get() / 100.0f;
    synth.hpfQ = std::exp(3.0f * hpfReso);

    // Volume
    synth.volumeTrim = 0.0008f * (3.2f - 25.0f * synth.noiseLevel) * (1.5f - 0.5f * lpfReso);
    synth.outputLevelSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputLevelParam->get()));
    
    // Filter velocity
//    float lpfVelocity = lpfVelocityParam->get();
//    if (lpfVelocity < -90.0f) {
//        synth.velocitySensitivity = 0.0f;
//        synth.ignoreVelocity = true;
//    }
//    else {
//        synth.velocitySensitivity = 0.0005f * lpfVelocity;
//        synth.ignoreVelocity = false;
//    }
    
    // Velocity sensitivity toggle
    synth.ignoreVelocity = (velocitySensitivityParam->getIndex() == 0 ? true : false);
    
    // Ring mod
    synth.ringMod = (ringModParam->getIndex() == 0 ? false : true);
    
    // LFO
    // Skew parameter value to 0.02Hz-20Hz approx.
    float lfoRate = std::exp(7.0f * lfoRateParam->get() - 4.0f);
    synth.lfoInc = lfoRate * inverseUpdateRate * float(constants::TWO_PI);
    
    // Vibrato (LFO depth)
    // Divide by 110.0 to prevent sample being too high
    float vibrato = vibratoParam->get() / 120.0f;
    synth.vibrato = vibrato;
    
//    // Glide
//    synth.glideMode = glideModeParam->getIndex();
    
//    float glideRate = glideRateParam->get();
//    if (glideRate < 2.0f) {
//        synth.glideRate = 1.0f; // No glide
//    }
//    else {
//        synth.glideRate = 1.0f - std::exp(-inverseUpdateRate * std::exp(6.0f - 0.07f * glideRate));
//    }
//    
//    synth.glideBend = glideBendParam->get();
    
    // Filter LFO depth
    float lpfLFO = lpfLFOParam->get() / 100.0f;
    synth.lpfLFODepth = 2.5f * lpfLFO * lpfLFO; // Parabolic curve from 0 to 2.5
    
    float hpfLFO = hpfLFOParam->get() / 100.0f;
    synth.hpfLFODepth = 2.5f * hpfLFO * hpfLFO;
    
    // Filters envelopes
    synth.lpfAttack = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * lpfAttackParam->get()));
    synth.lpfDecay = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * lpfDecayParam->get()));
    float lpfSustain = lpfSustainParam->get() / 100.0f;
    synth.lpfSustain = lpfSustain * lpfSustain; // square sustain to skew param
    synth.lpfRelease = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * lpfReleaseParam->get()));

    synth.lpfEnvDepth = 0.06f * lpfEnvParam->get(); // env depth between -6.0 and 6.0
    
    synth.hpfAttack = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * hpfAttackParam->get()));
    synth.hpfDecay = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * hpfDecayParam->get()));
    float hpfSustain = hpfSustainParam->get() / 100.0f;
    synth.hpfSustain = hpfSustain * hpfSustain;
    synth.hpfRelease = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * hpfReleaseParam->get()));

    synth.hpfEnvDepth = 0.06f * hpfEnvParam->get();
}
