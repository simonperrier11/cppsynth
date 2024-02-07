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
#include "Envelope.h"

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
    float period; // TODO: the synth used in the book sets its pitch by its period, but I might want to change this
    SineWave sineOsc;
    SawtoothWave sawOsc;
    Envelope env;
    Blit osc1;
    Blit osc2;
    
    /**
     Resets the state of the voice instance and its components.
     */
    void reset()
    {
        note = constants::noNoteValue;
        //velocity = 0;
        saw = 0.0f;
        sineOsc.reset();
        sawOsc.reset();
        osc1.reset();
        osc2.reset();
        env.reset();
    }
    
    /**
     Triggers the release phase of the envelope.
     */
    void release()
    {
        env.release();
    }
    
    /**
      Renders the next value of the oscillator. This function also takes noise as input.
     */
    float render(float input)
    {
//        // Sine
//        return sineOsc.nextSample();
//        
//        // Saw (old)
//        return sawOsc.nextSample();
        
        // Saw (new)
        float sample1 = osc1.nextSample();
        float sample2 = osc2.nextSample();
        
        // BLIT attenuation for sawtooth wave
        saw = saw * 0.997f + (sample1 - sample2);
        
        float output = saw + input;
        float envelope = env.nextValue();
        
        // return envelope;
        return output * envelope;
    }
};

