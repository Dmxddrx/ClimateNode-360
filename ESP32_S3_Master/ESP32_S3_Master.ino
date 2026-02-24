#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
#include "sdcard.h"
#include "serverconfig.h"
#include "datatypes.h"

void setup() {
    // Initialize system peripherals
    initGeneral();
    initSensors();
    initDustSensor();
    initSD();
    initESPNow();
}

void loop() {
    static uint32_t lastSampleTime = 0;
    uint32_t currentTime = millis();

    // Run generalRun every 15 seconds (15000 ms)
    if (currentTime - lastSampleTime >= 30000) {
        lastSampleTime = currentTime;
        generalRun();
    }

    // Small delay to prevent watchdog reset
    delay(10);
}