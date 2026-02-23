#include "serverconfig.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"
#define SERVER_URL "http://server/api/upload"

// Maximum queue size
#define MAX_QUEUE 100

// Simple in-memory queue for batch upload
static SensorData serverQueue[MAX_QUEUE];
static uint8_t queueCount = 0;

// ----- Queue Management -----

void addToServerQueue(SensorData &data) {
    if (queueCount < MAX_QUEUE) {
        serverQueue[queueCount++] = data;
    }
}

size_t getQueueSize() {
    return queueCount;
}

SensorData getQueuedData(uint8_t index) {
    if (index < queueCount) return serverQueue[index];
    SensorData empty = {0,0,0,0};
    return empty;
}

void clearQueue() {
    queueCount = 0;
}

// ----- Network -----

bool isNetworkAvailable() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
        delay(50); // Wait max 5 seconds
    }
    return WiFi.status() == WL_CONNECTED;
}

// ----- Upload -----

bool uploadQueuedData() {
    if (queueCount == 0) return true; // Nothing to send

    if (!isNetworkAvailable()) return false; // Network not available

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Build JSON payload
    String payload = "[";
    for (uint8_t i = 0; i < queueCount; i++) {
        char buf[128];
        snprintf(buf, sizeof(buf),
            "{\"nodeId\":%d,\"temperature\":%d,\"humidity\":%d,\"dust\":%d}",
            serverQueue[i].nodeId,
            serverQueue[i].temperature,
            serverQueue[i].humidity,
            serverQueue[i].dust);
        payload += buf;
        if (i < queueCount - 1) payload += ",";
    }
    payload += "]";

    int httpCode = http.POST(payload);

    http.end();
    WiFi.disconnect(true);

    if (httpCode > 0 && httpCode < 300) {
        // Success, clear queue
        clearQueue();
        return true;
    }

    // Fail, keep queue for next attempt
    return false;
}