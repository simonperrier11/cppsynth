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
    
    // Give sampleRate to voices filters to calculate coefficiants
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        voices[v].lpf.sampleRate = sampleRate;
        voices[v].hpf.sampleRate = sampleRate;
    }
}

void Synth::deallocateResources()
{
    // TODO
}

void Synth::reset()
{
    // Reset voices
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        voices[v].reset();
    }
    
    // Reset noise generators
    whiteNoise.reset();
    pinkNoise.reset();
    
    // Reset default values for sytnh
    pitchBend = 1.0f;
    sustainPressed = false;
    outputLevelSmoother.reset(sampleRate, 0.05); // 50 msec
    lfo = 0.0f;
    lfoStep = 0;
    modWheel = 0;
    lastNote = 0;
    lpfZip = 0;
    hpfZip = 0;
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    // Update some of the synth's currently playing voices to catch param changes
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        
        if (voice.env.isActive()) {
            // TODO: general tune and finetune, OSC and noise levels here

            // Glide might affect current period value
            updatePeriod(voice);
            voice.glideRate = glideRate;
            
            // Filter values
            voice.lpfCutoff = lpfCutoff;
            voice.lpfQ = lpfQ;
            voice.lpfEnvDepth = lpfEnvDepth;
            
            voice.hpfCutoff = hpfCutoff;
            voice.hpfQ = hpfQ;
            voice.hpfEnvDepth = hpfEnvDepth;
        }
    }
        
    // For all the samples we need to render (sampleCount)...
    for (int sample = 0; sample < sampleCount; ++sample) {
        updateLFO();
        
        // Get next noise value
        float noise;
        switch (noiseType) {
            case 0: { // White
                noise = whiteNoise.nextValue() * noiseLevel;
                break;
            }
            case 1: { // Pink
                noise = pinkNoise.nextValue() * noiseLevel;
                break;
            }
        }
        
        float outputLeft = 0.0f;
        float outputRight = 0.0f;

        for (int v = 0; v < constants::MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            
            if (voice.env.isActive()) {
                // Render voice with noise mixed in
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
            // TODO: implement stereophony
            outputBufferLeft[sample] = outputLeft;
            outputBufferRight[sample] = outputRight;
        }
        else {
            // Mix both output in left buffer if mono
            outputBufferLeft[sample] = (outputLeft + outputRight) * 0.5f;
        }
    }
    
    // Reset envelope and filter if done
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        
        if (!voice.env.isActive()) {
            voice.env.reset();
            voice.lpf.reset();
            voice.hpf.reset();
        }
    }

    // TODO: remove when done with dev, but add back when developing features
    // Protect buffers from loudness
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
            // TODO: not sure about this, see for change (189)
            pitchBend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            break;
        }
    }
}

void Synth::startVoice(int voiceIndex, int note, int velocity) {
    float period = calcPeriod(voiceIndex, note);

    Voice& voice = voices[voiceIndex];
    voice.target = period;
    
    // Calculate note distance for glide
    int noteDistance = 0;
    if (lastNote > 0) {
        // Glide mode "always" or "while playing legato"
        if ((glideMode == 2) || ((glideMode == 1) && isPlayingLegatoStyle())) {
            noteDistance = note - lastNote;
        }
    }
    
    /*
     Set voice.period to the period to glide from; this is necessary because in polyphony mode,
     the voice may not have the period of the most recent note that was played, if that note was
     handled by another voice.
     Note : 2^N/12 == 1.059463094359^N, where N is in semitones
     .*/
    voice.period = period * std::pow(1.059463094359f, float(noteDistance) - glideBend);
    
    // Limit period to small value
    if (voice.period < 6.0f) { voice.period = 6.0f; }
    
    lastNote = note;
    voice.note = note;
    
    // Reset oscillators when starting voice
    if (oscReset) {
        voice.osc1.reset();
        voice.osc2.reset();
    }
    
    // Apply curve to velocity
    // Custom curve with dynamic range -23dB - 0.72dB
    float velocityCurve = 0.004f * float((velocity + 64) * (velocity + 64)) - 8.0f;
    voice.osc1.amplitude = velocityCurve * volumeTrim;
    voice.osc2.amplitude = voice.osc1.amplitude;
    
    // Apply levels
    // TODO: apply change when moving slider
    voice.osc1.amplitude = voice.osc1.amplitude * osc1Level;
    voice.osc2.amplitude = voice.osc2.amplitude * osc2Level;

    // LRN & to dereference voice.env to access it just by env variable
    // Envelope settings + trigger envelope
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
    env.attack();
    
    // Filter
    voice.lpfCutoff = lpfCutoff;
    voice.lpfQ = lpfQ;
    voice.hpfCutoff = hpfCutoff;
    voice.hpfQ = hpfQ;
    
    // Filter envelope settings + trigger envelope
    Envelope& lpfEnv = voice.lpfEnv;
    lpfEnv.attackMultiplier = lpfAttack;
    lpfEnv.decayMultiplier = lpfDecay;
    lpfEnv.sustainLevel = lpfSustain;
    lpfEnv.releaseMultiplier = lpfRelease;
    lpfEnv.attack();
    
    Envelope& hpfEnv = voice.hpfEnv;
    hpfEnv.attackMultiplier = hpfAttack;
    hpfEnv.decayMultiplier = hpfDecay;
    hpfEnv.sustainLevel = hpfSustain;
    hpfEnv.releaseMultiplier = hpfRelease;
    hpfEnv.attack();
}

void Synth::restartVoiceLegato(int note, int velocity) {
    float period = calcPeriod(0, note);
    
    Voice& voice = voices[0];
    voice.target = period;
    
    // Directly set period to voice if glide is off
    if (glideMode == 0) { voice.period = period; }
    
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
    
    if (polyMode == 0) { // Mono
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

float Synth::calcPeriod(int voiceIndex, int note) const
{
    // Add small analog drift to frequency
    float freq = 440.0f * std::exp2((float(note - 69 + (constants::ANALOG_DRIFT * float(voiceIndex))) + tune) / 12.0f);
    float period = sampleRate / freq;
    
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
        case 0x01: { // mod wheel
            // Convert data2 to parabolic curve (more control over small values)
            modWheel = 0.0005f * float(data2);
            break;
        }
        case 0x40: { // sustain
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

void Synth::updateLFO()
{
    // Decrement and enter every LOWER_UPDATE_RATE_MAX_VALUE step
    if (--lfoStep <= 0) {
        lfoStep = constants::LOWER_UPDATE_RATE_MAX_VALUE;
        
        lfo += lfoInc;
        
        // Keep lfo phase value between -/+ pi for std:sin()
        if (lfo > constants::PI) { lfo -= constants::TWO_PI; }
        
        const float sine = std::sin(lfo);
        
        // Create and apply vibrato modulation to voices
        float vibratoMod = 1.0f + sine * (modWheel + vibrato);
        
        // LFO depth for filter cutoff
        float lpfMod = lpfLFODepth * sine;
        float hpfMod = hpfLFODepth * sine;
        
        // One-pole filter to move filterZip closer to filterMod every step
        lpfZip += 0.005f * (lpfMod - lpfZip);
        hpfZip += 0.005f * (hpfMod - hpfZip);
        
        for (int v = 0; v < constants::MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            
            if (voice.env.isActive()) {
                voice.osc1.modulation = vibratoMod;
                voice.osc2.modulation = vibratoMod;
                voice.lpfMod = lpfZip;
                voice.hpfMod = hpfZip;
                voice.updateLFO();
                updatePeriod(voice);
            }
        }
    }
}

void Synth::updatePeriod(Voice &voice)
{
    // TODO: master tune
    voice.osc1.period = voice.period * pitchBend;
    voice.osc2.period = voice.osc1.period * osc2detune;
}

bool Synth::isPlayingLegatoStyle() const
{
    int held = 0;
    for (int i = 0; i < constants::MAX_VOICES; ++i) {
        if (voices[i].note > 0) { held += 1; }
    }
    return held > 0;
}
