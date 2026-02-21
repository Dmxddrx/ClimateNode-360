#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>

uint8_t masterAddress[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};  // S3 MAC

void initESPNow() {
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, masterAddress, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void sendToMaster(SensorData &data) {
  esp_now_send(masterAddress, (uint8_t*)&data, sizeof(data));
}