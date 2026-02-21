#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>

SensorData receivedData[MAX_NODES];
uint8_t receivedCount = 0;

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(SensorData) && receivedCount < MAX_NODES) {
    memcpy(&receivedData[receivedCount], data, sizeof(SensorData));
    receivedCount++;
  }
}

void initESPNow() {
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
}

void waitForSlaves(uint32_t ms) {
  uint32_t start = millis();
  while (millis() - start < ms) {
    delay(10);
  }
}