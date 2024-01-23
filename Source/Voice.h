/*
  ==============================================================================

    Voice.h
    Created: 22 Jan 2024 3:21:10pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once
#include "Constants.h"

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
    
    /**
     Resets the state of the voice instance.
     */
    void reset()
    {
        note = constants::noNoteValue;
        velocity = 0;
    }
};

