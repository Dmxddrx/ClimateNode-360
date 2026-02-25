#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
//#include "sdcard.h"
#include "serverconfig.h"
#include "datatypes.h"

void setup() {
    // The C3 often needs a slight delay for Serial to stabilize if using built-in USB
    delay(1000); 

    // Initialize system peripherals
    initGeneral();
    initSensors();
    initDustSensor();
    //initSD();
    initESPNow();
}

void loop() {
    static uint32_t lastSampleTime = millis();
    uint32_t currentTime = millis();

    // Run generalRun every 30 seconds
    if (currentTime - lastSampleTime >= 30000) {
        lastSampleTime = currentTime;
        generalRun();
    }

    // Critical for C3: yield() or delay() ensures the 
    // background WiFi/ESP-Now stacks don't starve on the single core.
    yield();
    delay(10);
}