#include "general.h"
#include "SHT30.h"          // Updated header
#include "ADS1115_Dust.h"   // Updated header
#include "serverconfig.h"
#include "datatypes.h"

unsigned long lastSampleTime = 0;
unsigned long lastUploadTime = 0;

void setup() {
    // initGeneral now handles initSHT30() and initDustSensor() 
    // internally as per our updated general.cpp
    initGeneral();
}

void loop() {
    uint32_t currentTime = millis();

    // 1. Collect data every 5 seconds (Logic remains identical)
    if (currentTime - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = currentTime;
        collectSample();
    }

    // 2. Upload average every 60 seconds (Logic remains identical)
    if (currentTime - lastUploadTime >= UPLOAD_INTERVAL_MS) {
        lastUploadTime = currentTime;
        uploadAverage();
    }

    // Essential delay for ESP32-C3 Single-core stability
    delay(10); 
}