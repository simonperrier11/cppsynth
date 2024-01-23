/*
  ==============================================================================

    Utils.h
    Created: 23 Jan 2024 4:27:33pm
    Author:  Simon Perrier

  ==============================================================================
*/

#pragma once

/**
 Utility method that silences a buffer if it contains invalid values (nan, inf, etc.), which may cause excessive loudness.
 This should be used in development only.
 */
inline void loudnessProtectBuffer(float* buffer, int sampleCount)
{
    if (buffer == nullptr) { return; }
    
    bool firstWarning = true;
    
    for (int i = 0; i < sampleCount; ++i) {
        float x = buffer[i];
        bool silence = false;
        
        if (std::isnan(x)) {
            DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
            silence = true;
        } else if (std::isinf(x)) {
            DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            silence = true;
        } else if (x < -2.0f || x > 2.0f) {  // screaming feedback
            DBG("!!! WARNING: sample out of range, silencing !!!");
            silence = true;
        } else if (x < -1.0f) {
            if (firstWarning) {
                DBG("!!! WARNING: sample out of range, clamping !!!");
                firstWarning = false;
            }
            // Hard clip to -1.0
            buffer[i] = -1.0f;
        } else if (x > 1.0f) {
            if (firstWarning) {
                DBG("!!! WARNING: sample out of range, clamping !!!");
                firstWarning = false;
            }
            // Hard clip to 1.0
            buffer[i] = 1.0f;
        }
        if (silence) {
            memset(buffer, 0, sampleCount * sizeof(float));
            return;
        }
    }
}
