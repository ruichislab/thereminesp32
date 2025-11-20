# Guía de Construcción y Uso: Theremin ESP32 Profesional

Bienvenido a la guía oficial para construir tu propio Theremin Profesional basado en ESP32. Este proyecto ha sido diseñado para ofrecer una respuesta rápida, sonido continuo de alta calidad y características avanzadas como afinación automática y calibración persistente.

---

## 1. Lista de Materiales (Hardware)

Para construir este theremin necesitarás los siguientes componentes:

| Componente | Cantidad | Descripción |
| :--- | :---: | :--- |
| **ESP32 Dev Kit V1** | 1 | El cerebro del proyecto (30 o 38 pines). |
| **Sensor Ultrasónico HC-SR04** | 2 | Uno para el **Tono** (Frecuencia) y otro para el **Volumen**. |
| **Pantalla OLED I2C** | 1 | Modelo SSD1306 (0.96 pulgadas), resolución 128x64. |
| **Potenciómetro** | 2 | De 10kΩ o 100kΩ lineales (B10K). |
| **Botón (Pulsador)** | 1 | Pulsador momentáneo estándar. |
| **Altavoz o Jack de Audio** | 1 | Pequeño altavoz de 8Ω con amplificador (ej. PAM8403) o un jack hembra de 3.5mm para conectar a altavoces externos. |
| **Protoboard y Cables** | 1 | Para realizar las conexiones. |
| **Condensador (Opcional)** | 1 | 100µF o similar, para filtrar la alimentación si hay ruido. |

---

## 2. Diagrama de Conexiones

Es crucial respetar estos pines, ya que el código está configurado específicamente para ellos.

### Sensores y Actuadores

| Dispositivo | Pin del Dispositivo | Pin ESP32 (GPIO) | Función |
| :--- | :--- | :--- | :--- |
| **Sensor Tono (HC-SR04)** | Trig | **12** | Emite pulso ultrasónico |
| | Echo | **13** | Recibe eco (Distancia = Tono) |
| | VCC | 5V (Vin) | Alimentación |
| | GND | GND | Tierra |
| **Sensor Volumen (HC-SR04)** | Trig | **14** | Emite pulso ultrasónico |
| | Echo | **27** | Recibe eco (Distancia = Volumen) |
| | VCC | 5V (Vin) | Alimentación |
| | GND | GND | Tierra |
| **Potenciómetro 1** | Pin Central (Wiper) | **34** | Selector de Onda (Seno, Cuadrada...) |
| **Potenciómetro 2** | Pin Central (Wiper) | **35** | Volumen Maestro / Sensibilidad |
| **Botón** | Un lado | **32** | Botón de Modo / Calibración |
| | Otro lado | GND | Tierra |
| **Audio (Salida)** | Entrada Audio (+) | **25** | Salida DAC (Sonido) |
| | Entrada Audio (-) | GND | Tierra |

**Nota sobre Audio:** El pin 25 es un DAC (Conversor Digital-Analógico) real. Produce una señal de 0 a 3.3V. **NO** conectes un altavoz directamente a este pin sin una resistencia o amplificador, podrías dañar el ESP32. Lo ideal es usar un pequeño módulo amplificador (PAM8403) o conectar un Jack de audio hacia unos altavoces de PC.

### Pantalla OLED (I2C)

| Pin OLED | Pin ESP32 |
| :--- | :--- |
| **SDA** | **21** |
| **SCL** | **22** |
| **VCC** | 3.3V |
| **GND** | GND |

---

## 3. Instalación del Software

### Requisitos Previos
1.  Descarga e instala el [Arduino IDE](https://www.arduino.cc/en/software).
2.  Instala el soporte para ESP32 en el Gestor de Tarjetas (Board Manager) de Arduino.
    *   *URL Adicional:* `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

### Librerías Necesarias
En el Arduino IDE, ve a **Herramientas -> Administrar Bibliotecas** e instala:
1.  **Adafruit GFX Library**
2.  **Adafruit SSD1306**

### Subir el Código
1.  Abre el archivo `Theremin_ESP32.ino` ubicado en la carpeta `codigo/`.
2.  Selecciona tu placa: **Tools -> Board -> ESP32 Dev Module**.
3.  Conecta el ESP32 por USB.
4.  Dale al botón **Subir (Upload)**.

---

## 4. Guía de Uso

Una vez encendido, verás el logo "Theremin PRO" en la pantalla.

### Calibración (¡Muy Importante!)
Cada habitación es diferente. La primera vez que lo uses (o si cambias de sitio), debes calibrarlo para que sepa dónde están tus manos.

1.  Mantén presionado el **Botón (Pin 32)** durante **2 segundos** hasta que la pantalla diga "CALIBRACION".
2.  Sigue las instrucciones en pantalla (tienes 5 segundos para prepararte entre pasos):
    *   **Paso 1:** Acerca tu mano al sensor de **Tono** (aprox. 5cm) para registrar la nota más aguda.
    *   **Paso 2:** Aleja tu mano del sensor de **Tono** (hasta donde quieras llegar, ej. 50cm) para la nota más grave.
    *   **Paso 3:** Acerca tu mano al sensor de **Volumen** (tapar el sensor) para definir el silencio.
    *   **Paso 4:** Aleja tu mano del sensor de **Volumen** para definir el volumen máximo.
3.  ¡Listo! La configuración se guardará en la memoria permanente. No necesitas hacerlo cada vez que lo enciendas.

### Modos de Juego
Pulsa el **Botón** brevemente (clic rápido) para cambiar entre modos:
*   **Modo FREE (Libre):** El theremin clásico. El tono cambia suavemente y de forma continua (como un violín o una sirena). Ideal para efectos de sonido y glissandos.
*   **Modo MUSICAL (Cuantizado):** El theremin "afina" automáticamente tu mano a la nota musical más cercana (Do, Re, Mi...). Ideal para tocar melodías sin desafinar. La pantalla mostrará la nota actual (ej. "C#4").

### Controles en Vivo
*   **Mano Izquierda (Sensor Vol):** Controla la intensidad del sonido.
*   **Mano Derecha (Sensor Tono):** Controla la altura de la nota.
*   **Potenciómetro 1:** Gira para cambiar el tipo de sonido (Onda Senoidal, Cuadrada, Diente de Sierra, Triangular).
*   **Potenciómetro 2:** Volumen Maestro. Limita el volumen máximo de salida (útil si usas auriculares).

---

## 5. Cómo Funciona (Para Curiosos)

Este código es mucho más avanzado que un sketch de Arduino normal:

1.  **Multitarea (FreeRTOS):** El ESP32 tiene dos núcleos.
    *   El **Núcleo 0** se encarga exclusivamente de leer los sensores ultrasónicos. Esto es vital porque los sensores son lentos (tienen que esperar al eco del sonido). Si hiciéramos esto en el bucle principal, el audio se cortaría.
    *   El **Núcleo 1** maneja la pantalla, los botones y la lógica musical.
2.  **Síntesis Wavetable por Interrupciones:** El sonido no se genera con `tone()`. Usamos un **Timer de Hardware** que se dispara 22,000 veces por segundo. Cada vez que se dispara, el código busca el siguiente punto de la onda en una tabla pre-calculada y lo envía al DAC. Esto garantiza un sonido fluido y "analógico".
3.  **Mapeo Flotante:** A diferencia de otros proyectos que usan números enteros, este usa matemáticas de punto flotante (`float`) para calcular la frecuencia, permitiendo una suavidad infinita entre notas.

¡Disfruta de tu nuevo instrumento musical profesional!
