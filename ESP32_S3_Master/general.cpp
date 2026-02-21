#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
#include "sdcard.h"
#include "serverconfig.h"
#include "debugmode.h"
#include <esp_sleep.h>
#include <Arduino.h>

// Main run function
void generalRun() {
    // 1. Read local sensors periodically
    SensorData localData;
    localData.nodeId = 0; // Master node

    localData.temperature = readTemperature();
    delay(50); // small delay between readings
    localData.humidity = readHumidity();
    delay(50);
    localData.dust = readDust();

    // Store master local data in ESP-NOW buffer
    storeLocalReading(localData);

    // 2. Wait for slave data
    waitForSlaves(8000);  // 8 sec window

    // 3. Save all collected data to SD card
    saveBufferedDataToSD(receivedData, receivedCount);

    // 4. Add data to server queue
    for(uint8_t i = 0; i < receivedCount; i++) {
        addToServerQueue(receivedData[i]); // function from serverconfig.cpp
    }

    // Clear buffer after saving/queueing
    receivedCount = 0;

    // 5. Attempt to upload queued data
    if(isNetworkAvailable()) { // check network
        uploadQueuedData();    // try uploading all queued data
    }

    // 6. Go to deep sleep until next sample
    goDeepSleep(SAMPLE_INTERVAL_SEC);
}

// Initialize general settings and debug
void initGeneral() {
    Serial.begin(115200);
    DEBUG_MODE = true; // enable/disable all debug prints
    DEBUG_PRINTLN("S3 Master Booting...");
}

// Deep sleep helper
void goDeepSleep(uint32_t seconds) {
    DEBUG_PRINTF("Going to deep sleep for %d seconds...\n", seconds);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}