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
    float osc1Level, osc2Level, noiseLevel;
    float envAttack, envDecay, envSustain, envRelease;
    float tune;
    float osc2detune;
    float osc1Morph, osc2Morph;
    float volumeTrim;
    float velocitySensitivity;
    float lfoInc; // phase increment for LFO (between 0 and 2pi)
    float vibrato; // pitch LFO depth
    float modWheel;
    float vibratoMod;
    float glideRate; // speed of glide
    float glideBend; // adds a glide up or down before new notes
    float lpfCutoff, lpfQ;
    float lpfLFODepth;
    float lpfAttack, lpfDecay, lpfSustain, lpfRelease;
    float lpfEnvDepth;
    float hpfCutoff, hpfQ;
    float hpfLFODepth;
    float hpfAttack, hpfDecay, hpfSustain, hpfRelease;
    float hpfEnvDepth;
    int numVoices;
    int polyMode; // 0: Mono; 1: Poly;
    int glideMode;
    int noiseType; // 0: White; 1: Pink
    bool ignoreVelocity;
    bool ringMod;
    // TODO: apply smoothing technique to some other params as well (osc mix, etc.)
    juce::LinearSmoothedValue<float> outputLevelSmoother;
    
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
     Empties the held note stack. Useful when resetting synth or changing mono/poly modes.
     */
    void emptyHeldNotes();

private:
    int lfoStep; // counter from LFO max value to 0
    int lastNote; // keep track of last note for glide
    int lastVelocity;
    std::stack<int> heldNotesMono;
    float sampleRate;
    float pitchBend;
    float lfo; // current phase of LFO sine wave
    // will hold the smoothed versions of filters Mod value (to remove zipper noise)
    float lpfZip;
    float hpfZip;
    bool sustainPressed;
    // LRN allocate arr size directly in std::array<Type, Size> arr;
    std::array<Voice, constants::MAX_VOICES> voices;
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
