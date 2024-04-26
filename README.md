# cppsynth 🎹

```cppsynth```, short for C++ synthesizer, is a simple open source wavetable polyphonic synthesizer made in C++ with the JUCE framework. It can be compiled as a VST3 plugin, or as a standalone application.

<img width="1100" alt="cppsynth_demo" src="https://github.com/simonperrier11/cppsynth/assets/44238249/acfecfa4-0ff2-4d22-86a0-19d3ebd53ef2">

## Features 🎛️
- Dual wavetable oscillators, offering real time morphing between sine <-> triangle wave shapes, triangle <-> square wave shapes, and square <-> saw wave shapes;
  - The second oscillator's pitch can also be adjusted in real time by +/- 24 semitones and +/- 50 cents;
- Polyphonic mode, up to 10 voices simultaneously;
- Monophonic mode, with last note priority;
- White/Pink noise generator;
- Sine wave low frequency oscillator (LFO) (~0-20Hz), with adjustable depth for pitch, low pass filter cutoff frequency and high pass filter cutoff frequency;
- ADSR amplitude envelope with attack/decay/release adjustable between 0-10 seconds;
- State variable low pass and high pass filters, with adjustable cutoff frequency (0-20000Hz) and resonance, each with their own reversable ADSR envelope;
- Overall output level adjustment from -24dB to +6dB, and ovrall pitch adjustment by +/- 100 cents;
- MIDI keyboard input, with support of sustain, mod wheel and pitch wheel features;
- Ring mod, phase randomizer on new note, and velocity sensitivity toggles.

## Build 🧑‍💻
### MacOS 🍎

### Windows 🪟

## Resources 💛
Making this project was mostly possible thanks to the [_Creating Synthesizer Plug-Ins with C++ and JUCE_ book](https://www.theaudioprogrammer.com/synth-plugin-book) by Matthijs Hollemans (The Audio Programmer), as well as [various tutorials on sound synthesis](https://thewolfsound.com/sound-synthesis/) by Jan Wilczek (WolfSound). Big thanks!
