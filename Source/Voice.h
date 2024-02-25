/*
  ==============================================================================

    Voice.h
    Created: 22 Jan 2024 3:21:10pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once
#include "Constants.h"
#include "Blit.h"
#include "Envelope.h"
#include "Filter.h"

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
    float target; // target for glide
    float glideRate; // copy of synth's glide rate
    Envelope env;
    Blit osc1;
    Blit osc2;
    Filter filter;
    
    /**
     Resets the state of the voice instance and its components.
     */
    void reset()
    {
        note = constants::NO_NOTE_VALUE;
        //velocity = 0;
        saw = 0.0f;
        osc1.reset();
        osc2.reset();
        env.reset();
        filter.reset();
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
        // Get next samples for both BLIT oscillators
        float sample1 = osc1.nextSample();
        float sample2 = osc2.nextSample();
        
        // Substract osc2 from osc1, and multiply by saw with attenation
        //  to create shape
        saw = saw * 0.997f + (sample1 - sample2);
        
        // Mix with input (noise)
        float output = saw + input;
        
        // Apply filter
        output = filter.render(output);
        
        // Apply enveloppe
        float envelope = env.nextValue();
        return output * envelope;
    }
    
    /**
     Exponential one-pole filter formula to calculate glide between notes.
     Also updates coefficiants of filter for voice.
     */
    float updateLFO()
    {
        period += glideRate * (target - period);
        filter.updateCoefficiants(1000.0f, 0.707f);
    }
};

