/*
  ==============================================================================

    Constants.h
    Created: 23 Jan 2024 2:30:14pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

/**
 This namespace contains constant values used throughout the plugin.
 */
namespace constants
{
    // Some special values used for notes
    inline constexpr int NO_NOTE_VALUE { 0 };

    // Define our pi based values in float instead of doubles like in std lib
    // Approximation of 2 * pi
    inline constexpr float TWO_PI { 6.2831853071795864f };
    // Approximation of pi
    inline constexpr float PI { 3.1415926535897932f };
    // Approximation of pi / 4
    inline constexpr float PI_OVER_FOUR { 0.7853981633974483f };

    // Treshold under which amplitude level should be considered silent
    inline constexpr float SILENCE_TRESHOLD { 0.0001f };

    // Various target levels for envelopes
    inline constexpr float ENV_ATK_TARGET { 2.0f };
    inline constexpr float ENV_SUS_TARGET { 1.0f };
    inline constexpr float ENV_REL_TARGET { 0.0f };

    // Maximum of voices for this synth
    inline constexpr int MAX_VOICES { 10 };

    // Analog oscillator drift factor
    inline constexpr float ANALOG_DRIFT { 0.002f };

    // Lower rate; used for LFO among other things
    // Will be multiplied by the inverse of the sample rate
    inline constexpr int LOWER_UPDATE_RATE_MAX_VALUE { 32 };

    // Max oscillators count is number of MIDI notes available
    inline constexpr int WAVETABLE_OSCILLATORS_COUNT { 128 };

    // Wavetable sample size
    inline constexpr int WAVETABLE_LENGTH { 2048 };

    // Samples to add to envelop attack/release to prevent pop
    inline constexpr int POP_PREVENT_SAMPLES { 2500 };
}
