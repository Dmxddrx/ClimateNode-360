#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "debugmode.h"

#if defined(ESP32)
  WiFiMulti wifiMulti;
#endif

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Track if AP was already added to prevent memory leaks in WiFiMulti
static bool apAdded = false;

bool isNetworkAvailable() {
    // 1. Initialize WiFi mode if not already done
    if (WiFi.getMode() != WIFI_STA) {
        WiFi.mode(WIFI_STA);
    }

    // 2. Add AP only once
    if (!apAdded) {
        wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
        apAdded = true;
    }

    // 3. If already connected, return true immediately
    if (WiFi.status() == WL_CONNECTED) {
        return true;
    }

    // 4. Attempt connection
    DEBUG_PRINTLN("WiFi connecting...");
    uint32_t start = millis();
    
    // Increased timeout to 12s. C3 sometimes takes longer to DHCP than S3.
    while (wifiMulti.run() != WL_CONNECTED && millis() - start < 12000) {
        delay(500); // 500ms allows the background WiFi task to breathe
        DEBUG_PRINT("."); 
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINTLN("\nWiFi Connected!");
        DEBUG_PRINT("IP Address: ");
        DEBUG_PRINTLN(WiFi.localIP().toString().c_str());
        return true;
    } else {
        // Diagnostic: Why did it fail?
        DEBUG_PRINT("\nWiFi Failed. Status Code: ");
        DEBUG_PRINTLN(String(WiFi.status()).c_str()); 
        // 4 = WL_CONNECT_FAILED, 6 = WL_DISCONNECTED, 1 = WL_NO_SSID_AVAIL
        return false;
    }
}

bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    
    // Check network - this will now print detailed dots and IP on success
    if (!isNetworkAvailable()) return false;

    bool success = true;
    for (uint8_t i = 0; i < count; i++) {
        Point sensor("climate_data");
        sensor.addTag("nodeId", String(dataArray[i].nodeId));
        sensor.addField("temperature", dataArray[i].temperature / 100.0);
        sensor.addField("humidity", dataArray[i].humidity / 100.0);
        sensor.addField("dust", dataArray[i].dust / 10.0);
        
        uint64_t timestampNS = (uint64_t)dataArray[i].timestamp * 1000000000ULL;
        sensor.setTime(timestampNS);

        if (!client.writePoint(sensor)) {
            DEBUG_PRINTF("InfluxDB write failed: %s\n", client.getLastErrorMessage().c_str());
            success = false;
        }
    }

    // Note: Removed WiFi.disconnect(true). 
    // On the C3, keeping the connection alive is more stable than reconnecting every minute.
    return success;
}