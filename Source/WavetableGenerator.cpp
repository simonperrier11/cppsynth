/*
  ==============================================================================

    WavetableGenerator.cpp
    Created: 23 Apr 2024 12:45:34pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "WavetableGenerator.h"

std::vector<float> WavetableGenerator::generateSineWavetable() {
    auto sineWavetable = std::vector<float>(constants::WAVETABLE_LENGTH);

    for (auto i = 0; i < constants::WAVETABLE_LENGTH; ++i) {
        sineWavetable[i] = std::sinf(2 * constants::PI * static_cast<float>(i) / constants::WAVETABLE_LENGTH);
    }

    return sineWavetable;
}

std::vector<float> WavetableGenerator::generateTriangleWavetable() {
    auto triangleWaveTable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

    // For the triangle wavetable, I feel that 10 harmonics is enough
    int harmonicsCount = 10;

    for (auto k = 1; k <= harmonicsCount; ++k) {
        for (auto j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
            const auto phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
            
            triangleWaveTable[j] += 8.f / std::pow(constants::PI, 2.f) * std::pow(-1.f, k) * std::pow(2 * k - 1, -2.f) * std::sin((2.f * k - 1.f) * phase);
            }
    }

    return triangleWaveTable;
}

std::vector<float> WavetableGenerator::generateSquareWavetable(int harmonicsCount) {
    auto squareWavetable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

    for (auto k = 1; k <= harmonicsCount; ++k) {
        for (auto j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
            const auto phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
            
            squareWavetable[j] += 4.f / constants::PI * std::pow(2.f * k - 1.f, -1.f) * std::sin((2.f * k - 1.f) * phase);
        }
    }

    return squareWavetable;
}

std::vector<float> WavetableGenerator::generateSawtoothWavetable(int harmonicsCount) {
    auto sawWavetable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

    for (auto k = 1; k <= harmonicsCount; ++k) {
        for (auto j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
            const auto phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
            
            sawWavetable[j] += 2.f / constants::PI * std::pow(-1.f, k) * std::pow(k, -1.f) * std::sin(k * phase);
        }
    }

    return sawWavetable;
}
