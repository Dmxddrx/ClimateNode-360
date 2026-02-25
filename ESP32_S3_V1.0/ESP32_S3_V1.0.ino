#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "serverconfig.h"
#include "datatypes.h"

unsigned long lastSampleTime = 0;
unsigned long lastUploadTime = 0;

void setup() {
    initGeneral();
    initSensors();
    initDustSensor();
}

void loop() {
    uint32_t currentTime = millis();

    // 1. Collect data every 5 seconds
    if (currentTime - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = currentTime;
        collectSample();
    }

    // 2. Upload average every 60 seconds
    if (currentTime - lastUploadTime >= UPLOAD_INTERVAL_MS) {
        lastUploadTime = currentTime;
        uploadAverage();
    }

    delay(10);
}