#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "debugmode.h"

// -----------------------------
// Global InfluxDB client
// -----------------------------
WiFiMulti wifiMulti;

// The ESP32-C3 uses the InfluxDbCloud2CACert for secure TLS connection
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// -----------------------------
// Network Check
// -----------------------------
bool isNetworkAvailable() {
    // On C3, ensure we are in Station mode for both ESP-NOW and WiFi uploads
    if (WiFi.status() == WL_CONNECTED) return true;

    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

    DEBUG_PRINTLN("Connecting to WiFi...");
    uint32_t start = millis();
    
    // 8 second timeout for WiFi connection
    while (wifiMulti.run() != WL_CONNECTED && millis() - start < 8000) {
        // Use delay to allow the single-core CPU to handle the WiFi handshake
        delay(250);
        DEBUG_PRINT(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINTLN("\nWiFi Connected");
        // Optimization for C3: Set batch size and buffer limit to save RAM
        client.setWriteOptions(WriteOptions().batchSize(5).bufferSize(15));
        return true;
    }
    
    DEBUG_PRINTLN("\nWiFi Connection Failed");
    return false;
}

// -----------------------------
// Upload SensorData Array
// -----------------------------
bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    
    if (!isNetworkAvailable()) {
        return false;
    }

    bool success = true;

    for (uint8_t i = 0; i < count; i++) {
        Point sensor("climate_data");
        sensor.addTag("nodeId", String(dataArray[i].nodeId));
        sensor.addField("temperature", dataArray[i].temperature / 100.0);
        sensor.addField("humidity", dataArray[i].humidity / 100.0);
        sensor.addField("dust", dataArray[i].dust / 10.0);
        
        // --- MATHEMATICAL PART ---
        // ESP32-C3 (RISC-V) handles 64-bit math natively and efficiently
        // Using $$inline$$ LaTeX for the conversion formula:
        // $timestamp_{NS} = timestamp_{SEC} \times 10^9$
        uint64_t timestampNS = (uint64_t)dataArray[i].timestamp * 1000000000ULL;
        sensor.setTime(timestampNS);
        // -------------------------

        // Writing to InfluxDB Cloud via HTTPS (Heavy for C3)
        if (!client.writePoint(sensor)) {
            DEBUG_PRINTF("InfluxDB write failed: %s\n", client.getLastErrorMessage().c_str());
            success = false;
        }
        
        // Yield to prevent the Watchdog Timer from triggering during SSL processing
        yield();
    }

    // On C3, keeping WiFi on consumes a lot of power. 
    // If your cycle is 30s, we keep it; if you move to deep sleep, disconnect here.
    // WiFi.disconnect(); 
    
    return success;
}