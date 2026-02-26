#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "serverconfig.h"
#include "debugmode.h"
#include "datatypes.h"
#include <WiFi.h>
#include <time.h>
#include <esp_task_wdt.h>

// NTP Settings
#define NTP_SERVER      "pool.ntp.org"
#define GMT_OFFSET_SEC  19800     // Sri Lanka = UTC+5:30
#define DAYLIGHT_OFFSET 0
#define WDT_TIMEOUT_S   30

// Static variables to store the sums for averaging
static float tempSum = 0, humSum = 0, dustSum = 0;
static int sampleCount = 0;

void initGeneral() {
    Serial.begin(115200);
    delay(100);
    DEBUG_MODE = true; 

    // Initialize Watchdog Timer
    DEBUG_PRINTLN("Initializing Watchdog...");
    
    // 1. Clear any default background watchdog tasks
    esp_task_wdt_deinit(); 
    
    // 2. Setup the new configuration structure
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = WDT_TIMEOUT_S * 1000, 
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, 
        .trigger_panic = true 
    };
    
    // 3. Apply the config and attach the current loop
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL);

    DEBUG_PRINTLN("===== MASTER BOOT (AVERAGING MODE) =====");
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, NTP_SERVER);
}

// Ensure you have this function defined either here or wherever you put it
void feedWatchdog() {
    esp_task_wdt_reset(); 
}

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

// Function to read sensors and add to the total
void collectSample() {
    tempSum += readTemperature();
    delay(10);
    humSum += readHumidity();
    delay(10);
    dustSum += readDust();
    sampleCount++;

    DEBUG_PRINTF("Sample #%d captured.\n", sampleCount);
}

// Function to average and upload
void uploadAverage() {
    if (sampleCount == 0) return;

    DEBUG_PRINTLN("Processing 60s average and uploading...");

    SensorData localData;
    localData.nodeId = 2;
    
    // Calculate average
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
                success = uploadQueuedData(allData, 1);
                if (!success) {
                    retryCount++;
                    delay(2000); 
                }
            }
            if (success) DEBUG_PRINTLN("Upload successful.");
        }
    }

    // Reset accumulators for next 60s cycle
    tempSum = 0;
    humSum = 0;
    dustSum = 0;
    sampleCount = 0;
}