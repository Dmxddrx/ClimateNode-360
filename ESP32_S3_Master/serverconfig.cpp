#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "debugmode.h"

// -----------------------------
// Global InfluxDB client
// -----------------------------
#if defined(ESP32)
  WiFiMulti wifiMulti;
#endif

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// -----------------------------
// Network Check
// -----------------------------
bool isNetworkAvailable() {
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

    uint32_t start = millis();
    while (wifiMulti.run() != WL_CONNECTED && millis() - start < 8000) {
        delay(100);
    }

    return (WiFi.status() == WL_CONNECTED);
}

// -----------------------------
// Upload SensorData Array
// -----------------------------
bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    if (!isNetworkAvailable()) return false;

    bool success = true;

    for (uint8_t i = 0; i < count; i++) {
        Point sensor("climate_data");
        sensor.addTag("nodeId", String(dataArray[i].nodeId));
        sensor.addField("temperature", dataArray[i].temperature / 100.0);
        sensor.addField("humidity", dataArray[i].humidity / 100.0);
        sensor.addField("dust", dataArray[i].dust / 10.0);
        sensor.setTime(dataArray[i].timestamp);

        if (!client.writePoint(sensor)) {
            DEBUG_PRINTF("InfluxDB write failed: %s\n", client.getLastErrorMessage().c_str());
            success = false;
        }
    }

    WiFi.disconnect(true);  // optional: free WiFi for low-power
    return success;
}