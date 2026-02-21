#include "espnow.h"
#include <esp_now.h>
#include <WiFi.h>

uint8_t masterAddress[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC}; // Replace with S3 MAC

void initESPNow() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed!");
        return;
    }
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, masterAddress, 6);
    peer.channel = 0;
    peer.encrypt = false;
    if (esp_now_add_peer(&peer) != ESP_OK) {
        Serial.println("Failed to add master peer");
    }
}

void sendToMaster(SensorData &data) {
    esp_err_t result = esp_now_send(masterAddress, (uint8_t*)&data, sizeof(data));
    if(result != ESP_OK){
        Serial.println("ESP-NOW send failed!");
    } else {
        Serial.println("Data sent to master");
    }
}