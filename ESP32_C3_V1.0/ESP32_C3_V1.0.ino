#include "general.h"
#include "SHT30.h"         
#include "ADS1115_Dust.h"  
#include "serverconfig.h"

unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_GAP = 3000; // 3 sec x 5 samples = 15 sec total

void setup() {
    initGeneral();
    initSHT30();           
    initDustSensor();      
}

void loop() {
    feedWatchdog();
    if (millis() - lastSampleTime >= SAMPLE_GAP) {
        lastSampleTime = millis();
        collectSample();
    }
}