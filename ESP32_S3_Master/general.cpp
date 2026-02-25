#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
// espnow.h removed
#include "serverconfig.h"
#include "debugmode.h"
#include "datatypes.h"
#include <WiFi.h>
#include <time.h>
#include <InfluxDbClient.h>

// NTP Settings
#define NTP_SERVER      "pool.ntp.org"
#define GMT_OFFSET_SEC  19800     // Sri Lanka = UTC+5:30
#define DAYLIGHT_OFFSET 0

//------------------------------------------------------------
// Initialize system
//------------------------------------------------------------
void initGeneral() {
    Serial.begin(115200);
    delay(100);
    DEBUG_MODE = true;   // Enable debug prints

    DEBUG_PRINTLN("===== MASTER BOOT (SINGLE NODE) =====");

    // Start NTP background sync immediately
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, NTP_SERVER);
}

//------------------------------------------------------------
// Wait until NTP time is valid
//------------------------------------------------------------
uint32_t getValidTimestamp(uint32_t timeoutMs = 10000) {
    uint32_t start = millis();
    struct tm timeinfo;
    while (millis() - start < timeoutMs) {
        if (getLocalTime(&timeinfo)) {
            if (timeinfo.tm_year >= 120) { 
                return mktime(&timeinfo);
            }
        }
        delay(200);
    }
    DEBUG_PRINTLN("Failed to obtain valid NTP time");
    return 0;
}

//------------------------------------------------------------
// Main logic
//------------------------------------------------------------
void generalRun() {
    DEBUG_PRINTLN("Starting sampling cycle...");

    // 1. Read Local Sensors
    SensorData localData;
    localData.nodeId = 0;
    localData.temperature = readTemperature();
    delay(50);
    localData.humidity = readHumidity();
    delay(50);
    localData.dust = readDust();
    localData.timestamp = 0; 

    DEBUG_PRINTF("Local Data -> Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                  localData.temperature / 100.0,
                  localData.humidity / 100.0,
                  localData.dust / 10.0);

    // 2. Ensure network is available
    DEBUG_PRINTLN("Checking network for time sync...");
    if (!isNetworkAvailable()) {
        DEBUG_PRINTLN("Network unavailable. Upload aborted.");
        return; 
    }

    // 3. Ensure NTP time is valid
    uint32_t now = getValidTimestamp(10000); 
    if (now == 0) {
        DEBUG_PRINTLN("Time sync failed. Upload aborted.");
        return;
    }
    DEBUG_PRINTF("Time synced: %lu\n", now);

    localData.timestamp = now;

    // 4. Build array for upload (keeping array format for compatibility)
    SensorData allData[1];
    allData[0] = localData;

    // 5. Upload with retry loop
    int retryCount = 0;
    const int maxRetries = 3;
    bool success = false;

    while (retryCount < maxRetries && !success) {
        // Uploading 1 record (local only)
        success = uploadQueuedData(allData, 1);
        if (!success) {
            DEBUG_PRINTF("Upload failed. Retrying (%d/%d)...\n",
                          retryCount + 1, maxRetries);
            retryCount++;
            delay(2000); 
        }
    }

    if (success) {
        DEBUG_PRINTLN("Upload successful.");
    } else {
        DEBUG_PRINTLN("Upload failed after retries.");
    }
}