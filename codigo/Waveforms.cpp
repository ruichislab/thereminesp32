#include "Waveforms.h"

// Definitions
uint8_t Waveforms::sine[WAVETABLE_SIZE];
uint8_t Waveforms::triangle[WAVETABLE_SIZE];
uint8_t Waveforms::saw[WAVETABLE_SIZE];
uint8_t Waveforms::square[WAVETABLE_SIZE];

void Waveforms::init() {
    for (int i = 0; i < WAVETABLE_SIZE; i++) {
        // Sine: 0..255
        float angle = (float)i / WAVETABLE_SIZE * 2.0 * PI;
        sine[i] = (uint8_t)((sin(angle) + 1.0) * 127.5);

        // Sawtooth: 0..255
        saw[i] = (uint8_t)i;

        // Square: 0 or 255
        square[i] = (i < WAVETABLE_SIZE / 2) ? 255 : 0;

        // Triangle: 0..255..0
        // Using slightly safer math to ensure peak is smooth
        if (i < WAVETABLE_SIZE / 2) {
            // Going up: 0 to 127 -> 0 to 254
            triangle[i] = (uint8_t)(i * 2);
        } else {
            // Going down: 128 to 255 -> 255 to 0
            // i=128: (255-128)*2 = 127*2 = 254.
            // i=255: (255-255)*2 = 0.
            triangle[i] = (uint8_t)((WAVETABLE_SIZE - 1 - i) * 2);
        }
    }
}
