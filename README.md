# cppsynth 🎹

```cppsynth```, short for C++ synthesizer, is a simple open source wavetable polyphonic synthesizer made in C++ with the JUCE framework. It can be compiled as a VST3 plugin, or as a standalone application.

Demo running inside Bitwig Studio : https://www.youtube.com/watch?v=t_3MBXmM0h8

<img width="1100" alt="cppsynth_demo" src="https://github.com/simonperrier11/cppsynth/assets/44238249/acfecfa4-0ff2-4d22-86a0-19d3ebd53ef2">

## Features 🎛️
- **Dual wavetable oscillators**, offering real time morphing between sine <-> triangle wave shapes, triangle <-> square wave shapes, and square <-> saw wave shapes
- Real time adjustment of OSC2's pitch by +/- 24 semitones and +/- 50 cents
- **Polyphonic** mode, with up to 10 voices simultaneously, and **monophonic** mode with last note priority
- **White/Pink noise** generator
- **Sine wave low frequency oscillator (LFO)** (~0-20Hz), with adjustable depth for pitch, low pass filter cutoff frequency, and high pass filter cutoff frequency
- **ADSR amplitude envelope** with attack/decay/release adjustable between 0-10 seconds
- **State variable low pass and high pass filters**, with adjustable cutoff frequency (0-20000Hz) and resonance, each with their own **reversable ADSR envelope**
- Overall output level adjustment from -24dB to +6dB, and overall pitch adjustment by +/- 100 cents
- MIDI keyboard input, with support of sustain, mod wheel and pitch wheel functionalities
- Additional features : **Ring mod**, **phase randomizer** on new note, and **velocity sensitivity** toggle

## Build 🧑‍💻
You will need the latest version of [JUCE 7](https://juce.com/get-juce/).

### MacOS 🍎
You will need to have [Xcode and the Command Line Tools for Xcode](https://developer.apple.com/xcode/resources/) installed in order to build the project.

- Open the ```cppsynth.jucer``` project in JUCE
- Make sure the selected exporter is set to Xcode (MacOS), and click on the "Save and export to IDE" button
- In the scheme selection dropdown at the top of the IDE, select either "cppsynth - VST3" or "cppsynth - Standalone Plugin"
- Build the project using ```cmd+B```; the exported build will be in the ```cppsynth/Builds/MacOSX/build/Release``` folder
- If you compiled the project as a VST3 plugin, make sure to move the VST3 file to a folder that your DAW will recognize!
  
### Windows 🪟

## Resources 💛
The goal of this project was for me to learn about audio synthesis and digital signal processing with C++, and to discover the JUCE framework. Making this project was mostly possible thanks to the excellent [_Creating Synthesizer Plug-Ins with C++ and JUCE_ book](https://www.theaudioprogrammer.com/synth-plugin-book) by Matthijs Hollemans (The Audio Programmer), as well as [various tutorials on sound synthesis](https://thewolfsound.com/sound-synthesis/) by Jan Wilczek (WolfSound). Big thanks! Other references are directly in the code as comments.

The font used for the user interface is the [JetBrains Mono font](https://www.jetbrains.com/lp/mono/).
