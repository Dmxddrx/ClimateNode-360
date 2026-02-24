#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SD.h>
#include "sdcard.h"

// ------------------------------------------------------------
// Add Data To SD Queue
// ------------------------------------------------------------
void addToServerQueue(SensorData &data) {

    // Ensure queue file exists
    if (!SD.exists(QUEUE_FILE)) {
        File f = SD.open(QUEUE_FILE, FILE_WRITE);
        if (f) f.close();
        else return; // fail silently if cannot create
    }

    File file = SD.open(QUEUE_FILE, FILE_APPEND);
    if (!file) return; // fail silently if cannot open

    file.printf("%lu,%d,%hd,%hd,%hd\n",
        data.timestamp,
        data.nodeId,
        data.temperature,
        data.humidity,
        data.dust
    );

    file.close();
}

// ------------------------------------------------------------
// Network Check
// ------------------------------------------------------------
bool isNetworkAvailable() {

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
        delay(100);
    }

    return WiFi.status() == WL_CONNECTED;
}

// ------------------------------------------------------------
// Upload Queued Data To InfluxDB Cloud
// ------------------------------------------------------------
bool uploadQueuedData() {

    if (!SD.exists(QUEUE_FILE)) {
        return true; // nothing to upload
    }

    if (!isNetworkAvailable()) {
        return false;
    }

    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (!file) {
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();  // For production use proper certificate

    HTTPClient http;
    http.begin(client, INFLUXDB_URL);
    http.addHeader("Authorization", String("Token ") + INFLUXDB_TOKEN);
    http.addHeader("Content-Type", "text/plain; charset=utf-8");

    String payload = "";

    while (file.available()) {

        String line = file.readStringUntil('\n');
        if (line.length() < 5) continue;

        uint32_t timestamp;
        int nodeId;
        int16_t temp, hum, dust;

        sscanf(line.c_str(), "%lu,%d,%hd,%hd,%hd",
               &timestamp, &nodeId, &temp, &hum, &dust);

        if (timestamp == 0) continue;

        // Convert stored integers back to real float values
        float tempF = temp / 100.0;
        float humF  = hum  / 100.0;
        float dustF = dust / 10.0;

        // InfluxDB Line Protocol
        payload += "climate_data";
        payload += ",nodeId=" + String(nodeId);
        payload += " temperature=" + String(tempF, 2);
        payload += ",humidity=" + String(humF, 2);
        payload += ",dust=" + String(dustF, 2);
        payload += " ";
        payload += String(timestamp);
        payload += "\n";
    }

    file.close();

    if (payload.length() == 0) {
        return false;
    }

    int httpCode = http.POST(payload);

    http.end();
    WiFi.disconnect(true);

    if (httpCode >= 200 && httpCode < 300) {
        SD.remove(QUEUE_FILE);
        return true;
    }

    return false;
}