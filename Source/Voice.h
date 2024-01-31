/*
  ==============================================================================

    Voice.h
    Created: 22 Jan 2024 3:21:10pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once
#include "Constants.h"
#include "SineWave.h"
#include "SawtoothWave.h"
#include "Blit.h"

// LRN we can use a struct instead of a class when we don't need private/protected
//  and other inheritance shenanigans (class defaults to private, struct to public)
/**
 Represents a voice for the synthesizer; produces the next output sample for a given note.
 The synthesizer can have multiple voices in polyphony.
 */
struct Voice
{
    int note;
    int velocity;
    float saw;
    Blit blit;
    SineWave sineOsc;
    SawtoothWave sawOsc;
    
    /**
     Resets the state of the voice instance.
     */
    void reset()
    {
        note = constants::noNoteValue;
        //velocity = 0;
        saw = 0.0f;
        sineOsc.reset();
        sawOsc.reset();
        blit.reset();
    }
    
    /**
      Renders the next value of the oscillator.
     */
    float render()
    {
//        // Sine
//        return sineOsc.nextSample();
//        
//        // Saw (old)
//        return sawOsc.nextSample();
        
        // Saw (new)
        float sample = blit.nextSample();
        saw = saw * 0.997f + sample;
        return saw;
    }
};

