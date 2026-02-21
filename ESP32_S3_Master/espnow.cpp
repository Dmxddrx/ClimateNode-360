#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>
#include <string.h>  // For memcpy

// Buffer to store all received data
SensorData receivedData[MAX_NODES];
uint8_t receivedCount = 0;

// ESP-NOW receive callback
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len == sizeof(SensorData) && receivedCount < MAX_NODES) {
        memcpy(&receivedData[receivedCount], data, sizeof(SensorData));
        receivedCount++;
    }
}

// Initialize ESP-NOW
void initESPNow() {
    WiFi.mode(WIFI_STA);
    esp_now_init();                 // Ignore return code for production
    esp_now_register_recv_cb(onReceive);
}

// Wait for a specific window to allow slaves to send their data
void waitForSlaves(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms) delay(10);
}

// Store master’s local sensor reading into the buffer
void storeLocalReading(SensorData &data) {
    if (receivedCount < MAX_NODES) {
        receivedData[receivedCount++] = data;
    }
}