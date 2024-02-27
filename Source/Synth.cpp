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
        voices[v].filter.sampleRate = sampleRate;
    }
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
    lfo = 0.0f;
    lfoStep = 0;
    modWheel = 0;
    lastNote = 0;
    filterZip = 0;
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
            updatePeriod(voice);
            voice.glideRate = glideRate;
            voice.osc2.amplitude = voice.osc1.amplitude * oscMix;
            voice.filterCutoff = filterCutoff;
            voice.filterQ = filterQ;
            voice.filterEnvDepth = filterEnvDepth;
        }
    }
        
    // For all the samples we need to render (sampleCount)...
    for (int sample = 0; sample < sampleCount; ++sample) {
        updateLFO();
        
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
    
    // Reset envelope and filter if done
    for (int v = 0; v < constants::MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        
        if (!voice.env.isActive()) {
            voice.env.reset();
            voice.filter.reset();
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

    Voice& voice = voices[v];
    voice.target = period;
    
    // Calculate note distance for glide
    int noteDistance = 0;
    if (lastNote > 0) {
        // Glide mode "always" or "while playing legato"
        if ((glideMode == 2) || ((glideMode == 1) && isPlayingLegatoStyle())) {
            noteDistance = note - lastNote;
        }
    }
    
    // TODO: book p.227
    voice.period = period * std::pow(1.059463094359f, float(noteDistance) - glideBend);
    
    // Limit period to small value
    if (voice.period < 6.0f) { voice.period = 6.0f; }
    
    lastNote = note;
    voice.note = note;
    
    // TODO: reset on new note could be a param
    // voice.osc1.reset();
    // voice.osc2.reset();
    
    // Apply curve to velocity
    // Custom curve with dynamic range -23dB - 0.72dB
    float velocityCurve = 0.004f * float((velocity + 64) * (velocity + 64)) - 8.0f;
    voice.osc1.amplitude = velocityCurve * volumeTrim;

    // LRN & to dereference voice.env to access it just by env variable
    // Envelope settings + trigger envelope
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
    env.attack();
    
    // Filter
    voice.filterCutoff = filterCutoff;
    voice.filterQ = filterQ;
    
    // Filter envelope settings + trigger envelope
    Envelope& filterEnv = voice.filterEnv;
    filterEnv.attackMultiplier = filterAttack;
    filterEnv.decayMultiplier = filterDecay;
    filterEnv.sustainLevel = filterSustain;
    filterEnv.releaseMultiplier = filterRelease;
    filterEnv.attack();
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
        float filterMod = filterLFODepth * sine;
        
        // One-pole filter to move filterZip closer to filterMod every step
        filterZip += 0.005f * (filterMod - filterZip);
        
        for (int v = 0; v < constants::MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            
            if (voice.env.isActive()) {
                voice.osc1.modulation = vibratoMod;
                voice.osc2.modulation = vibratoMod;
                voice.filterMod = filterZip;
                voice.updateLFO();
                updatePeriod(voice);
            }
        }
    }
}

void Synth::updatePeriod(Voice &voice)
{
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
