#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <Arduino.h>
#include <math.h>

class Synthesizer {
public:
    Synthesizer(int dacPin);
    void init();
    void setSynthType(int type);
    void setFrequency(float frequency);
    void setVolume(int volume);
    void update(); // Called by ISR

private:
    int dacPin;
    volatile int synthType;
    volatile float frequency;
    volatile int volume;
    volatile float phase;
    float phaseIncrement;
    const int sampleRate = 22050; // Hz

    // Waveform generation helpers
    uint8_t getSineSample();
    uint8_t getSquareSample();
    uint8_t getSawtoothSample();
    uint8_t getTriangleSample();
};

#endif
