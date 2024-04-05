/*
  ==============================================================================

    Voice.h
    Created: 22 Jan 2024 3:21:10pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "Blit.h"
#include "WavetableOscillator.h"
#include "Envelope.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"

/**
 Represents a voice for the synthesizer; produces the next output sample for a given note.
 The synthesizer can have multiple voices in polyphony.
 */
// TODO: split in .h and .cpp files
class Voice
{
public:
    int note; // the MIDI note number for the current voice
    float frequency; // the base frequency of the note played on OSC1 for this voice
    float velocityAmp;
    float osc1Level;
    float osc2Level;
    float target; // target for glide
    float glideRate; // copy of synth's glide rate
    bool ringMod;
    
    // Filters
    LowPassFilter lpf;
    HighPassFilter hpf;
    float lpfCutoff;
    float hpfCutoff;
    float lpfQ;
    float hpfQ;
    float lpfMod;
    float hpfMod;
    float lpfEnvDepth;
    float hpfEnvDepth;
    
    // OSC wavetables
    std::vector<WavetableOscillator> sineTableOsc1;
    std::vector<WavetableOscillator> triTableOsc1;
    std::vector<WavetableOscillator> squareTableOsc1;
    std::vector<WavetableOscillator> sawTableOsc1;
    
    std::vector<WavetableOscillator> sineTableOsc2;
    std::vector<WavetableOscillator> triTableOsc2;
    std::vector<WavetableOscillator> squareTableOsc2;
    std::vector<WavetableOscillator> sawTableOsc2;
    
    // Wavetables morph factor
    float osc1Morph;
    float osc2Morph;
    
    // Envelopes
    Envelope env;
    Envelope lpfEnv;
    Envelope hpfEnv;
    
    /**
     Resets the state of the voice instance and its components.
     */
    void reset()
    {
        note = constants::NO_NOTE_VALUE;
        env.reset();
        lpf.reset();
        lpfEnv.reset();
        hpf.reset();
        hpfEnv.reset();
        osc1Morph = 0.f;
        osc2Morph = 0.f;
        ringMod = false;
    }
    
    /**
     Triggers the release phase of the envelope.
     */
    void release()
    {
        env.release();
        lpfEnv.release();
        hpfEnv.release();
    }
    
    /**
      Renders the next values of the oscillators. This function also takes noise as input.
     */
    float render(float noise)
    {
        float output = 0.0f;
        float envelope = env.nextValue();
    
        // If the envelope is done (level extremely close to 0), stop note
        if (envelope < constants::SILENCE_TRESHOLD) {
            sineTableOsc1[note].stop();
            sawTableOsc1[note].stop();
            triTableOsc1[note].stop();
            squareTableOsc1[note].stop();
            
            sineTableOsc2[note].stop();
            sawTableOsc2[note].stop();
            triTableOsc2[note].stop();
            squareTableOsc2[note].stop();

            note = constants::NO_NOTE_VALUE;
        }
        
        // All tables are "playing" at the same time, so we only need to check in one
        if (sineTableOsc1[note].isPlaying()) {
            float sineSample = sineTableOsc1[note].getSample();
            float triSample = triTableOsc1[note].getSample();
            float squareSample = squareTableOsc1[note].getSample();
            float sawSample = sawTableOsc1[note].getSample();
            
            output += interpolatedSample(osc1Morph, sineSample, triSample, squareSample, sawSample) * 0.2f * osc1Level;
        }
        if (sineTableOsc2[note].isPlaying()) {
            float sineSample = sineTableOsc2[note].getSample();
            float triSample = triTableOsc2[note].getSample();
            float squareSample = squareTableOsc2[note].getSample();
            float sawSample = sawTableOsc2[note].getSample();
            
            if (ringMod) {
                output *= interpolatedSample(osc2Morph, sineSample, triSample, squareSample, sawSample) * osc2Level;
            }
            else {
                output -= interpolatedSample(osc2Morph, sineSample, triSample, squareSample, sawSample) * 0.2f * osc2Level;
            }
        }
        
        // Velocity amplitude modifier
        output = output * velocityAmp;
        
        // Mix with noise
        output = output + noise;
        
        // Apply filter in series; first LPF, then HPF
        output = lpf.render(output);
        output = hpf.render(output);
        
        return output * envelope;
    }
    
    /**
     Update various modulations on the voice
     */
    void updateLFO()
    {
        // Update frequency with glide rate
        frequency += glideRate * (target - frequency);
        
        // LPF
        float lpfEnvMod = lpfEnv.nextValue() * lpfEnvDepth;

        // Update coefficiants of filter with modulation, if any
        float modulatedCutoff = lpfCutoff * std::exp(lpfMod + lpfEnvMod); // TODO: envmod outside of exp
        modulatedCutoff = std::clamp(modulatedCutoff, 30.0f, 20000.0f); // clamp to prevent crazy values
        lpf.updateCoefficiants(modulatedCutoff, lpfQ);
        
        // HPF 
        float hpfEnvMod = hpfEnv.nextValue() * hpfEnvDepth;
        
        float modulatedHpfCutoff = hpfCutoff * std::exp(hpfMod + hpfEnvMod);
        modulatedHpfCutoff = std::clamp(modulatedHpfCutoff, 30.0f, 20000.0f);
        hpf.updateCoefficiants(modulatedHpfCutoff, hpfQ);
    }
    
    // TODO: extract wavetable factory functions in separate class
    // TODO: source to https://thewolfsound.com/android-synthesizer-6-wavetable-synthesis-in-c-plus-plus/#wavetable-factory
    std::vector<float> generateSineWavetable() {
        auto sineWavetable = std::vector<float>(constants::WAVETABLE_LENGTH);

        for (auto i = 0; i < constants::WAVETABLE_LENGTH; ++i) {
            sineWavetable[i] = std::sinf(2 * constants::PI * static_cast<float>(i) / constants::WAVETABLE_LENGTH);
        }

        return sineWavetable;
    }

    std::vector<float> generateTriangleWavetable() {
        auto triangleWaveTable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

        // For the triangle wavetable, I feel that 13 harmonics is enough
        int harmonicsCount = 13;

        for (auto k = 1; k <= harmonicsCount; ++k) {
            for (auto j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
                const auto phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
                
                triangleWaveTable[j] += 8.f / std::pow(constants::PI, 2.f) * std::pow(-1.f, k) * std::pow(2 * k - 1, -2.f) * std::sin((2.f * k - 1.f) * phase);
                }
        }

        return triangleWaveTable;
    }

    std::vector<float> generateSquareWavetable(int harmonicsCount) {
        auto squareWavetable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

        for (auto k = 1; k <= harmonicsCount; ++k) {
            for (auto j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
                const auto phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
                
                squareWavetable[j] += 4.f / constants::PI * std::pow(2.f * k - 1.f, -1.f) * std::sin((2.f * k - 1.f) * phase);
            }
        }

        return squareWavetable;
    }

    std::vector<float> generateSawWavetable(int harmonicsCount) {
        auto sawWavetable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

        for (auto k = 1; k <= harmonicsCount; ++k) {
            for (auto j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
                const auto phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
                
                sawWavetable[j] += 2.f / constants::PI * std::pow(-1.f, k) * std::pow(k, -1.f) * std::sin(k * phase);
            }
        }

        return sawWavetable;
    }

    
    /**
     Returns the interpolated sample across the four wave shapes.
     */
    float interpolatedSample(float factor, float s1, float s2, float s3, float s4) {
        if (factor <= 0.33f) {
            return s1 + (s2 - s1) * (factor / 0.33f);
        } else if (factor <= 0.66f) {
            return s2 + (s3 - s2) * ((factor - 0.33f) / 0.33f);
        } else {
            return s3 + (s4 - s3) * ((factor - 0.66f) / 0.33f);
        }
    }

    /**
     Initializes the wavetable oscillators. This should only be called whenever the sample rate is set/changes.
     */
    void initializeOscillators(float sampleRate)
    {
        // Basic sine wave
        auto sineTab = generateSineWavetable();
        
        // Triangle (kept simple)
        auto triTab = generateTriangleWavetable();
        
        /*
         Generate wavetables with varying number of harmonics. The number of frequencies per table
         were kind of determined by ear (which step at which frequency gives a less jarring drop
         in harmonics).
         */
        
        // Saw
        auto sawTab1 = generateSawWavetable(368);
        auto sawTab2 = generateSawWavetable(256);
        auto sawTab3 = generateSawWavetable(128);
        auto sawTab4 = generateSawWavetable(72);
        auto sawTab5 = generateSawWavetable(40);
        auto sawTab6 = generateSawWavetable(19);
        auto sawTab7 = generateSawWavetable(10);
        auto sawTab8 = generateSawWavetable(5);
        
        // Square
        auto sqrTab1 = generateSquareWavetable(368);
        auto sqrTab2 = generateSquareWavetable(256);
        auto sqrTab3 = generateSquareWavetable(110);
        auto sqrTab4 = generateSquareWavetable(60);
        auto sqrTab5 = generateSquareWavetable(30);
        auto sqrTab6 = generateSquareWavetable(10);
        auto sqrTab7 = generateSquareWavetable(7);
        auto sqrTab8 = generateSquareWavetable(5);
                
        // Clear oscillators wavetables
        sineTableOsc1.clear();
        sawTableOsc1.clear();
        triTableOsc1.clear();
        squareTableOsc1.clear();
        sineTableOsc2.clear();
        sawTableOsc2.clear();
        triTableOsc2.clear();
        squareTableOsc2.clear();
        
        /*
         As we go up in MIDI notes, we assign wavetables with less harmonics to prevent aliasing.
         This is obviously not perfect, and could be tweaked further.
         */
        for (auto i = 0; i < constants::WAVETABLE_OSCILLATORS_COUNT; ++i) {
            sineTableOsc1.emplace_back(sineTab, sampleRate);
            sineTableOsc2.emplace_back(sineTab, sampleRate);
            triTableOsc1.emplace_back(triTab, sampleRate);
            triTableOsc2.emplace_back(triTab, sampleRate);

            if (i < 28) {
                sawTableOsc1.emplace_back(sawTab1, sampleRate);
                sawTableOsc2.emplace_back(sawTab1, sampleRate);
                squareTableOsc1.emplace_back(sqrTab1, sampleRate);
                squareTableOsc2.emplace_back(sqrTab1, sampleRate);
            }
            else if (i < 40) {
                sawTableOsc1.emplace_back(sawTab2, sampleRate);
                sawTableOsc2.emplace_back(sawTab2, sampleRate);
                squareTableOsc1.emplace_back(sqrTab2, sampleRate);
                squareTableOsc2.emplace_back(sqrTab2, sampleRate);
            }
            else if (i < 51) {
                sawTableOsc1.emplace_back(sawTab3, sampleRate);
                sawTableOsc2.emplace_back(sawTab3, sampleRate);
                squareTableOsc1.emplace_back(sqrTab3, sampleRate);
                squareTableOsc2.emplace_back(sqrTab3, sampleRate);
            }
            else if (i < 64) {
                sawTableOsc1.emplace_back(sawTab4, sampleRate);
                sawTableOsc2.emplace_back(sawTab4, sampleRate);
                squareTableOsc1.emplace_back(sqrTab4, sampleRate);
                squareTableOsc2.emplace_back(sqrTab4, sampleRate);
            }
            else if (i < 75) {
                sawTableOsc1.emplace_back(sawTab5, sampleRate);
                sawTableOsc2.emplace_back(sawTab5, sampleRate);
                squareTableOsc1.emplace_back(sqrTab5, sampleRate);
                squareTableOsc2.emplace_back(sqrTab5, sampleRate);
            }
            else if (i < 87) {
                sawTableOsc1.emplace_back(sawTab6, sampleRate);
                sawTableOsc2.emplace_back(sawTab6, sampleRate);
                squareTableOsc1.emplace_back(sqrTab6, sampleRate);
                squareTableOsc2.emplace_back(sqrTab6, sampleRate);
            }
            else if (i < 99) {
                sawTableOsc1.emplace_back(sawTab7, sampleRate);
                sawTableOsc2.emplace_back(sawTab7, sampleRate);
                squareTableOsc1.emplace_back(sqrTab7, sampleRate);
                squareTableOsc2.emplace_back(sqrTab7, sampleRate);
            }
            else {
                sawTableOsc1.emplace_back(sawTab8, sampleRate);
                sawTableOsc2.emplace_back(sawTab8, sampleRate);
                squareTableOsc1.emplace_back(sqrTab8, sampleRate);
                squareTableOsc2.emplace_back(sqrTab8, sampleRate);
            }
        }
    }
    
    /**
     Sets the frequency for the wavetables at MIDI note index in each wavetable vectors.
     */
    void setFrequencyAtNote(int note, float freq)
    {
        // OSC1
        sineTableOsc1[note].setFrequency(freq);
        sineTableOsc1[note].initFrequency = freq;
        
        sawTableOsc1[note].setFrequency(freq);
        sawTableOsc1[note].initFrequency = freq;
        
        triTableOsc1[note].setFrequency(freq);
        triTableOsc1[note].initFrequency = freq;
        
        squareTableOsc1[note].setFrequency(freq);
        squareTableOsc1[note].initFrequency = freq;
        
        // OSC2
        sineTableOsc2[note].setFrequency(freq);
        sineTableOsc2[note].initFrequency = freq;
        
        sawTableOsc2[note].setFrequency(freq);
        sawTableOsc2[note].initFrequency = freq;
        
        triTableOsc2[note].setFrequency(freq);
        triTableOsc2[note].initFrequency = freq;
        
        squareTableOsc2[note].setFrequency(freq);
        squareTableOsc2[note].initFrequency = freq;
    }
    
    /**
     Modifies the frequency for the wavetables at MIDI note index in each wavetable vectors.
     */
    void modFrequencyAtNote(int note, float pitchBend, float vibratoMod, float osc2Detune)
    {
        // Apply pitch bend, vibrato and OSC2 detune (semi + cents)
        sineTableOsc1[note].setFrequency(sineTableOsc1[note].initFrequency * pitchBend * vibratoMod);
        sineTableOsc2[note].setFrequency(sineTableOsc2[note].initFrequency * pitchBend * osc2Detune * vibratoMod);
        
        sawTableOsc1[note].setFrequency(sawTableOsc1[note].initFrequency * pitchBend * vibratoMod);
        sawTableOsc2[note].setFrequency(sawTableOsc2[note].initFrequency * pitchBend * osc2Detune * vibratoMod);
        
        triTableOsc1[note].setFrequency(triTableOsc1[note].initFrequency * pitchBend * vibratoMod);
        triTableOsc2[note].setFrequency(triTableOsc2[note].initFrequency * pitchBend * osc2Detune * vibratoMod);
        
        squareTableOsc1[note].setFrequency(squareTableOsc1[note].initFrequency * pitchBend * vibratoMod);
        squareTableOsc2[note].setFrequency(squareTableOsc2[note].initFrequency * pitchBend * osc2Detune * vibratoMod);
    }
};

