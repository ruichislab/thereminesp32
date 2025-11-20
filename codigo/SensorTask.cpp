#include "SensorTask.h"

volatile SensorData sharedSensorData = {0.0, 0.0};
UltrasonicSensor* ptrSensorTone = NULL;
UltrasonicSensor* ptrSensorVolume = NULL;

void sensorTask(void * parameter) {
    while(true) {
        if (ptrSensorTone && ptrSensorVolume) {
            // Read Tone Sensor
            float dt = ptrSensorTone->readDistance();
            // Read Volume Sensor
            float dv = ptrSensorVolume->readDistance();

            // Update shared data atomically (float write is atomic on ESP32 usually, but let's be simple)
            // For absolute safety we could use a mutex, but for this application
            // occasional race condition on a single float isn't catastrophic.
            // We just assign them.
            sharedSensorData.distanceTone = dt;
            sharedSensorData.distanceVolume = dv;
        }

        // Sleep a bit to yield and control sample rate (e.g., 20-30ms)
        // 30ms ~= 33Hz update rate, plenty for hand gestures
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}
