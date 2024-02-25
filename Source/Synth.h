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
#include "Constants.h"
#include "Voice.h"
#include "NoiseGenerator.h"

/**
 Represents the synthesizer as a whole. The synthesizer handles MIDI, renders audio.
 */
class Synth
{
public:
    float noiseMix;
    float oscMix;
    float envAttack;
    float envDecay;
    float envSustain;
    float envRelease;
    float tune;
    float osc2detune;
    float volumeTrim;
    float velocitySensitivity;
    float lfoInc; // phase increment for LFO (between 0 and 2pi)
    float vibrato; // pitch LFO depth
    float modWheel;
    float glideRate;
    float glideBend;
    float filterCutoff;
    float filterQ;
    int numVoices;
    int glideMode;
    bool ignoreVelocity;
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
    
private:
    int lfoStep; // counter from LFO max value to 0
    int lastNote; // keep track of last note for glide
    float sampleRate;
    float pitchBend;
    float lfo; // current phase of LFO sine wave
    bool sustainPressed;
    // LRN allocate arr size directly in std::array<Type, Size> arr;
    std::array<Voice, constants::MAX_VOICES> voices;
    NoiseGenerator noiseGen;
    
    /**
     Handles the Note On command.
     */
    void noteOn(int note, int velocity);
    
    /**
     Handles the Note Off command.
     */
    void noteOff(int note);
    
    // LRN const after function guarantees that the function will
    //  not change the object it is invoked on (its members)
    /**
     Calculates and returns the period given a MIDI note.
     */
    float calcPeriod(int v, int note) const;
    
    /**
     Starts a voice.
     */
    void startVoice(int v, int note, int velocity);
    
    /**
     When playing legato in mono mode, this is used when pressing a new note.
     */
    void restartVoiceLegato(int note, int velocity);
    
    /**
     Finds a free voice to use for the next note played when all voices are in use. This will be the 
     quietest voice that is not in the attack stage. The index of the voice is returned.
     */
    int findFreeVoice() const;
    
    /**
     Updates the synth's LFO .
     */
    void updateLfo();
    
    /**
     Updates the oscillators period if the voice changes it (while gliding, for example).
     */
    void updatePeriod(Voice& voice);
    
    /**
     Helper method to determine if synth is being played in legato style.
     The synth is being played in legato style if, for a new noteOn event, at least one key
     must be held down for a previous note.
     */
    bool isPlayingLegatoStyle() const;

};
