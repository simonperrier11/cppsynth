/*
  ==============================================================================

    Constants.h
    Created: 23 Jan 2024 2:30:14pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

/**
 This namespace contains constant values used throughout the plugin. Of course, re-compilation is needed when
 changing any of these.
 */
namespace constants
{
    // The value used for non-playing notes
    inline constexpr int noNoteValue { -1 };

    // Define our pi based values in float instead of doubles like in std lib
    // Approximation of 2 * pi
    inline constexpr float twoPi { 6.2831853071795864f };
    // Approximation of pi
    inline constexpr float pi { 3.1415926535897932f };
    // Approximation of pi / 4
    inline constexpr float piOverFour { 0.7853981633974483f };
}
