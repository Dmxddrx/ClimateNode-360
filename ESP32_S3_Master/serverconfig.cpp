#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ------------------------------------------------------------
// Network Check
// ------------------------------------------------------------
bool isNetworkAvailable() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
        delay(100);
    }

    return (WiFi.status() == WL_CONNECTED);
}

// ------------------------------------------------------------
// Upload SensorData Array Directly To InfluxDB
// ------------------------------------------------------------
bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    if (!isNetworkAvailable()) return false;

    WiFiClientSecure client;
    client.setInsecure();  // Skip certificate verification

    HTTPClient http;
    http.begin(client, INFLUXDB_URL);
    http.addHeader("Authorization", String("Token ") + INFLUXDB_TOKEN);
    http.addHeader("Content-Type", "text/plain; charset=utf-8");

    String payload = "";
    for (uint8_t i = 0; i < count; i++) {
        float tempF = dataArray[i].temperature / 100.0;
        float humF  = dataArray[i].humidity / 100.0;
        float dustF = dataArray[i].dust / 10.0;

        payload += "climate_data";
        payload += ",nodeId=" + String(dataArray[i].nodeId);
        payload += " temperature=" + String(tempF, 2);
        payload += ",humidity=" + String(humF, 2);
        payload += ",dust=" + String(dustF, 2);
        payload += " ";
        payload += String(dataArray[i].timestamp);
        payload += "\n";
    }

    int httpCode = http.POST(payload);
    http.end();
    WiFi.disconnect(true);

    return (httpCode >= 200 && httpCode < 300);
}