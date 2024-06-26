/*
  ==============================================================================

    Synth.h
    Created: 22 Jan 2024 3:20:57pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

// LRN include <file.h> for std lib stuff or stuff in predefined directory,
//  "file.h" for programmer-defined stuff
#include <JuceHeader.h>
#include <stack>
#include "Constants.h"
#include "Voice.h"
#include "WhiteNoise.h"
#include "PinkNoise.h"

/**
 Represents the synthesizer as a whole. The synthesizer handles MIDI, renders audio.
 */
class Synth
{
public:
    float osc1Level, osc2Level, noiseLevel; // output levels
    float envAttack, envDecay, envSustain, envRelease; // envelope levels
    float tune; // synth's cents tuning
    float osc2detune; // overall tuning of OSC2 (semitones + cents)
    float osc1Morph, osc2Morph; // OSC1&2 shape
    float volumeTrim; // master output level trim
    float velocitySensitivity; // velocity sensitivity multiplier
    float lfoInc; // phase increment for LFO (between 0 and 2pi)
    float vibrato; // pitch LFO depth
    float modWheel; // modulation wheel value
    float vibratoMod; // vibrato modulation valye
//    float glideRate; // speed of glide
//    float glideBend; // adds a glide up or down before new notes
    // LPF values
    float lpfCutoff, lpfQ;
    float lpfLFODepth;
    float lpfAttack, lpfDecay, lpfSustain, lpfRelease;
    float lpfEnvDepth;
    // HPF valyes
    float hpfCutoff, hpfQ;
    float hpfLFODepth;
    float hpfAttack, hpfDecay, hpfSustain, hpfRelease;
    float hpfEnvDepth;
    int polyMode; // 0: Mono; 1: Poly;
//    int glideMode;
    int noiseType; // 0: White; 1: Pink
    bool ignoreVelocity; // velocity toggle
    bool ringMod; // ring mod toggle
    bool phaseRand; // phase randomizer toggle
    juce::LinearSmoothedValue<float> outputLevelSmoother; // smoother for output level
    
    Synth();
    
    /**
     Allocates memory for audio rendering.
     */
    void allocateResources(double sampleRate, int samplesPerBlock);
    
    /**
     Deallocate memory after usage finished.
     */
    void deallocateResources();
    
    /**
     Resets the state of the Synth instance.
     */
    void reset();
    
    /**
     Renders audio in outputBuffers, which point to the first and second channels.
     */
    void render(float** outputBuffers, int sampleCount);
    
    /**
     Parses and handles the MIDI message. First argument is the command byte.
     */
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
    
    /**
     Handles various MIDI CC messages.
     */
    void controlChange(uint8_t data1, uint8_t data2);
    
    /**
     Empties the held note list. Called when resetting synth or changing mono/poly modes.
     */
    void emptyHeldNotes();
    
    /**
     Adds a held note to the held note list.
     */
    void addHeldNote(int note);
    
    /**
     Removes a note from the held note list.
     */
    void removeHeldNote(int note);
    
    /**
     Returns the last held note.
     */
    int lastHeldNote();
    
    /**
     Checks if the held notes list is empty.
     */
    bool heldNotesEmpty();

private:
    int lfoStep; // counter from LFO max value to 0
//    int lastNote; // keep track of last note for glide
    int lastVelocity; // keep track of the velocity of the last held note
    /**
     The list of held notes in mono mode is represented as a list of 10 integers.
     The constant for no notes means no note is held, and any other number means the note is held.
     The last held note is the last number in the list that != the no note value constant
     Even though last note priority is a mono mode feature and only one voice is used,
     we still use the max voices constant, since 10 held notes is enough.
     */
    int heldNotesMono[constants::MAX_VOICES];
    float sampleRate; // sample rate taken from host
    float pitchBend; // pitch bend value
    float lfo; // current phase of LFO sine wave
    // zips hold the smoothed values for filter modulation factors
    float lpfZip;
    float hpfZip;
    bool sustainPressed; // sustain pressed toggle
    // LRN allocate arr size directly in std::array<Type, Size> arr;
    std::array<Voice, constants::MAX_VOICES> voices; // voices array
    WhiteNoise whiteNoise;
    PinkNoise pinkNoise;
    
    /**
     Handles the Note On command.
     */
    void noteOn(int note, int velocity);
    
    /**
     Handles the Note Off command.
     */
    void noteOff(int note);
        
    /**
     Starts a voice.
     */
    void startVoice(int voiceIndex, int note, int velocity);
        
    /**
     Finds a free voice to use for the next note played when all voices are in use. This will be the 
     quietest voice that is not in the attack stage. The index of the voice is returned.
     */
    int findFreeVoice() const;
    
    /**
     Updates the synth's LFO .
     */
    void updateLFO();
        
    /**
     Updates the oscillators frequency if the voice changes it (while gliding or pitch bending, for example).
     */
    void updateFreq(Voice& voice);
    
    /**
     Converts a MIDI note number to a frequency in hertz. Adds analog drift with voice index.
     */
    float midiNoteNumberToFreq(int midiNoteNumber, int voiceIndex);
    
    /**
     Helper method to determine if synth is being played in legato style.
     The synth is being played in legato style if, for a new noteOn event, at least one key
     must be held down for a previous note.
     */
//    bool isPlayingLegatoStyle() const;
};
