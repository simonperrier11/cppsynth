/*
  ==============================================================================

    Voice.h
    Created: 22 Jan 2024 3:21:10pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "WavetableOscillator.h"
#include "Envelope.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"
#include "WavetableGenerator.h"

/**
 Represents a voice for the synthesizer; produces the next output sample for a given note.
 The synthesizer can have multiple voices in polyphony.
 */
class Voice
{
public:
    int note; // the MIDI note number for the current voice
    float frequency; // the base frequency of the note played on OSC1 for this voice
    float velocityAmp;
    float osc1Level;
    float osc2Level;
    float target; // target for glide
    float glideRate; // copy of synth's glide rate
    bool ringMod;
    bool sustained;
    
    // Filters
    LowPassFilter lpf;
    HighPassFilter hpf;
    float lpfCutoff;
    float hpfCutoff;
    float lpfQ;
    float hpfQ;
    float lpfMod;
    float hpfMod;
    float lpfEnvDepth;
    float hpfEnvDepth;
    
    // OSC wavetables
    std::vector<WavetableOscillator> sineTableOsc1;
    std::vector<WavetableOscillator> triTableOsc1;
    std::vector<WavetableOscillator> squareTableOsc1;
    std::vector<WavetableOscillator> sawTableOsc1;
    
    std::vector<WavetableOscillator> sineTableOsc2;
    std::vector<WavetableOscillator> triTableOsc2;
    std::vector<WavetableOscillator> squareTableOsc2;
    std::vector<WavetableOscillator> sawTableOsc2;
    
    // Wavetables morph factor
    float osc1Morph;
    float osc2Morph;
    
    // Envelopes
    Envelope env;
    Envelope lpfEnv;
    Envelope hpfEnv;
    
    /**
     Resets the state of the voice instance and its components.
     */
    void reset();
    
    /**
     Triggers the release phase of the envelope.
     */
    void release();
    
    /**
      The core function of this class. Renders the next values of the oscillators. Also takes noise as input.
     */
    float render(float noise);
    
    /**
     Update various modulations on the voice according to modulation values from Synth.
     */
    void updateLFO();

    /**
     Returns the interpolated sample across the four wave shapes.
     Depending on factor value, interpolate between s1 and s2, s2 and s3 or s3 and s4.
     */
    float interpolatedSample(float factor, float s1, float s2, float s3, float s4);

    /**
     Initializes the wavetable oscillators to be used by this voice.
     This should only be called whenever the sample rate is set, or when it changes.
     */
    void initializeOscillators(float sampleRate);
    
    /**
     Sets the frequency for the wavetables at MIDI note index in each wavetable vectors.
     This is called on new notes.
     */
    void setFrequencyAtNote(int note, float freq);
    
    /**
     Modifies the frequency for the wavetables at MIDI note index in each wavetable vectors.
     This is done at each render of the synth's voice to catch any changes.
     */
    void modFrequencyAtNote(int note, float pitchBend, float vibratoMod, float osc2Detune);
};

