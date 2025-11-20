#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <Arduino.h>

// Wavetable size
#define WAVETABLE_SIZE 256

class Waveforms {
public:
    static uint8_t sine[WAVETABLE_SIZE];
    static uint8_t triangle[WAVETABLE_SIZE];
    static uint8_t saw[WAVETABLE_SIZE];
    static uint8_t square[WAVETABLE_SIZE];

    static void init();
};

#endif
