/*
  ==============================================================================

    Voice.cpp
    Created: 23 Apr 2024 12:37:59pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "Voice.h"

void Voice::reset()
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
    phaseRand = false;
    sustained = false;
}
    
void Voice::release()
{
    env.release();
    lpfEnv.release();
    hpfEnv.release();
}

float Voice::render(float noise)
{
    float output = 0.0f;
    float envelope = env.nextValue();

    // If the envelope is done (level extremely close to 0), stop note
    if (envelope < constants::SILENCE_TRESHOLD) {
        sineTableOsc1[note].stop(phaseRand);
        sawTableOsc1[note].stop(phaseRand);
        triTableOsc1[note].stop(phaseRand);
        squareTableOsc1[note].stop(phaseRand);
        
        sineTableOsc2[note].stop(phaseRand);
        sawTableOsc2[note].stop(phaseRand);
        triTableOsc2[note].stop(phaseRand);
        squareTableOsc2[note].stop(phaseRand);

        note = constants::NO_NOTE_VALUE;
    }
    
    // All tables are "playing" at the same time, so we only need to check in one
    if (sineTableOsc1[note].isPlaying()) {
        float sineSample = sineTableOsc1[note].getSample();
        float triSample = triTableOsc1[note].getSample();
        float squareSample = squareTableOsc1[note].getSample();
        float sawSample = sawTableOsc1[note].getSample();
        
        if (ringMod) {
            output += interpolatedSample(osc1Morph, sineSample, triSample, squareSample, sawSample) * 0.4f * osc1Level;
        }
        else {
            output += interpolatedSample(osc1Morph, sineSample, triSample, squareSample, sawSample) * 0.2f * osc1Level;
        }
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

void Voice::updateLFO()
{
//    // Update frequency with glide rate
//    frequency += glideRate * (target - frequency);
    
    // LPF
    float lpfEnvMod = lpfEnv.nextValue() * lpfEnvDepth;

    // Update coefficiants of filter with modulation, if any
    float modulatedCutoff = lpfCutoff * std::exp(lpfMod + lpfEnvMod);
    modulatedCutoff = std::clamp(modulatedCutoff, 30.0f, 20000.0f); // clamp to prevent crazy values
    lpf.updateCoefficiants(modulatedCutoff, lpfQ);
    
    // HPF
    float hpfEnvMod = hpfEnv.nextValue() * hpfEnvDepth;
    
    float modulatedHpfCutoff = hpfCutoff * std::exp(hpfMod + hpfEnvMod);
    modulatedHpfCutoff = std::clamp(modulatedHpfCutoff, 30.0f, 20000.0f);
    hpf.updateCoefficiants(modulatedHpfCutoff, hpfQ);
}
    
float Voice::interpolatedSample(float factor, float s1, float s2, float s3, float s4) {
    if (factor <= 0.33f) {
        return s1 + (s2 - s1) * (factor / 0.33f);
    } else if (factor <= 0.66f) {
        return s2 + (s3 - s2) * ((factor - 0.33f) / 0.33f);
    } else {
        return s3 + (s4 - s3) * ((factor - 0.66f) / 0.33f);
    }
}

void Voice::initializeOscillators(float sampleRate)
{
    std::unique_ptr<WavetableGenerator> wavetableGen;
    
    // Basic sine wave
    std::vector<float> sineTab = wavetableGen->generateSineWavetable();
    
    // Triangle (kept simple)
    std::vector<float> triTab = wavetableGen->generateTriangleWavetable();
    
    /*
     Generate wavetables with varying number of harmonics. The number of frequencies per table
     were kind of determined by ear (which step at which frequency gives a less jarring drop
     in harmonics).
     */
    
    // Saw
    std::vector<float> sawTab1 = wavetableGen->generateSawtoothWavetable(368);
    std::vector<float> sawTab2 = wavetableGen->generateSawtoothWavetable(256);
    std::vector<float> sawTab3 = wavetableGen->generateSawtoothWavetable(128);
    std::vector<float> sawTab4 = wavetableGen->generateSawtoothWavetable(72);
    std::vector<float> sawTab5 = wavetableGen->generateSawtoothWavetable(50);
    std::vector<float> sawTab6 = wavetableGen->generateSawtoothWavetable(25);
    std::vector<float> sawTab7 = wavetableGen->generateSawtoothWavetable(10);
    std::vector<float> sawTab8 = wavetableGen->generateSawtoothWavetable(5);
    
    // Square
    std::vector<float> sqrTab1 = wavetableGen->generateSquareWavetable(368);
    std::vector<float> sqrTab2 = wavetableGen->generateSquareWavetable(256);
    std::vector<float> sqrTab3 = wavetableGen->generateSquareWavetable(110);
    std::vector<float> sqrTab4 = wavetableGen->generateSquareWavetable(60);
    std::vector<float> sqrTab5 = wavetableGen->generateSquareWavetable(40);
    std::vector<float> sqrTab6 = wavetableGen->generateSquareWavetable(20);
    std::vector<float> sqrTab7 = wavetableGen->generateSquareWavetable(10);
    std::vector<float> sqrTab8 = wavetableGen->generateSquareWavetable(5);
            
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
    
void Voice::setFrequencyAtNote(int note, float freq)
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
    
void Voice::modFrequencyAtNote(int note, float pitchBend, float vibratoMod, float osc2Detune)
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
