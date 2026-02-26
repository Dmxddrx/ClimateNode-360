#include "general.h"
#include "SHT30.h"         
#include "ADS1115_Dust.h"  
#include "serverconfig.h"
#include "debugmode.h"
#include "datatypes.h"
#include <WiFi.h>
#include <Wire.h>
#include <esp_task_wdt.h>

#define MAX_SAMPLES 5 

static float tempSum = 0, humSum = 0, dustSum = 0;
static int sampleCount = 0;

void initGeneral() {
    Serial.begin(115200);
    delay(100);
    DEBUG_MODE = true; 

    Wire.begin(6, 7);          // INIT I2C ONLY ONCE
    Wire.setClock(100000);
    Wire.setTimeOut(50);

    esp_task_wdt_deinit(); 
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 30000, 
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, 
        .trigger_panic = true 
    };
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL);

    DEBUG_PRINTLN("===== SYSTEM START (C3 STABILITY MODE) =====");
}

void feedWatchdog() { esp_task_wdt_reset(); }

void collectSample() {
    int16_t rawTemp = readTemperature(); 
    delay(10);
    int16_t rawHum  = readHumidity();    
    delay(10);
    int16_t rawDust = readDust();      

    tempSum += rawTemp;
    humSum  += rawHum;
    dustSum += rawDust;
    sampleCount++;

    DEBUG_PRINTF("Sample #%d/%d Captured.\n", sampleCount, MAX_SAMPLES);

    if (sampleCount >= MAX_SAMPLES) {
        uploadAverage();
    }
}

void uploadAverage() {
    DEBUG_PRINTLN("Target reached. Muting I2C for WiFi...");


    SensorData localData;
    localData.nodeId = 1;
    localData.temperature = (int32_t)(tempSum / sampleCount);
    localData.humidity    = (int32_t)(humSum / sampleCount);
    localData.dust        = (int32_t)(dustSum / sampleCount);

    if (uploadQueuedData(&localData, 1)) {
        DEBUG_PRINTLN("Upload Success.");
    } else {
        DEBUG_PRINTLN("Upload Failed.");
    }


    tempSum = 0; humSum = 0; dustSum = 0; sampleCount = 0;
}