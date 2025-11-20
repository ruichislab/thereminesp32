#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <Arduino.h>
#include <math.h>
#include "Waveforms.h"

class Synthesizer {
public:
    Synthesizer(int dacPin);
    void init();

    // Configuration
    void setSynthType(int type);
    void setFrequency(float frequency);
    void setVolume(int volume);
    void setQuantization(bool enabled); // Enable "Musical Mode"

    // ISR method
    void update();

    // Musical Utils
    static float getClosestNoteFreq(float freq, char* noteNameBuffer);

private:
    int dacPin;

    // Audio State (Volatile for ISR)
    volatile int synthType;
    volatile float frequency; // Target frequency
    volatile float currentFreq; // Actual frequency (for glide/quantization)
    volatile int volume;

    // Phase accumulator (Fixed point arithmetic could be faster, but float is fine on ESP32)
    volatile float phaseIndex; // 0.0 to WAVETABLE_SIZE
    float phaseIncrement;
    const int sampleRate = 22050;

    // Quantization State
    bool quantizationEnabled;

    // Helper to calculate phase increment
    void updatePhaseIncrement();
};

#endif
