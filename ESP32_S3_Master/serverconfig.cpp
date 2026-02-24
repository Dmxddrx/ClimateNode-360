#include "serverconfig.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>

// Wi-Fi credentials
#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"
#define SERVER_URL "http://server/api/upload"
#define QUEUE_FILE "/queue.csv"



// ----- Queue Management -----

void addToServerQueue(SensorData &data) {

    File file = SD.open(QUEUE_FILE, FILE_APPEND);
    if (!file) return;

    file.printf("%lu,%d,%d,%d,%d\n",
        data.timestamp,
        data.nodeId,
        data.temperature,
        data.humidity,
        data.dust
    );

    file.close();
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

bbool uploadQueuedData() {

    if (!SD.exists(QUEUE_FILE)) return true;

    if (!isNetworkAvailable()) return false;

    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (!file) return false;

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    String payload = "[";
    bool first = true;

    while (file.available()) {

        String line = file.readStringUntil('\n');
        if (line.length() < 5) continue;

        uint32_t timestamp;
        int nodeId, temp, hum, dust;

        sscanf(line.c_str(), "%lu,%d,%d,%d,%d",
               &timestamp, &nodeId, &temp, &hum, &dust);

        if (!first) payload += ",";
        first = false;

        char buf[128];
        snprintf(buf, sizeof(buf),
            "{\"timestamp\":%lu,\"nodeId\":%d,\"temperature\":%d,\"humidity\":%d,\"dust\":%d}",
            timestamp, nodeId, temp, hum, dust);

        payload += buf;
    }

    file.close();
    payload += "]";

    int httpCode = http.POST(payload);

    http.end();
    WiFi.disconnect(true);

    if (httpCode > 0 && httpCode < 300) {
        SD.remove(QUEUE_FILE);  // Clear queue file only if success
        return true;
    }

    return false;
}