#include "general.h"
#include "espnow.h"
#include "am1001.h"
#include "gp2y10.h"

void setup() {
    initGeneral();
    initSensors();        // AM1001 + GP2Y10 init
    initDustSensor();
    initESPNow();         // ESP-NOW init

    readAndSendData();    // Read sensors + send to master
    delay(200);           // Ensure ESP-NOW transmission completes

    //goDeepSleep(SAMPLE_INTERVAL_SEC);
}

void loop() {
    // Empty: all work in setup + deep sleep
}