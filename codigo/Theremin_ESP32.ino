// Theremin_ESP32.ino
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "UltrasonicSensor.h"
#include "Synthesizer.h"
#include "CalibrationManager.h"
#include "SensorTask.h"
#include "Waveforms.h"

// Hardware Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_PIN 32
#define POT_1_PIN 34   // Waveform
#define POT_2_PIN 35   // Master Volume / Sensitivity
#define TRIG_PIN_1 12  // Tone
#define ECHO_PIN_1 13
#define TRIG_PIN_2 14  // Volume
#define ECHO_PIN_2 27
#define DAC_PIN 25

// Objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
UltrasonicSensor sensorTone(TRIG_PIN_1, ECHO_PIN_1);
UltrasonicSensor sensorVolume(TRIG_PIN_2, ECHO_PIN_2);
Synthesizer synth(DAC_PIN);
CalibrationManager calibManager;

// Application State
bool musicalMode = false; // False = Continuous, True = Quantized
int synthType = 0;
int masterVolPot = 0;
float currentFreq = 440.0;
char currentNote[5] = "--";
unsigned long lastDisplayUpdate = 0;

// Button State
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
bool calibrating = false;
unsigned long buttonDownTime = 0;

// Task Handle
TaskHandle_t Task1;

// Helper: Map Float
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Helper: Calibration Routine
void runCalibration() {
    synth.setVolume(0); // Mute during calibration

    float minT = 100.0, maxT = 0.0;
    float minV = 100.0, maxV = 0.0;

    // Step 1: Tone Min (Close)
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("CALIBRACION");
    display.println("Acerca mano Tono");
    display.println("(Alta frecuencia)");
    display.println("Esperando 5s...");
    display.display();
    delay(5000);

    display.println("Leyendo...");
    display.display();
    unsigned long start = millis();
    while(millis() - start < 2000) {
        float d = sharedSensorData.distanceTone;
        if (d > 1.0 && d < 100.0) {
           if (d < minT) minT = d;
        }
        delay(50);
    }

    // Step 2: Tone Max (Far)
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("CALIBRACION");
    display.println("Aleja mano Tono");
    display.println("(Baja frecuencia)");
    display.println("Esperando 5s...");
    display.display();
    delay(5000);

    display.println("Leyendo...");
    display.display();
    start = millis();
    maxT = minT + 10.0; // safe default
    while(millis() - start < 2000) {
        float d = sharedSensorData.distanceTone;
        if (d > 1.0 && d < 100.0) {
           if (d > maxT) maxT = d;
        }
        delay(50);
    }

    // Step 3: Volume Min (Close - Mute)
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("CALIBRACION");
    display.println("Acerca mano Vol");
    display.println("(Silencio)");
    display.println("Esperando 5s...");
    display.display();
    delay(5000);

    display.println("Leyendo...");
    display.display();
    start = millis();
    while(millis() - start < 2000) {
        float d = sharedSensorData.distanceVolume;
        if (d > 1.0 && d < 100.0) {
           if (d < minV) minV = d;
        }
        delay(50);
    }

    // Step 4: Volume Max (Far - Max Vol)
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("CALIBRACION");
    display.println("Aleja mano Vol");
    display.println("(Max Volumen)");
    display.println("Esperando 5s...");
    display.display();
    delay(5000);

    display.println("Leyendo...");
    display.display();
    start = millis();
    maxV = minV + 10.0;
    while(millis() - start < 2000) {
        float d = sharedSensorData.distanceVolume;
        if (d > 1.0 && d < 100.0) {
           if (d > maxV) maxV = d;
        }
        delay(50);
    }

    // Save
    calibManager.saveCalibration(minT, maxT, minV, maxV);

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("CALIBRACION");
    display.println("GUARDADA!");
    display.display();
    delay(2000);
}

// Setup
void setup() {
  Serial.begin(115200);

  // Pin Setup
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(POT_1_PIN, INPUT);
  pinMode(POT_2_PIN, INPUT);

  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED init failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println(F("Theremin PRO"));
  display.display();
  delay(1000);

  // Init Subsystems
  calibManager.init();
  sensorTone.init();
  sensorVolume.init();
  synth.init();

  // Setup Sensor Task pointers
  ptrSensorTone = &sensorTone;
  ptrSensorVolume = &sensorVolume;

  // Create Sensor Task on Core 0
  xTaskCreatePinnedToCore(
      sensorTask,   /* Task function. */
      "SensorTask", /* name of task. */
      4096,         /* Stack size of task */
      NULL,         /* parameter of the task */
      1,            /* priority of the task */
      &Task1,       /* Task handle to keep track of created task */
      0);           /* pin task to core 0 */
}

void handleButton() {
    int reading = digitalRead(BUTTON_PIN);

    if (reading == LOW && lastButtonState == HIGH) {
        // Button Pressed
        buttonDownTime = millis();
    }

    if (reading == HIGH && lastButtonState == LOW) {
        // Button Released
        unsigned long duration = millis() - buttonDownTime;
        if (duration < 2000) {
            // Short press: Toggle Mode
            musicalMode = !musicalMode;
            synth.setQuantization(musicalMode);
        }
    }

    if (reading == LOW) {
        // Check for hold
        if (millis() - buttonDownTime > 2000) {
            // Long press: Calibration
            runCalibration();
            buttonDownTime = millis(); // prevent re-trigger immediately
        }
    }

    lastButtonState = reading;
}

void loop() {
  // 1. Input Handling
  handleButton();

  // Read Pots
  int pot1 = analogRead(POT_1_PIN);
  int pot2 = analogRead(POT_2_PIN);

  // Update Synth Type
  int newType = map(pot1, 0, 4095, 0, 3);
  if (newType != synthType) {
      synthType = newType;
      synth.setSynthType(synthType);
  }

  // Master Volume
  masterVolPot = map(pot2, 0, 4095, 0, 255);

  // 2. Get Sensor Data (from Shared Memory updated by Core 0)
  float distTone = sharedSensorData.distanceTone;
  float distVol = sharedSensorData.distanceVolume;

  // 3. Process Tone
  float freq = currentFreq;
  if (distTone > 1.0 && distTone < 100.0) { // Valid range check
      float d = constrain(distTone, calibManager.minDistTone, calibManager.maxDistTone);
      // Map Float for smooth analog feel
      // High pitch (1000Hz) at min distance, Low pitch (60Hz) at max distance
      freq = mapFloat(d, calibManager.minDistTone, calibManager.maxDistTone, 1000.0, 60.0);
  }

  // Update Synth Frequency
  synth.setFrequency(freq);
  currentFreq = freq;

  // Calculate Note Name for UI
  Synthesizer::getClosestNoteFreq(freq, currentNote);

  // 4. Process Volume
  int vol = 0;
  if (distVol > 1.0 && distVol < 100.0) {
      float d = constrain(distVol, calibManager.minDistVol, calibManager.maxDistVol);
      // Map Float
      // Close = Silence (0), Far = Max (255)
      float v = mapFloat(d, calibManager.minDistVol, calibManager.maxDistVol, 0.0, 255.0);
      vol = (int)v;
  } else {
      vol = 255; // Default if lost
  }

  // Apply Master Volume scaling
  vol = (vol * masterVolPot) / 255;
  synth.setVolume(vol);

  // 5. UI Update (Approx 15fps)
  if (millis() - lastDisplayUpdate > 66) {
      lastDisplayUpdate = millis();
      display.clearDisplay();

      // Header
      display.setCursor(0,0);
      display.print(musicalMode ? "MODE: MUSICAL" : "MODE: FREE");
      display.setCursor(90,0);
      switch(synthType){
          case 0: display.print("SIN"); break;
          case 1: display.print("SQR"); break;
          case 2: display.print("SAW"); break;
          case 3: display.print("TRI"); break;
      }

      // Note Big Display
      display.setTextSize(2);
      display.setCursor(10, 15);
      display.print(currentNote);

      display.setTextSize(1);
      display.setCursor(70, 15);
      display.print((int)currentFreq);
      display.print(" Hz");

      // Volume Bar
      display.drawRect(10, 40, 108, 10, WHITE);
      int barWidth = map(vol, 0, 255, 0, 106);
      display.fillRect(11, 41, barWidth, 8, WHITE);

      // Footer
      display.setCursor(10, 54);
      display.print("DT:"); display.print((int)distTone);
      display.print(" DV:"); display.print((int)distVol);

      display.display();
  }

  delay(10); // Small delay for Core 1 loop stability
}
