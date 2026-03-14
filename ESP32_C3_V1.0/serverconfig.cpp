#include "serverconfig.h"
#include "debugmode.h"
#include <esp_task_wdt.h>

bool isNetworkAvailable() {
    if (WiFi.status() == WL_CONNECTED) 
    return true;

    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    WiFi.setSleep(false);   // VERY IMPORTANT FOR C3 STABILITY

    for (uint8_t i = 0; i < WIFI_COUNT; i++) {

        DEBUG_PRINTF("Trying WiFi: %s\n", wifiList[i].ssid);

        // Reset previous state
        WiFi.disconnect(true);
        delay(100);

        WiFi.begin(wifiList[i].ssid, wifiList[i].pass);

        uint32_t start = millis();

        while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
            delay(100);
            esp_task_wdt_reset();
        }

        if (WiFi.status() == WL_CONNECTED) {
            DEBUG_PRINTF("Connected to %s\n", wifiList[i].ssid);
            DEBUG_PRINTF("IP: %s\n", WiFi.localIP().toString().c_str());
            return true;
        }
        DEBUG_PRINTLN("Connection attempt failed.");
    }

    DEBUG_PRINTLN("WiFi Failed.");
    return false;
}

bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    if (!isNetworkAvailable()) return false;

    HTTPClient http;
    bool success = true;

    // ----------------------------
    // Build a single payload for all readings
    // ----------------------------
    String payload = "";
    for (uint8_t i = 0; i < count; i++) {
        if (i > 0) payload += "&";  // separate multiple entries if URL-encoded
        payload += "node_id=" + String(dataArray[i].nodeId) +
                   "&temp="    + String(dataArray[i].temperature / 100.0) +
                   "&hum="     + String(dataArray[i].humidity / 100.0) +
                   "&dust="    + String(dataArray[i].dust / 10.0);
    }

    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Connection", "keep-alive"); // optional but reduces overhead

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        DEBUG_PRINTF("Server Response: %d\n", httpResponseCode);
    } else {
        DEBUG_PRINTF("Post Failed: %s\n", http.errorToString(httpResponseCode).c_str());
        success = false;
    }

    http.end();  // close the single connection
    return success;
}