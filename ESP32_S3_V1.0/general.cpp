#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "serverconfig.h"
#include "debugmode.h"
#include "datatypes.h"
#include <WiFi.h>
#include <time.h>
#include <esp_task_wdt.h> // Watchdog Library

#define NTP_SERVER      "pool.ntp.org"
#define GMT_OFFSET_SEC  19800 
#define DAYLIGHT_OFFSET 0
#define WDT_TIMEOUT_S   30 // 30-second window before reset

static float tempSum = 0, humSum = 0, dustSum = 0;
static int sampleCount = 0;

void initGeneral() {
    Serial.begin(115200);
    delay(100);
    DEBUG_MODE = true; 

    // Initialize Watchdog Timer
    DEBUG_PRINTLN("Initializing Watchdog...");
    esp_task_wdt_init(WDT_TIMEOUT_S, true); 
    esp_task_wdt_add(NULL); 

    DEBUG_PRINTLN("===== MASTER BOOT (NODE 2) =====");
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, NTP_SERVER);
}

void feedWatchdog() {
    esp_task_wdt_reset();
}

// ... getValidTimestamp() remains unchanged ...

void collectSample() {
    tempSum += readTemperature();
    delay(10);
    humSum += readHumidity();
    delay(10);
    dustSum += readDust();
    sampleCount++;

    DEBUG_PRINTF("Sample #%d captured.\n", sampleCount);
}

void uploadAverage() {
    if (sampleCount == 0) return;

    DEBUG_PRINTLN("Processing 60s average and uploading...");

    SensorData localData;
    localData.nodeId = 2; // Kept as Node 2
    
    localData.temperature = (int32_t)(tempSum / sampleCount);
    localData.humidity    = (int32_t)(humSum / sampleCount);
    localData.dust        = (int32_t)(dustSum / sampleCount);
    localData.timestamp   = 0;

    DEBUG_PRINTF("Avg Data -> Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                  localData.temperature / 100.0,
                  localData.humidity / 100.0,
                  localData.dust / 10.0);

    if (!isNetworkAvailable()) {
        DEBUG_PRINTLN("Network unavailable. Upload aborted.");
    } else {
        uint32_t now = getValidTimestamp(10000); 
        if (now != 0) {
            localData.timestamp = now;
            SensorData allData[1];
            allData[0] = localData;

            int retryCount = 0;
            const int maxRetries = 3;
            bool success = false;

            while (retryCount < maxRetries && !success) {
                // IMPORTANT: Feed watchdog during long retry loops
                feedWatchdog(); 
                success = uploadQueuedData(allData, 1);
                if (!success) {
                    retryCount++;
                    delay(2000); 
                }
            }
            if (success) DEBUG_PRINTLN("Upload successful.");
        }
    }

    tempSum = 0; humSum = 0; dustSum = 0; sampleCount = 0;
}