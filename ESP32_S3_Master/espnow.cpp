#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>

// Buffer to store received data
SensorData receivedData[MAX_NODES];
uint8_t receivedCount = 0;

// Callback function when a packet is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(SensorData) && receivedCount < MAX_NODES) {
    memcpy(&receivedData[receivedCount], data, sizeof(SensorData));
    receivedCount++;
  }
}

// Initialize ESP-NOW and register callback
void initESPNow() {
  WiFi.mode(WIFI_STA);        // Station mode
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
}

// Wait for a window of time to receive data from slaves
void waitForSlaves(uint32_t ms) {
  uint32_t start = millis();
  while (millis() - start < ms) {
    delay(10);
  }
}

// Add S3 local sensor data to the same buffer
void storeLocalReading(SensorData &data) {
  if (receivedCount < MAX_NODES) {
    receivedData[receivedCount++] = data;
  }
}