/*
  ==============================================================================

    WavetableGenerator.cpp
    Created: 23 Apr 2024 12:45:34pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "WavetableGenerator.h"

std::vector<float> WavetableGenerator::generateSineWavetable() {
    std::vector<float> waveTable = std::vector<float>(constants::WAVETABLE_LENGTH);

    for (int i = 0; i < constants::WAVETABLE_LENGTH; ++i) {
        waveTable[i] = std::sinf(2 * constants::PI * static_cast<float>(i) / constants::WAVETABLE_LENGTH);
    }

    return waveTable;
}

std::vector<float> WavetableGenerator::generateTriangleWavetable() {
    std::vector<float> waveTable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

    // For the triangle wavetable, I feel that 10 harmonics is enough
    int harmonicsCount = 10;

    for (int k = 1; k <= harmonicsCount; ++k) {
        for (int j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
            const float phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
            
            waveTable[j] += 8.f / std::pow(constants::PI, 2.f) * std::pow(-1.f, k) * std::pow(2 * k - 1, -2.f) * std::sin((2.f * k - 1.f) * phase);
            }
    }

    return waveTable;
}

std::vector<float> WavetableGenerator::generateSquareWavetable(int harmonicsCount) {
    std::vector<float> waveTable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

    for (int k = 1; k <= harmonicsCount; ++k) {
        for (int j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
            const float phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
            
            waveTable[j] += 4.f / constants::PI * std::pow(2.f * k - 1.f, -1.f) * std::sin((2.f * k - 1.f) * phase);
        }
    }

    return waveTable;
}

std::vector<float> WavetableGenerator::generateSawtoothWavetable(int harmonicsCount) {
    auto waveTable = std::vector<float>(constants::WAVETABLE_LENGTH, 0.f);

    for (int k = 1; k <= harmonicsCount; ++k) {
        for (int j = 0; j < constants::WAVETABLE_LENGTH; ++j) {
            const float phase = 2.f * constants::PI * 1.f * j / constants::WAVETABLE_LENGTH;
            
            waveTable[j] += 2.f / constants::PI * std::pow(-1.f, k) * std::pow(k, -1.f) * std::sin(k * phase);
        }
    }

    return waveTable;
}
