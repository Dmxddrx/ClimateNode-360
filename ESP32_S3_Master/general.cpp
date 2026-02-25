#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
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

// Slave timeout window
#define SLAVE_TIMEOUT_MS 10000

//------------------------------------------------------------
// Initialize system
//------------------------------------------------------------
void initGeneral() {
    Serial.begin(115200);
    delay(100);
    DEBUG_MODE = true;   // Enable debug prints

    DEBUG_PRINTLN("===== MASTER BOOT =====");

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
// Wait until all slaves send OR timeout
//------------------------------------------------------------
void waitForSlavesSmart(uint32_t timeoutMs) {
    DEBUG_PRINTLN("Waiting for slave nodes...");

    uint32_t start = millis();
    uint8_t lastCount = receivedCount;

    while (millis() - start < timeoutMs) {
        if (receivedCount != lastCount) {
            lastCount = receivedCount;
            start = millis();  // extend window
        }
        delay(50);
    }

    DEBUG_PRINTF("Slave collection complete. Total nodes: %d\n", receivedCount);
}

//------------------------------------------------------------
// Main logic
//------------------------------------------------------------
void generalRun() {
    DEBUG_PRINTLN("Starting sampling cycle...");

    // 1. Read Master Local Sensors
    SensorData localData;
    localData.nodeId = 0;
    localData.temperature = readTemperature();
    delay(50);
    localData.humidity = readHumidity();
    delay(50);
    localData.dust = readDust();
    localData.timestamp = 0; 
    
    // storeLocalReading(localData); // Removed SD storage call

    DEBUG_PRINTF("Master Data -> Node: %d, Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                  localData.nodeId,
                  localData.temperature / 100.0,
                  localData.humidity / 100.0,
                  localData.dust / 10.0);

    // 2. Wait for Slave Nodes
    waitForSlavesSmart(SLAVE_TIMEOUT_MS);

    for (uint8_t i = 0; i < receivedCount; i++) {
        DEBUG_PRINTF("Slave Data -> Node: %d, Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                      receivedData[i].nodeId,
                      receivedData[i].temperature / 100.0,
                      receivedData[i].humidity / 100.0,
                      receivedData[i].dust / 10.0);
    }

    // 3. Ensure network is available
    DEBUG_PRINTLN("Checking network for time sync...");
    if (!isNetworkAvailable()) {
        DEBUG_PRINTLN("Network unavailable. Upload aborted.");
        return; 
    }

    // 4. Ensure NTP time is valid
    uint32_t now = getValidTimestamp(10000); 
    if (now == 0) {
        DEBUG_PRINTLN("Time sync failed. Upload aborted.");
        return;
    }
    DEBUG_PRINTF("Time synced: %lu\n", now);

    // Assign timestamp to all data
    localData.timestamp = now;
    for (uint8_t i = 0; i < receivedCount; i++) {
        receivedData[i].timestamp = now;
    }

    // 5. Build array of all data
    SensorData allData[receivedCount + 1];
    allData[0] = localData;
    for (uint8_t i = 0; i < receivedCount; i++) {
        allData[i + 1] = receivedData[i];
    }

    // 6. Upload with retry loop
    int retryCount = 0;
    const int maxRetries = 3;
    bool success = false;

    while (retryCount < maxRetries && !success) {
        success = uploadQueuedData(allData, receivedCount + 1);
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

    // 7. Clear receive buffer
    receivedCount = 0;
}