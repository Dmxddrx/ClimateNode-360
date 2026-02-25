#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>
#include <string.h> 

// Buffer to store all received data
SensorData receivedData[MAX_NODES];
// Use volatile for variables modified in interrupts/callbacks on single-core
volatile uint8_t receivedCount = 0; 

// ESP-NOW receive callback
// Note: In newer ESP32 cores, the signature for esp_now_recv_info_t is required
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len == sizeof(SensorData)) {
        if (receivedCount < MAX_NODES) {
            // Memory copy is safe here, but keep it fast 
            // so we don't block the WiFi stack on the single core
            memcpy(&receivedData[receivedCount], data, sizeof(SensorData));
            receivedCount++;
        } else {
            // Buffer full - logic to handle overflow if necessary
        }
    }
}

// Initialize ESP-NOW
void initESPNow() {
    // C3 requires Station mode for ESP-NOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Ensure we aren't trying to connect to an AP yet

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    // Register the receive callback
    esp_now_register_recv_cb(onReceive);
    
    Serial.println("ESP-NOW Initialized on ESP32-C3");
}

// Wait for a specific window to allow slaves to send their data
void waitForSlaves(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms) {
        // yield() is critical on C3 to allow the background 
        // ESP-NOW/WiFi stack to trigger the onReceive callback
        yield(); 
        delay(10);
    }
}

// Store master’s local sensor reading into the buffer
void storeLocalReading(SensorData &data) {
    if (receivedCount < MAX_NODES) {
        receivedData[receivedCount] = data;
        receivedCount++;
    }
}