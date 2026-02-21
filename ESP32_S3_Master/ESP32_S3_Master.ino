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

    // Run the main logic loop once
    generalRun();
}

void loop() {
    // Nothing here, everything handled in generalRun()
}