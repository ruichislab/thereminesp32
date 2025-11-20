// Theremin_ESP32.ino
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>  // Biblioteca para la pantalla OLED
#include "UltrasonicSensor.h"
#include "Synthesizer.h"

// Definimos los pines del hardware adicional
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_PIN 32
#define POT_1_PIN 34   // Potenciómetro para seleccionar sintetizador
#define POT_2_PIN 35   // Potenciómetro para ajustar parámetro (volumen)

// Definimos los pines de los sensores de ultrasonido
#define TRIG_PIN_1 12  // Sensor de tono
#define ECHO_PIN_1 13
#define TRIG_PIN_2 14  // Sensor de volumen
#define ECHO_PIN_2 27

// DAC output pin (para generar sonido)
#define DAC_PIN 25

// Inicializamos la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Inicializamos los sensores de ultrasonido y el sintetizador
UltrasonicSensor sensorTone(TRIG_PIN_1, ECHO_PIN_1);
UltrasonicSensor sensorVolume(TRIG_PIN_2, ECHO_PIN_2);
Synthesizer synth(DAC_PIN);

// Variables del menú y estado
int synthType = 0;
int masterVolume = 255;
unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 100; // Actualizar pantalla cada 100ms

// Variables para filtrado simple
float filteredFreq = 440.0;
float filteredVol = 0.0;

void setup() {
  // Configuramos el botón y los potenciómetros
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(POT_1_PIN, INPUT);
  pinMode(POT_2_PIN, INPUT);

  Serial.begin(115200);

  // Iniciamos la comunicación I2C para la pantalla
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error en la inicialización de la pantalla OLED"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Theremin ESP32"));
  display.println(F("Iniciando..."));
  display.display();

  // Iniciamos los sensores y el sintetizador
  sensorTone.init();
  sensorVolume.init();
  synth.init(); // Esto arranca el timer de audio

  delay(500);
}

void loop() {
  // 1. Leer Potenciómetros (Control General)
  int pot1Value = analogRead(POT_1_PIN);
  int pot2Value = analogRead(POT_2_PIN);

  // Pot 1: Selección de Onda
  int newSynthType = map(pot1Value, 0, 4095, 0, 3);
  if (newSynthType != synthType) {
    synthType = newSynthType;
    synth.setSynthType(synthType);
  }

  // Pot 2: Volumen Maestro (Sensibilidad)
  masterVolume = map(pot2Value, 0, 4095, 0, 255);

  // 2. Leer Sensores de Ultrasonido
  float distanceTone = sensorTone.readDistance();
  float distanceVolume = sensorVolume.readDistance();

  // 3. Procesar Tono (Frecuencia)
  // Rango útil: 5cm a 50cm.
  // 5cm = Tono Alto (ej. 1000Hz), 50cm = Tono Bajo (ej. 100Hz) o viceversa.
  // Theremin clásico: Cerca = Tono Alto.
  float targetFreq = 0;
  if (distanceTone > 2.0 && distanceTone < 60.0) {
     // Mapeo inverso: Distancia corta -> Frecuencia alta
     // constrain para asegurar limites
     float distClamped = constrain(distanceTone, 5.0, 50.0);
     targetFreq = map(distClamped, 5.0, 50.0, 1000.0, 100.0);
  } else {
     // Si está fuera de rango, mantener la última o silenciar?
     // Mantengamos la última frecuencia válida para efecto glide suave
     targetFreq = filteredFreq;
  }

  // Suavizado simple (filtro paso bajo)
  filteredFreq = 0.7 * filteredFreq + 0.3 * targetFreq;
  synth.setFrequency(filteredFreq);

  // 4. Procesar Volumen (Amplitud)
  // Rango útil: 5cm a 40cm.
  // Theremin clásico: Cerca = Silencio, Lejos = Volumen Alto (Control de Expresión)
  // O viceversa: Cerca = Volumen Alto.
  // Usualmente la antena de volumen del theremin: Mano cerca = volumen bajo (silencio).
  float targetVol = 0;
  if (distanceVolume > 2.0 && distanceVolume < 50.0) {
     float distClamped = constrain(distanceVolume, 5.0, 40.0);
     // Cerca (5cm) -> 0 volumen, Lejos (40cm) -> 255 volumen
     targetVol = map(distClamped, 5.0, 40.0, 0, 255);
  } else {
     // Lejos o nada detectado -> Volumen máximo (comportamiento típico si quitas la mano)
     // O volumen 0 si queremos que se calle si no estás tocando.
     // Para este proyecto DIY, mejor que suene si no hay mano cerca?
     // Probemos: Si no detecta nada (timeout), volumen máximo.
     // Si detecta "muy lejos" (>50), volumen máximo.
     targetVol = 255;
  }

  // Aplicar volumen maestro
  targetVol = (targetVol * masterVolume) / 255.0;

  // Suavizado de volumen para evitar clics
  filteredVol = 0.6 * filteredVol + 0.4 * targetVol;
  synth.setVolume((int)filteredVol);

  // 5. Actualizar Pantalla (No bloqueante)
  if (millis() - lastDisplayUpdate > displayInterval) {
    lastDisplayUpdate = millis();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("Theremin ESP32"));

    display.print(F("Onda: "));
    switch (synthType) {
      case 0: display.println(F("Seno")); break;
      case 1: display.println(F("Cuadrada")); break;
      case 2: display.println(F("Sierra")); break;
      case 3: display.println(F("Triangulo")); break;
    }

    display.print(F("Freq: "));
    display.print((int)filteredFreq);
    display.println(F(" Hz"));

    display.print(F("Vol: "));
    display.print((int)filteredVol);
    display.print(F(" / "));
    display.println(masterVolume);

    display.display();
  }

  // Pequeño delay para no saturar las lecturas de ultrasonido
  // Ultrasonido requiere cierto tiempo entre pings para disipar ecos
  delay(30);
}
