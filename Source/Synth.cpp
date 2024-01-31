/*
  ==============================================================================

    Synth.cpp
    Created: 22 Jan 2024 3:20:57pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "Synth.h"
#include "Utils.h"

Synth::Synth()
{
    // Default sample rate to 44.1Hz
    sampleRate = 44100.0f;
}

// LRN trailing _ here used to distinguish with private member sampleRate
void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/) {
    // LRN static_cast has more compile-time checks than regular cast, and is safer
    sampleRate = static_cast<float>(sampleRate_);
}

void Synth::deallocateResources()
{
    // TODO
}

void Synth::reset()
{
    voice.reset();
    noiseGen.reset();
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    // For all the samples we need to render (sampleCount)...
    for (int sample = 0; sample < sampleCount; ++sample) {
        // Get next noise value
        float noise = noiseGen.nextValue();
        float output = 0.0f;
        
        if (voice.note > 0) {
            // Multiply noise by velocity (which is divided by 127),
            // 6dB reduction (* 0.5), then put in output
//            output = noise * (voice.velocity / 127.0f) * 0.5f;
            
            output = voice.render();
        }
        
        // Write value in buffers
        outputBufferLeft[sample] = output;
        if (outputBufferRight != nullptr) {
            outputBufferRight[sample] = output;
        }
    }
    
    // TODO put this back!
//    loudnessProtectBuffer(outputBufferLeft, sampleCount);
//    loudnessProtectBuffer(outputBufferRight, sampleCount);
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    // The status byte (data0) has 2 parts: command (first 4 bits) and
    // channel (last 4 bits). We simply need to do a binary AND with 11110000
    // and 00001111 to get both parts respectively
    uint8_t command = data0 & 0xF0;
    uint8_t channel = data0 & 0x0F;
    
    // Force set values to 0-127 range by doing binary AND, just in case
    uint8_t note = data1 & 0x7F;
    uint8_t velocity = data2 & 0x7F;

    switch (command) {
        case 0x80: { // Note Off command code
            noteOff(note, velocity);
            break;
        }
        case 0x90: { // Note On command code
            if (velocity > 0) {
                noteOn(note, velocity);
            }
            else {
                // Note On with no velocity is treated as Note Off
                // (running status optimization)
                noteOff(note, velocity);
            }
            break;
        }
    }
}

void Synth::noteOn(int note, int velocity)
{
    voice.note = note;
    //voice.velocity = velocity;
    
    // Convert MIDI note to frequency
    // The formula is, with base frequency 440Hz : frequency = 440 × 2^(N/12)
    // eg.: 400 x 2^(-1/12) is one semitone down from A, 400 x 2^(2/12) is a tone up from A
    // N = (note - 69) to get the number of semitones difference with 440Hz A (MIDI #69)
    float freq = 440.0f * std::exp2(float(note - 69) / 12.0f);
    
    // Sine
    voice.sineOsc.amplitude = (velocity / 127.0f) * 0.5f;
//    voice.osc.freq = freq;
//    voice.osc.sampleRate = sampleRate;
//    voice.osc.phaseOffset = 0.0f;
    voice.sineOsc.increment = freq / sampleRate;
    voice.sineOsc.reset();
    
    // Saw (old)
    voice.sawOsc.amplitude = (velocity / 127.0f) * 0.5f;
    voice.sawOsc.increment = freq / sampleRate;
    voice.sawOsc.freq = freq;
    voice.sawOsc.sampleRate = sampleRate;
    voice.sawOsc.reset();
    
    // Saw (new)
    voice.blit.amplitude = (velocity / 127.0f) * 0.5f;
    voice.blit.period = sampleRate / freq;
    voice.blit.reset();
}

void Synth::noteOff(int note, int velocity)
{
    // TODO if noteOff velocity is to be implemented, use here
    if (voice.note == note) {
        voice.note = constants::noNoteValue;
        //voice.velocity = 0;
    }
}
