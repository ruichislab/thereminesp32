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
  this->volume = 0;
  this->phase = 0.0;
  this->phaseIncrement = 0.0;
}

void Synthesizer::init() {
  globalSynth = this;

  // Configure Timer 0
  // Prescaler 80 -> 80MHz / 80 = 1MHz (1 tick = 1us)
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);

  // Alarm every (1000000 / sampleRate) microseconds
  // For 22050Hz, that's approx 45us
  int alarmValue = 1000000 / sampleRate;
  timerAlarmWrite(timer, alarmValue, true);
  timerAlarmEnable(timer);
}

void Synthesizer::setSynthType(int type) {
  this->synthType = type;
}

void Synthesizer::setFrequency(float frequency) {
  this->frequency = frequency;
  this->phaseIncrement = frequency / (float)sampleRate;
}

void Synthesizer::setVolume(int volume) {
  // Clamp volume between 0 and 255
  if (volume < 0) volume = 0;
  if (volume > 255) volume = 255;
  this->volume = volume;
}

void Synthesizer::update() {
  // Increment phase
  phase += phaseIncrement;
  if (phase >= 1.0) phase -= 1.0;

  uint8_t sample = 0;

  // Only generate sound if volume is > 0 to save CPU/Power if needed
  // though strictly for a synth we just keep running.

  switch (synthType) {
    case 0: sample = getSineSample(); break;
    case 1: sample = getSquareSample(); break;
    case 2: sample = getSawtoothSample(); break;
    case 3: sample = getTriangleSample(); break;
    default: sample = getSineSample(); break;
  }

  // Apply volume
  // sample is 0-255. Volume is 0-255.
  // Output = (sample * volume) / 255
  // To be safe with types:
  uint16_t output = ((uint16_t)sample * (uint16_t)volume) >> 8;

  dacWrite(dacPin, output);
}

uint8_t Synthesizer::getSineSample() {
  // Approximate sine with standard library for simplicity
  // Optimization: Precompute table if needed, but sin() is fast enough on ESP32 for 22kHz
  // range -1 to 1 -> map to 0-255
  float rads = phase * 2.0 * PI;
  float val = sin(rads);
  return (uint8_t)((val + 1.0) * 127.5);
}

uint8_t Synthesizer::getSquareSample() {
  if (phase < 0.5) return 255;
  else return 0;
}

uint8_t Synthesizer::getSawtoothSample() {
  return (uint8_t)(phase * 255.0);
}

uint8_t Synthesizer::getTriangleSample() {
  if (phase < 0.5) {
    return (uint8_t)(phase * 2.0 * 255.0);
  } else {
    return (uint8_t)((1.0 - phase) * 2.0 * 255.0);
  }
}
