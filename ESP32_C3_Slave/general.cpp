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

#define NTP_SERVER      "pool.ntp.org"
#define GMT_OFFSET_SEC  19800     
#define DAYLIGHT_OFFSET 0
#define SLAVE_TIMEOUT_MS 10000

void initGeneral() {
    Serial.begin(115200);
    
    // C3 specific: Wait for USB Serial to connect
    unsigned long start = millis();
    while (!Serial && millis() - start < 3000); 
    
    DEBUG_MODE = true; 
    DEBUG_PRINTLN("\n===== ESP32-C3 MASTER BOOT =====");

    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, NTP_SERVER);
}

uint32_t getValidTimestamp(uint32_t timeoutMs) {
    uint32_t start = millis();
    struct tm timeinfo;
    while (millis() - start < timeoutMs) {
        if (getLocalTime(&timeinfo)) {
            if (timeinfo.tm_year >= 120) { 
                return mktime(&timeinfo);
            }
        }
        delay(200); // Allow single-core background tasks
    }
    DEBUG_PRINTLN("DEBUG: Failed to obtain valid NTP time");
    return 0;
}

void waitForSlavesSmart(uint32_t timeoutMs) {
    DEBUG_PRINTLN("DEBUG: Waiting for slave nodes...");
    uint32_t start = millis();
    uint8_t lastCount = receivedCount;

    while (millis() - start < timeoutMs) {
        if (receivedCount != lastCount) {
            lastCount = receivedCount;
            start = millis(); 
        }
        delay(50); // Prevent watchdog timeout on single core
    }
    DEBUG_PRINTF("DEBUG: Slave collection complete. Total: %d\n", receivedCount);
}

void generalRun() {
    DEBUG_PRINTLN("--- Starting sampling cycle ---");

    // 1. Read Master Local Sensors
    SensorData localData;
    localData.nodeId = 2;
    localData.temperature = readTemperature();
    delay(50); 
    localData.humidity = readHumidity();
    delay(50);
    localData.dust = readDust();
    localData.timestamp = 0; 
    
    // Show local data for debugging
    DEBUG_PRINTF("LOCAL DATA -> Temp: %.2f, Hum: %.2f, Dust: %.2f\n", 
                 localData.temperature / 100.0, 
                 localData.humidity / 100.0, 
                 localData.dust / 10.0);

    storeLocalReading(localData);

    // 2. Wait for Slave Nodes
    waitForSlavesSmart(SLAVE_TIMEOUT_MS);

    // 3. Network Check
    if (!isNetworkAvailable()) {
        DEBUG_PRINTLN("DEBUG: Network unavailable. Upload aborted.");
        return; 
    }

    // 4. Time Sync
    uint32_t now = getValidTimestamp(10000);
    if (now == 0) {
        DEBUG_PRINTLN("DEBUG: Time sync failed. Aborting upload.");
        return;
    }

    localData.timestamp = now;
    for (uint8_t i = 0; i < receivedCount; i++) {
        receivedData[i].timestamp = now;
    }

    // 5. Data Array Management
    int totalNodes = receivedCount + 1;
    SensorData allData[totalNodes]; 
    
    allData[0] = localData;
    for (uint8_t i = 0; i < receivedCount; i++) {
        allData[i + 1] = receivedData[i];
    }

    // 6. Upload with retry
    int retryCount = 0;
    bool success = false;
    while (retryCount < 3 && !success) {
        DEBUG_PRINTF("DEBUG: Upload attempt %d...\n", retryCount + 1);
        success = uploadQueuedData(allData, totalNodes);
        if (!success) {
            retryCount++;
            delay(2000); 
        }
    }

    if (success) {
        DEBUG_PRINTLN("SUCCESS: Data uploaded to InfluxDB.");
    } else {
        DEBUG_PRINTLN("ERROR: Upload failed after retries.");
    }
    
    // 7. Reset for next cycle
    receivedCount = 0;
    DEBUG_PRINTLN("--- Cycle Finished ---\n");
}