#include "general.h"
#include "SHT30.h"         
#include "ADS1115_Dust.h"  
#include "serverconfig.h"

unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_GAP = 15000; //  sec x 5 samples

void setup() {
    initGeneral();
    initSHT30();           
    initDustSensor();     
}

void loop() {
    feedWatchdog();
    if (millis() - lastSampleTime >= SAMPLE_GAP) {
        lastSampleTime += SAMPLE_GAP; 
        collectSample();
    }
}