#include "Synthesizer.h"

// Global pointer for the ISR
Synthesizer* globalSynth = NULL;
hw_timer_t * timer = NULL;

// ISR function - must be in IRAM
void IRAM_ATTR onTimer() {
  if (globalSynth) {
    globalSynth->update();
  }
}

Synthesizer::Synthesizer(int dacPin) {
  this->dacPin = dacPin;
  this->synthType = 0;
  this->frequency = 440.0;
  this->currentFreq = 440.0;
  this->volume = 0;
  this->phaseIndex = 0.0;
  this->phaseIncrement = 0.0;
  this->quantizationEnabled = false;
}

void Synthesizer::init() {
  globalSynth = this;

  // Initialize Waveforms
  Waveforms::init();

  // Configure Timer 0
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);

  // Alarm at sample rate
  int alarmValue = 1000000 / sampleRate;
  timerAlarmWrite(timer, alarmValue, true);
  timerAlarmEnable(timer);
}

void Synthesizer::setSynthType(int type) {
  this->synthType = type;
}

void Synthesizer::setFrequency(float freq) {
  this->frequency = freq;
  // If not quantizing, update immediately (with simple smoothing done externally or here)
  // We'll do logic in updatePhaseIncrement or main loop?
  // For now, let's just set it. The ISR uses phaseIncrement.

  float target = freq;

  // If quantization is enabled, snap to nearest note
  // Note: calculating this in ISR is bad. Calculating here is okay.
  if (quantizationEnabled) {
     target = getClosestNoteFreq(freq, NULL);
  }

  // Simple Glide / Portamento
  // Move currentFreq towards target
  // In a real synth, this might be time-based.
  // Here we just snap for responsiveness or apply slight filter.
  this->currentFreq = target;

  updatePhaseIncrement();
}

void Synthesizer::setVolume(int vol) {
  if (vol < 0) vol = 0;
  if (vol > 255) vol = 255;
  this->volume = vol;
}

void Synthesizer::setQuantization(bool enabled) {
    this->quantizationEnabled = enabled;
}

void Synthesizer::updatePhaseIncrement() {
    // phaseIncrement = (Frequency * TableSize) / SampleRate
    this->phaseIncrement = (currentFreq * (float)WAVETABLE_SIZE) / (float)sampleRate;
}

void IRAM_ATTR Synthesizer::update() {
  // Increment phase
  phaseIndex += phaseIncrement;
  if (phaseIndex >= WAVETABLE_SIZE) phaseIndex -= WAVETABLE_SIZE;

  // Get integer index
  int idx = (int)phaseIndex;
  if (idx >= WAVETABLE_SIZE) idx = 0; // Safety

  uint8_t sample = 128;

  switch (synthType) {
    case 0: sample = Waveforms::sine[idx]; break;
    case 1: sample = Waveforms::square[idx]; break;
    case 2: sample = Waveforms::saw[idx]; break;
    case 3: sample = Waveforms::triangle[idx]; break;
    default: sample = Waveforms::sine[idx]; break;
  }

  // Apply volume
  // Fast integer math: (Sample * Volume) >> 8
  uint16_t output = ((uint16_t)sample * (uint16_t)volume) >> 8;

  dacWrite(dacPin, output);
}

// Musical helper
// Returns closest frequency in Hz to standard 12-tone equal temperament
float Synthesizer::getClosestNoteFreq(float freq, char* noteNameBuffer) {
    if (freq < 1.0) return freq; // Avoid log(0)

    // MIDI Note calculation: 69 + 12 * log2(freq / 440)
    float noteNumFloat = 69.0 + 12.0 * log2(freq / 440.0);
    int noteNum = (int)(noteNumFloat + 0.5); // Round to nearest integer

    // Convert back to freq: 440 * 2^((note - 69)/12)
    float snappedFreq = 440.0 * pow(2.0, (float)(noteNum - 69) / 12.0);

    if (noteNameBuffer != NULL) {
        const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        int octave = (noteNum / 12) - 1;
        int noteIndex = noteNum % 12;
        sprintf(noteNameBuffer, "%s%d", noteNames[noteIndex], octave);
    }

    return snappedFreq;
}
