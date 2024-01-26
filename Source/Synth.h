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
    
private:
    float sampleRate;
    Voice voice;
    NoiseGenerator noiseGen;
    
    /**
     Handles the Note On command.
     */
    void noteOn(int note, int velocity);
    
    /**
     Handles the Note Off command.
     */
    void noteOff(int note, int velocity);
};
