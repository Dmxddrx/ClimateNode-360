#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "debugmode.h"

// -----------------------------
// Global InfluxDB client
// -----------------------------
// ESP32-C3 is part of the ESP32 family, so this define remains valid
#if defined(ESP32)
  WiFiMulti wifiMulti;
#endif

// The C3 uses the same InfluxDbCloud2CACert for secure TLS connections
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// -----------------------------
// Network Check
// -----------------------------
bool isNetworkAvailable() {
    // Force Station mode for C3
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

    uint32_t start = millis();
    // 8-second timeout is appropriate for C3 to find the AP
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
        
        // --- MATHEMATICAL PART ---
        // 64-bit math is fully supported on the C3 RISC-V architecture
        uint64_t timestampNS = (uint64_t)dataArray[i].timestamp * 1000000000ULL;
        sensor.setTime(timestampNS);
        // -------------------------

        if (!client.writePoint(sensor)) {
            DEBUG_PRINTF("InfluxDB write failed: %s\n", client.getLastErrorMessage().c_str());
            success = false;
        }
    }

    // Disconnecting helps the C3 internal radio state machine reset cleanly
    WiFi.disconnect(true);  
    return success;
}