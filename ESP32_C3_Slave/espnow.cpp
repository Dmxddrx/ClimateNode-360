#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>
#include <string.h>  // For memcpy

uint8_t masterAddress[] = {0xAC, 0xA7, 0x04, 0x26, 0x08, 0x9C}; // Master S3 MAC

void initESPNow() {
    WiFi.mode(WIFI_STA);
    esp_err_t initResult = esp_now_init();
    if (initResult != ESP_OK) {
        // Do nothing here, debug will be in general.cpp if needed
        return;
    }

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, masterAddress, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer); // Ignore failure here; debug in general.cpp
}

void sendToMaster(SensorData &data) {
    esp_now_send(masterAddress, (uint8_t*)&data, sizeof(data));
    // No Serial print here; debug via general.cpp
}