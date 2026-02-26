#include "general.h"
#include "SHT30.h"
#include "ADS1115_Dust.h"
#include "serverconfig.h"
#include "debugmode.h"
#include "datatypes.h"
#include <WiFi.h>

static float tempSum = 0, humSum = 0, dustSum = 0;
static int sampleCount = 0;

void initGeneral() {
    Serial.begin(115200);
    delay(100);
    DEBUG_MODE = true; 

    DEBUG_PRINTLN("===== MASTER BOOT (C3 - I2C UPGRADE) =====");
    
    initSHT30();        
    initDustSensor();   

    // NEW: Start WiFi connection immediately at boot
    DEBUG_PRINTLN("Starting WiFi in background...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void collectSample() {
    // Read raw values
    int16_t rawTemp = readTemperature();
    delay(10);
    int16_t rawHum = readHumidity();
    delay(10);
    int16_t rawDust = readDust();

    // Update sums
    tempSum += rawTemp;
    humSum += rawHum;
    dustSum += rawDust;
    sampleCount++;

    // Print Header
    DEBUG_PRINTF("Sample #%d captured.\n", sampleCount);
    
    // Print RAW and Converted data
    // Temp: raw 2550 -> 25.50 C | Hum: raw 6050 -> 60.50 % | Dust: raw 355 -> 35.5 ug/m3
    DEBUG_PRINTF("  [RAW] T:%d, H:%d, D:%d\n", rawTemp, rawHum, rawDust);
    DEBUG_PRINTF("  [CONV] Temp: %.2f C, Hum: %.2f %%, Dust: %.1f ug/m3\n", 
                 rawTemp / 100.0, rawHum / 100.0, rawDust / 10.0);
    DEBUG_PRINTLN("-----------------------------------------");
}

void uploadAverage() {
    if (sampleCount == 0) return;

    DEBUG_PRINTLN("Processing 60s average and uploading...");

    SensorData localData;
    localData.nodeId = 0;
    
    localData.temperature = (int32_t)(tempSum / sampleCount);
    localData.humidity    = (int32_t)(humSum / sampleCount);
    localData.dust        = (int32_t)(dustSum / sampleCount);
    localData.timestamp   = 0; // Kept at 0 so it doesn't break datatypes.h

    DEBUG_PRINTF("Avg Data -> Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                  localData.temperature / 100.0,
                  localData.humidity / 100.0,
                  localData.dust / 10.0);

    if (!isNetworkAvailable()) {
        DEBUG_PRINTLN("Network unavailable. Upload aborted.");
    } else {
        SensorData allData[1];
        allData[0] = localData;

        int retryCount = 0;
        const int maxRetries = 3;
        bool success = false;

        // Instantly upload without waiting for NTP sync
        while (retryCount < maxRetries && !success) {
            success = uploadQueuedData(allData, 1);
            if (!success) {
                retryCount++;
                delay(2000); 
            }
        }
        if (success) DEBUG_PRINTLN("Upload successful.");
    }

    tempSum = 0;
    humSum = 0;
    dustSum = 0;
    sampleCount = 0;
}