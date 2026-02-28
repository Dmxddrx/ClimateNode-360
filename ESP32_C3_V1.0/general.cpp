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
#define SKIP_UPLOADS 4  // number of uploads to skip dust measurement

static float tempSum = 0, humSum = 0, dustSum = 0;
static int sampleCount = 0;
static int uploadCounter = 0;  // track how many uploads done
static int32_t lastDustAverage = 0;

void initGeneral() {
    Serial.begin(115200); 
    delay(100);
    DEBUG_MODE = true; 

    /*
    ESP32 Reset Reasons:

    1  = ESP_RST_POWERON      // Power-on reset
    2  = ESP_RST_EXT          // External reset (RST pin)
    3  = ESP_RST_SW           // Software reset (esp_restart)
    4  = ESP_RST_PANIC        // Exception / Guru Meditation
    5  = ESP_RST_INT_WDT      // Interrupt watchdog
    6  = ESP_RST_TASK_WDT     // Task watchdog
    7  = ESP_RST_WDT          // Other watchdog
    8  = ESP_RST_DEEPSLEEP    // Wake from deep sleep
    9  = ESP_RST_BROWNOUT     // Brownout (low voltage)
    10 = ESP_RST_SDIO         // SDIO reset
    11 = ESP_RST_USB          // USB reset
    12 = ESP_RST_JTAG         // JTAG reset    */

    esp_reset_reason_t reason = esp_reset_reason();
    DEBUG_PRINTF("Reset reason: %d\n", reason);

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

    int16_t rawDust;

    // Only measure dust during first 5 samples of cycle
    if (uploadCounter % (SKIP_UPLOADS + 1) == 0) {
        rawDust = readDust();  // active measurement
    } else {
        rawDust = lastDustAverage;  // use last known average
    }    

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
    localData.nodeId = 2;
    localData.temperature = (int32_t)(tempSum / sampleCount);
    localData.humidity    = (int32_t)(humSum / sampleCount);
    localData.dust        = (int32_t)(dustSum / sampleCount);

        // Store dust average ONLY when this is real measurement cycle
    if (uploadCounter % (SKIP_UPLOADS + 1) == 0) {
        lastDustAverage = localData.dust;
        //DEBUG_PRINTLN("New Dust Average Stored.");
    } else {
        //DEBUG_PRINTLN("Reusing Previous Dust Average.");
    }

    if (uploadQueuedData(&localData, 1)) {
        DEBUG_PRINTLN("Upload Success.");
    } else {
        DEBUG_PRINTLN("Upload Failed.");
    }


    tempSum = 0; humSum = 0; dustSum = 0; sampleCount = 0;
    uploadCounter++;
}