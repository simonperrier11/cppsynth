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
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        voices[v].reset();
    }
    
    noiseGen.reset();
    
    // Default values for sytnh
    pitchBend = 1.0f;
    sustainPressed = false;
    outputLevelSmoother.reset(sampleRate, 0.05); // 50 msec
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    // Update some of the synth's params at each render to catch any change
    // to any voices that are currently playing
    // TODO: general tune and finetune should also always affect playing notes

    // Pitch and amplitude of OSC2
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        
        if (voice.env.isActive()) {
            voice.osc1.period = voice.period * pitchBend;
            voice.osc2.period = voice.osc1.period * osc2detune;
            voice.osc2.amplitude = voice.osc1.amplitude * oscMix;
        }
    }
        
    // For all the samples we need to render (sampleCount)...
    for (int sample = 0; sample < sampleCount; ++sample) {
        // Get next noise value
        const float noise = noiseGen.nextValue() * noiseMix;
        float outputLeft = 0.0f;
        float outputRight = 0.0f;
        
        // TODO: stereophony p.196

        for (int v = 0; v < constants::MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            
            if (voice.env.isActive()) {
                float output = voice.render(noise);
                outputLeft += output;
                outputRight += output;
            }
        }
        
        // Apply output level with smoothing
        float outputLevel = outputLevelSmoother.getNextValue();
        outputLeft *= outputLevel;
        outputRight *= outputLevel;

        // Write value in left and right buffers
        if (outputBufferRight != nullptr) {
            outputBufferLeft[sample] = outputLeft;
            outputBufferRight[sample] = outputRight;
        }
        else {
            // Mix both output in left buffer if mono
            outputBufferLeft[sample] = (outputLeft + outputRight) * 0.5f;
        }
    }
    
    // Reset envelope if done
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        
        if (!voice.env.isActive()) {
            voice.env.reset();
        }
    }

    // TODO: remove when done with dev, but add back when developing features!
    loudnessProtectBuffer(outputBufferLeft, sampleCount);
    loudnessProtectBuffer(outputBufferRight, sampleCount);
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
            noteOff(note);
            break;
        }
        case 0x90: { // Note On command code
            if (velocity > 0) {
                noteOn(note, velocity);
            }
            else {
                // Note On with no velocity is treated as Note Off
                // (running status optimization)
                noteOff(note);
            }
            break;
        }
        case 0xB0: {
            // CC message
            controlChange(data1, data2);
            break;
        }
        case 0xE0: {
            // Range of pitch bend is 2 semitones up and down
            // TODO: book p.189
            pitchBend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            break;
        }
    }
}

void Synth::startVoice(int v, int note, int velocity) {
    float period = calcPeriod(v, note);
    
    // Apply curve to velocity
//    // Curve recommended by MIDI Association
//     float velocityCurve = float(velocity * velocity) / 127.0f;
    // Custom curve with dynamic range -23dB - 0.72dB
    float velocityCurve = 0.004f * float((velocity + 64) * (velocity + 64)) - 8.0f;

    Voice& voice = voices[v];
    voice.period = period;
    voice.note = note;
    
    voice.osc1.amplitude = velocityCurve * volumeTrim;
    // TODO: reset on new note could be a param
    // voice.osc1.reset();
    // voice.osc2.reset();

    // LRN & to dereference voice.env to access it just by env variable
    // Envelope settings + trigger envelope
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
    env.attack();
}

void Synth::restartVoiceLegato(int note, int velocity) {
    float period = calcPeriod(0, note);
    
    Voice& voice = voices[0];
    
    voice.period = period;
    voice.env.level += constants::SILENCE_TRESHOLD + constants::SILENCE_TRESHOLD;
    voice.note = note;
}

void Synth::noteOn(int note, int velocity)
{
    // Disables velocity of note (force to 80)
    if (ignoreVelocity) {
        velocity = 80;
    }
    
    int v = 0; // Voice index
    
    if (numVoices == 1) { // Mono
        if (voices[0].note > 0) { // Legato
            restartVoiceLegato(note, velocity);
            return;
        }
    }
    else { // Poly
        v = findFreeVoice();
    }
    
    startVoice(v, note, velocity);
}

void Synth::noteOff(int note)
{
    // TODO: if noteOff velocity is to be implemented, use here

    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        if (voices[v].note == note) {
            if (sustainPressed) {
                voices[v].note = constants::SUSTAINED_NOTE_VALUE;
            }
            else {
                voices[v].release();
                voices[v].note = constants::NO_NOTE_VALUE;
            }
        }
    }
}

float Synth::calcPeriod(int v, int note) const
{
    // TODO: book p.186, p.199 for detune
    float period = tune * std::exp(-0.05776226505f * (float(note) + (constants::ANALOG_DRIFT * float(v))));
    // Ensure that the period or detuned pitch of OSC2 is at least 6 samples long,
    // else we double it. This is because the BLIT based oscillator may not work
    // too well if the perdio is too small
    while (period < 6.0f || (period * osc2detune) < 6.0f) { period += period; }
    return period;
}

int Synth::findFreeVoice() const
{
    int v = 0;
    float l = 100.0f; // Arbitrarily loud level
    
    // Find quietest voice that is not in attack stage
    for (int i = 0; i < constants::MAX_VOICES; ++i) {
        if (voices[i].env.level < l && !voices[i].env.isInAttack()) {
            l = voices[i].env.level;
            v = i;
        }
    }
    
    return v;
}

void Synth::controlChange(uint8_t data1, uint8_t data2)
{
    switch(data1) {
        case 0x40: {
            // Sustain pedal is pressed
            // Most sustain pedals send 0 for false and 127 for true
            sustainPressed = (data2 >= 64);
            
            // Sustain pedal is lifted
            if (!sustainPressed) {
                // Call noteOff on all previously sustained notes
                noteOff(constants::SUSTAINED_NOTE_VALUE);
            }
            break;
        }
        default: {
            // Anything with control ID >= 120 is treated as a PANIC command
            // Kill all voices and lift sustain
            if (data1 >= 0x78) {
                for (int v = 0; v < constants::MAX_VOICES; ++v) {
                    voices[v].reset();
                }
                sustainPressed = false;
            }
            break;
        }
    }
}
