# Proyecto Theremin con ESP32 (Versión Profesional)

¡Bienvenido! Este repositorio contiene el código fuente y la documentación para construir un **Theremin basado en ESP32** de alta calidad.

A diferencia de los proyectos básicos de hobby, esta versión utiliza características avanzadas del ESP32 para comportarse como un verdadero instrumento musical.

## 🚀 Características Principales

*   **Audio Continuo sin Interrupciones:** Utiliza síntesis por tabla de ondas (Wavetable Synthesis) e interrupciones de hardware (Timer ISR) para un sonido suave y fluido.
*   **Multitarea Real (FreeRTOS):** La lectura de sensores se realiza en un núcleo separado para evitar retrasos (lag) en el audio o la interfaz.
*   **Modo Musical (Autotune):** Incluye un modo cuantizado que "afina" las notas automáticamente para facilitar la ejecución de melodías.
*   **Calibración Persistente:** Un sistema de calibración guarda los rangos de tus sensores en la memoria flash, adaptándose a cualquier entorno.
*   **Interfaz Visual:** Pantalla OLED que muestra la nota musical actual, frecuencia, barra de volumen y tipo de onda.

## 📚 Documentación

Para instrucciones detalladas de construcción, cableado y uso, consulta el manual completo:

👉 **[LEER EL MANUAL DE USUARIO (MANUAL_DE_USUARIO.md)](./MANUAL_DE_USUARIO.md)**

## 🛠️ Estructura del Proyecto

*   `codigo/`: Contiene el sketch de Arduino (`.ino`) y las clases C++ (`.cpp`, `.h`).
*   `MANUAL_DE_USUARIO.md`: Guía paso a paso, lista de materiales y diagrama de conexión.

## ⚡ Inicio Rápido

1.  Clona este repositorio.
2.  Abre `codigo/Theremin_ESP32.ino` en Arduino IDE.
3.  Instala las librerías `Adafruit SSD1306` y `Adafruit GFX`.
4.  Sube el código a tu ESP32.
5.  ¡Sigue las instrucciones de calibración en el Manual!

---
*Proyecto mejorado con arquitectura profesional de sistemas embebidos.*
