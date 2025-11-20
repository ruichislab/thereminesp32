#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <Arduino.h>
#include "UltrasonicSensor.h"

// Structure to share data between tasks safely
struct SensorData {
    float distanceTone;
    float distanceVolume;
};

// Global instance to access from task
extern volatile SensorData sharedSensorData;
extern UltrasonicSensor* ptrSensorTone;
extern UltrasonicSensor* ptrSensorVolume;

void sensorTask(void * parameter);

#endif
