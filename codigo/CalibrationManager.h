#ifndef CALIBRATION_MANAGER_H
#define CALIBRATION_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class CalibrationManager {
public:
    // Default values
    float minDistTone = 5.0;
    float maxDistTone = 50.0;
    float minDistVol = 5.0;
    float maxDistVol = 40.0;

    void init() {
        preferences.begin("theremin", false); // Read-write mode
        loadCalibration();
    }

    void loadCalibration() {
        minDistTone = preferences.getFloat("minTone", 5.0);
        maxDistTone = preferences.getFloat("maxTone", 50.0);
        minDistVol = preferences.getFloat("minVol", 5.0);
        maxDistVol = preferences.getFloat("maxVol", 40.0);
    }

    void saveCalibration(float minT, float maxT, float minV, float maxV) {
        minDistTone = minT;
        maxDistTone = maxT;
        minDistVol = minV;
        maxDistVol = maxV;

        preferences.putFloat("minTone", minDistTone);
        preferences.putFloat("maxTone", maxDistTone);
        preferences.putFloat("minVol", minDistVol);
        preferences.putFloat("maxVol", maxDistVol);

        Serial.println("Calibration Saved!");
    }

    void resetToDefaults() {
        preferences.clear();
        minDistTone = 5.0; maxDistTone = 50.0;
        minDistVol = 5.0; maxDistVol = 40.0;
        loadCalibration();
    }

private:
    Preferences preferences;
};

#endif
