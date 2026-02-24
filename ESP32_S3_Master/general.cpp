#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
#include "serverconfig.h"
#include "debugmode.h"
#include "datatypes.h"
#include <esp_sleep.h>
#include <WiFi.h>
#include <time.h>

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
    DEBUG_MODE = true;   // Disable by setting false

    DEBUG_PRINTLN("===== MASTER BOOT =====");
}

//------------------------------------------------------------
// Get current timestamp from NTP
//------------------------------------------------------------
uint32_t getTimestamp() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("Failed to obtain time");
        return 0;
    }
    return mktime(&timeinfo);
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
    localData.timestamp = 0; // will assign after NTP
    storeLocalReading(localData);

    DEBUG_PRINTF("Master Data -> Node: %d, Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                 localData.nodeId, localData.temperature / 100.0, localData.humidity / 100.0, localData.dust / 10.0);

    // 2. Wait for Slave Nodes
    waitForSlavesSmart(SLAVE_TIMEOUT_MS);

    for (uint8_t i = 0; i < receivedCount; i++) {
        DEBUG_PRINTF("Slave Data -> Node: %d, Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                     receivedData[i].nodeId,
                     receivedData[i].temperature / 100.0,
                     receivedData[i].humidity / 100.0,
                     receivedData[i].dust / 10.0);
    }

    // 3. Connect WiFi and sync time
    DEBUG_PRINTLN("Checking network for time sync...");

    if (isNetworkAvailable()) {
        configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, NTP_SERVER);
        delay(1000);

        uint32_t now = getTimestamp();
        if (now != 0) {
            DEBUG_PRINTF("Time synced: %lu\n", now);

            localData.timestamp = now;
            for (uint8_t i = 0; i < receivedCount; i++) {
                receivedData[i].timestamp = now;
            }
        }

        // Build array of all data
        SensorData allData[receivedCount + 1];
        allData[0] = localData;
        for (uint8_t i = 0; i < receivedCount; i++) {
            allData[i + 1] = receivedData[i];
        }

        // Upload directly
        if (uploadQueuedData(allData, receivedCount + 1)) {
            DEBUG_PRINTLN("Upload successful.");
        } else {
            DEBUG_PRINTLN("Upload failed.");
        }

    } else {
        DEBUG_PRINTLN("Network unavailable. Data cannot be uploaded.");
    }

    // Clear receive buffer
    receivedCount = 0;

    // 4. Deep Sleep
    DEBUG_PRINTF("Sleeping for %d seconds...\n", SAMPLE_INTERVAL_SEC);
    esp_sleep_enable_timer_wakeup((uint64_t)SAMPLE_INTERVAL_SEC * 1000000ULL);
    esp_deep_sleep_start();
}