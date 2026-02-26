#include "serverconfig.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "debugmode.h"


// -----------------------------
// Network Check
// -----------------------------
bool isNetworkAvailable() {
    if (WiFi.status() == WL_CONNECTED) return true;

    // If it dropped, force a clean reconnect
    DEBUG_PRINTLN("WiFi dropped. Reconnecting...");
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint32_t start = millis();
    // Give it up to 8 seconds to recover
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
        delay(100);
    }

    return (WiFi.status() == WL_CONNECTED);
}

// -----------------------------
// Upload SensorData Array to Local MySQL
// -----------------------------
bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    if (!isNetworkAvailable()) return false;

    bool success = true;

    for (uint8_t i = 0; i < count; i++) {
        HTTPClient http;
        http.begin(SERVER_URL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Format data exactly as your PHP script expects
        // Note: We use the raw integers/floats from your struct
        String postData = "node_id=" + String(dataArray[i].nodeId) + 
                          "&temp=" + String(dataArray[i].temperature / 100.0) + 
                          "&hum=" + String(dataArray[i].humidity / 100.0) + 
                          "&dust=" + String(dataArray[i].dust / 10.0);

        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            // Success response from your PHP script
            DEBUG_PRINTF("Upload Success: %d\n", httpResponseCode);
        } else {
            DEBUG_PRINTF("Local SQL write failed: %s\n", http.errorToString(httpResponseCode).c_str());
            success = false;
        }
        
        http.end();
    }

    // Optional: Only disconnect if you want to save power between batches
    // WiFi.disconnect(true); 
    
    return success;
}